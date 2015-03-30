/*
 * Periodicity.h
 *
 *  Created on: Nov 24, 2012
 *      Author: Alessio Balsini
 */

#ifndef __PERIODICITY_H__
#define __PERIODICITY_H__

#include <time.h>
#include <pthread.h>

/*
 * All the values are in ms
 */
typedef struct periodic_task_attr_ {
	void * arg;			// task argument
    int dmiss;			// number of deadline misses

    // The task acts as follows:
    //
    //    c0     dd   c1    s_runtime    deadline
    // |#######------####_______)___________| . . .

    int jobs;           // Number of jobs for each task
    int ss_every;       // The task Self-Suspends after every ss_every
                        //   activations
    int ss;             // The task Self-Suspends after ss_every activations
    int c0;             // Duration of C0
    int c1;             // Duration of C1

    int period;			// task period
    int deadline;		// task deadline

    int s_period;       // server period
    int s_deadline;     // server deadline
    int s_runtime;      // server runtime that will be used
                        //   by CBS for bandwidth management

	struct timespec at;	// next activation time
    struct timespec dl;	// abs. deadline
} periodic_task_attr;

void set_period(periodic_task_attr * periodic_task_attribute);
void wait_for_period(periodic_task_attr * periodic_task_attribute);

#endif /* __PERIODICITY_H__ */
