#define _GNU_SOURCE

#include <time.h>
#include "task.h"
#include "periodicity.h"
#include <sched.h>

pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barr; /* global var barrier */

int threads_arrived = 0;  /* threads on the barr */

struct time_task *tk;   /* pointer to the memory where are recorded
 	 	 	 	 	 	 * arrival times and finishing times
 	 	 	 	 	 	 * of each threads
 	 	 	 	 	 	 */

/* finds and returns task's time_task structure */

int find_time_tk(pthread_t id)
{
  int i=-1;

  /* finds tk with tid=id */
  for(i=0; i < threads_arrived; ++i)
    if ( tk[i].tid == id )
      return i;
}


void print_time(const struct timespec *t)
{
  pthread_mutex_lock(&console_mux);
  printf("[ %ld ] Time: %lld.%ld\n", gettid(), (long long)t->tv_sec, t->tv_nsec);
  pthread_mutex_unlock(&console_mux);
}

void task_init(periodic_task_attr *pta)
{
  int r;
  struct sched_attr attr;

  attr.size = sizeof(attr);
  attr.sched_flags =    0;
  attr.sched_nice =     0;
  attr.sched_priority = 0;

  attr.sched_policy =   SCHED_DEADLINE;
  attr.sched_runtime =  pta->s_runtime;
  attr.sched_period =   pta->s_period;
  attr.sched_deadline = pta->s_deadline;



  r = sched_setattr(0, &attr, 0);
  if (r < 0) {
    pthread_mutex_lock(&console_mux);
    perror("ERROR: sched_setattr");
    printf("runtime: %lld\nperiod: %lld\ndeadline: %lld\n",
           attr.sched_runtime,
           attr.sched_period,
           attr.sched_deadline);
    pthread_mutex_unlock(&console_mux);
    pthread_exit(NULL);
  }
}

void task_body(periodic_task_attr *pta)
{
  unsigned int i;
  unsigned int every;

  every = 0;

  pthread_mutex_lock(&console_mux);
  printf("Into Task Body [ %ld ]\n", gettid());
  pthread_mutex_unlock(&console_mux);

  set_period(pta);

  for (i=0; i<pta->jobs; ++i) {

    /* Busy wait for c0 */
    busy_wait(pta->c0);

    /* Self suspension */
    if (pta->ss_every > 0)
      every = (every + 1) % pta->ss_every;
    if (every == 0)
      susp_wait(pta->ss);

    /* Wait for c1 */
    busy_wait(pta->c1);

    wait_for_period(pta);
  }
}
void stampa()
{
  int i=0;
  for(i=0; i < threads_arrived;i++)
	  printf("%ld\n",tk[i].tid);
}

void *task_main(void *arg)
{
  int rc;
  pthread_mutex_lock(&console_mux);
  printf("Thread started [ %ld ]\n", gettid());
  pthread_mutex_unlock(&console_mux);

  /* tasks must be initialized to
   * be scheduled with SCHED_DEADLINE
   */
  task_init((periodic_task_attr *)arg);

  pthread_mutex_lock(&console_mux);
  tk[threads_arrived].tid = gettid(); /* assigns a time_task to a thread */
  threads_arrived++; /* task on the barr */
  printf("Thread [ %ld ] arrived\n",gettid());
  printf("Threads on the barrier: %d\n",threads_arrived);
  pthread_mutex_unlock(&console_mux);

  /* threads wait until all arrived on barr */
  rc=pthread_barrier_wait(&barr);
  if ((rc != 0) &&
	 (rc != PTHREAD_BARRIER_SERIAL_THREAD)) {
	   printf("Could not wait on barrier\n");
	   exit(-1);
   }

  task_body((periodic_task_attr *)arg);

  pthread_mutex_lock(&console_mux);
  printf("Thread completed [ %ld ]\n", gettid());
  pthread_mutex_unlock(&console_mux);

  pthread_exit(0);
}
