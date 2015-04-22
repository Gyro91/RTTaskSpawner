#ifndef __TASK_HPP__
#define __TASK_HPP__

#include "sched_new.h"

extern pthread_barrier_t barr;

void *task_main(void *arg);

#endif
