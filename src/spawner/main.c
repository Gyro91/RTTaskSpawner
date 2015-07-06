#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include "task.h"
#include "periodicity.h"
#include "json_inout.h"

#define gettid() syscall(__NR_gettid)
#define TASK_NUM_MAX 50

unsigned int thread_count;
pthread_t *thread_list;

/* calculate min among first arrival times */
long int calculate_min(int size)
{
  int i;
  long int tmp;
  long int min = ( tk[0].arrival_time[0].tv_nsec / 1000000 )
		         + ( tk[0].arrival_time[0].tv_sec * 1000 );

  for(i=1; i < size ; ++i){
    tmp = ( tk[i].arrival_time[0].tv_nsec / 1000000 )
		  + ( tk[i].arrival_time[0].tv_sec * 1000 );

    if ( tmp <= min )
      min = tmp;
  }

  return tmp;
}


void compute_response_time(int size,int jobs) /* in ms */
{
  int i,j;
  long int resp_time;
  long int a0,end_j;
  long int min_a0_0 = calculate_min(size);

  for(i=0; i < size; ++i){
    for(j=0; j < jobs ; ++j){
       /* first arrival time is after the barrier,the time
        * of the first in execution is the arrival time
        */
       if(j == 0)
         a0 = min_a0_0;
       else
         a0 = ( tk[i].arrival_time[j].tv_sec * 1000 )
    	      + ( tk[i].arrival_time[j].tv_nsec / 1000000 );
       /* finishing time for job j */
       end_j = ( tk[i].finishing_time[j].tv_sec * 1000 )
          	  + ( tk[i].finishing_time[j].tv_nsec / 1000000 );

       /* response_time for job j */
       resp_time = end_j - a0;

       printf("Response time Thread[ %ld ] ",tk[i].tid);
       printf("for job %d : %ld ms\n",j,resp_time);
    }
  }
}

/* creating task with body task_main
 * and the generator's parameters
 */

void create_task(periodic_task_attr *param)
{
  pthread_create(&thread_list[thread_count], NULL, task_main, (void *)param);

  thread_count++;
}

int main()
{
  periodic_task_attr *p;
  unsigned int i;
  unsigned int size = 50;

  thread_count = 0;

  parse_config_stdin(&p, &size);

  /* allocating memory for size thread */
  thread_list = (pthread_t *)malloc(sizeof(pthread_t) * size);

  print_pta_json(p, size);

  /* init for the barrier */
  pthread_barrier_init(&barr, NULL, size);

  /* allocating memory for size struct to record response_time */
  tk = (struct time_task *)malloc(sizeof(struct time_task)*size);

  /* allocating memory for jobs finishing times for each struct */
  for(i=0; i<size; ++i){
    memset(&tk[i],0,sizeof(struct time_task));
    tk[i].finishing_time =
    				(struct timespec *)malloc(sizeof(struct timespec)*p->jobs);
    tk[i].arrival_time =
        			(struct timespec *)malloc(sizeof(struct timespec)*p->jobs);
  }

  printf("Creating %d threads\n", size);

  for (i=0; i<size; ++i)
    create_task(&p[i]);

  while (thread_count) {
    thread_count--;
    pthread_join(thread_list[thread_count], NULL);
  }


  compute_response_time(size,p->jobs);

  /* free memory */
  free(T_body);

  for(i=0; i < size; ++i){
    free(tk[i].finishing_time);
  	free(tk[i].arrival_time);
  }

  free(tk);
  free(thread_list);
  free(p);

  return 0;
}
