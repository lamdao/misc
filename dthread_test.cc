//--------------------------------------------------------------------------
// A demonstration of old style multi-threaded programming using dthread
// wrapper utility
//--------------------------------------------------------------------------
// Author: Lam H. Dao <daohailam(at)yahoo(dot)com>
//--------------------------------------------------------------------------
// Compile & test:
// $ g++ -O2 -s -o dthread_test dthread_test.cc dthread.cc -lpthread -lstdc++
// $ ./dthread_test
//--------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include "dthread.h"
//--------------------------------------------------------------------------
class MyCalc: public DThread::Worker
{
private:
	std::vector<size_t> tmp;
public:
	A()
	{
		for (size_t i = 0; i < DThread::NBR_CPU_THREADS; i++)
			tmp.push_back(0);
	}

	// This method will be invoked inside DThread::Start
	// id (thread id), start, stop are calculated automatically
	// inside DThread and passed to this method
	void Run(size_t id, size_t start, size_t stop)
	{
		size_t n = 0;
		for (size_t i = start; i < stop; i++) {
			n += i;
		}
		std::cout << "id = " << id << ": " <<  n << '\n';
		tmp[id] = n;
	}

	size_t GetResult()
	{
		size_t r = 0;
		for (size_t i = 0; i < tmp.size(); i++)
			r += tmp[i];
		return r;
	}
};
//--------------------------------------------------------------------------
int main()
{
	MyCalc calc;

	DThread::CalcWorkLoads(1000003);
	DThread::Start(&calc);

	std::cout << "Result = " << calc.GetResult() << '\n';
	return 0;
}
