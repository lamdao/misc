//--------------------------------------------------------------------------
// Old style (before c++11) wrapper class/utility functions
// supporting for multi-threaded programming
//--------------------------------------------------------------------------
// Author: Lam H. Dao <daohailam(at)yahoo(dot)com>
//--------------------------------------------------------------------------
#ifndef __DTHREAD_OLDSTYLE_H
#define __DTHREAD_OLDSTYLE_H
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#ifdef __linux__
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#define DThreadHandle	pthread_t
#define DThreadParam	void*
#define DThreadApi		void*
typedef void* (*DThreadFx)(void *);
#else
#include <windows.h>
#define DThreadHandle	HANDLE
#define DThreadApi		DWORD WINAPI
#define DThreadParam	LPVOID
#define DThreadFx		LPTHREAD_START_ROUTINE
#endif
//--------------------------------------------------------------------------
namespace DThread {
//--------------------------------------------------------------------------
class Worker
{
public:
	virtual void Run(size_t id, size_t start, size_t stop) = 0;
};
//--------------------------------------------------------------------------
void CalcWorkLoads(size_t total);
//--------------------------------------------------------------------------
void Start(DThread::Worker *w);
//--------------------------------------------------------------------------
extern size_t NBR_CPU_THREADS;
//--------------------------------------------------------------------------
} // DThread
//--------------------------------------------------------------------------
#endif
