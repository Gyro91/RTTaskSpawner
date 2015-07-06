#ifndef __TASK_HPP__
#define __TASK_HPP__
#include "periodicity.h"
#include "sched_new.h"

extern pthread_barrier_t barr;

/*
 * struct to record arrival time of the task
 * and his fininishing times of every jobs
 */

struct time_task{
  pthread_t tid;
  struct timespec *arrival_time; /* when a task arrives */
  struct timespec *finishing_time; /* all finishing times for every jobs */
  int index; /* to locate the actually number of jobs */
};

extern struct time_task *tk;
extern int find_time_tk(pthread_t id);

void *task_main(void *arg);

extern void do_wait_do(periodic_task_attr *pta);

struct Task_Body {
  void (*do_wait_do)(periodic_task_attr *pta);
  void(*write_and_read)(periodic_task_attr *pta);
};

extern struct Task_Body *T_body;

#endif
