//--------------------------------------------------------------------------
// Old style (before c++11) wrapper class/utility functions
// supporting for multi-threaded programming
//--------------------------------------------------------------------------
// Author: Lam H. Dao <daohailam(at)yahoo(dot)com>
//--------------------------------------------------------------------------
#include "dthread.h"
//--------------------------------------------------------------------------
#include <vector>
//--------------------------------------------------------------------------
namespace DThread {
//--------------------------------------------------------------------------
#ifdef _WIN32
inline int GetNumberOfProcessors()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	DWORD np = sysinfo.dwNumberOfProcessors;
	return np > 0 ? np : 1;
}
#else	// pthread
inline int GetNumberOfProcessors()
{
	return sysconf(_SC_NPROCESSORS_CONF);
}
#endif
//--------------------------------------------------------------------------
size_t NBR_CPU_THREADS = GetNumberOfProcessors();
//--------------------------------------------------------------------------
#ifdef _WIN32
//--------------------------------------------------------------------------
inline DThreadHandle Create(DThreadFx fx, int id)
{
	DThreadHandle h = CreateThread(NULL, 0,
						fx, (DThreadParam)id, CREATE_SUSPENDED, 0);
	SetThreadAffinityMask(h, 1 << (id % NBR_CPU_THREADS));
	ResumeThread(h);
	return h;
}
//--------------------------------------------------------------------------
inline void Join(const std::vector<DThreadHandle> &threads)
{
	WaitForMultipleObjects(threads.size(), &threads[0], TRUE, INFINITE);
	for (size_t n = 0; n < threads.size(); n++) {
		CloseHandle(threads[n]);
	}
}
//--------------------------------------------------------------------------
#else	// pthread
//--------------------------------------------------------------------------
inline DThreadHandle Create(DThreadFx fx, int id)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	cpu_set_t cmask;
	CPU_ZERO(&cmask);
	CPU_SET((id % NBR_CPU_THREADS), &cmask);

	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cmask);

	DThreadHandle h;
	pthread_create(&h, &attr, fx, (DThreadParam)id);
	return h;
}
//--------------------------------------------------------------------------
inline void Join(const std::vector<DThreadHandle> &threads)
{
	for (int n = 0; n < threads.size(); n++) {
		pthread_join(threads[n], 0);
	}
}
//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
static size_t total = 0;
static size_t load = 0;
//--------------------------------------------------------------------------
void CalcWorkLoads(size_t total)
{
	DThread::total = total;
	load = (total / NBR_CPU_THREADS) + ((total % NBR_CPU_THREADS) > 0);
}
//--------------------------------------------------------------------------
static DThread::Worker *worker;
//--------------------------------------------------------------------------
static DThreadApi Execute(DThreadParam param)
{
	size_t idx = (size_t)param;
	size_t start = idx * load;
	size_t stop = start + load;
	if (stop > total) {
		stop = total;
	}
	worker->Run(idx, start, stop);
	return 0;
}
//--------------------------------------------------------------------------
void Start(DThread::Worker *w)
{
	static std::vector<DThreadHandle> threads(NBR_CPU_THREADS);

	if (threads.size() != NBR_CPU_THREADS) {
		threads = std::vector<DThreadHandle>(NBR_CPU_THREADS);
	}

	// Mutex lock can be added here to avoid multiple calls of DThread::Start
	// However, in dedicated computing system, one computing task should
	// occupied all computing resources to finish the task asap before
	// passing to another tasks. IOW, this function shouldn't be called
	// concurrently from different threads.

	worker = w;
	for (size_t n = 0; n < NBR_CPU_THREADS; n++) {
		threads[n] = Create(Execute, n);
	}
	Join(threads);
	worker = NULL;
}
//--------------------------------------------------------------------------
} // DThread

