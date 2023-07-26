# mission-control
Work-stealing Job System in pure C using phtreads.

# How does it work?

Jobs are essentially just functions that take in a `Job*` and some data contained in a `void*`:

```
void hello_job(Job* job, void* data) {
    printf("hi, %s\n", (char*) data);
}
```

Before executing any jobs, the job system must be initialized by calling `job_system_init()`. The job
system manages all of the `Workers` behind the scenes. A `Worker` can be thought of as a thread along with 
a queue of `Jobs` to complete. By default, `mission-control` creates a `Worker` per logical core on your 
machine.

Next you can create `Workers` and `Jobs`:

```
int main (void) {
  // initialize job system
  job_system_init();

  // ask the job system for an available worker
  Worker* worker = job_system_thread_worker();

  // create a job using your function
  Job* job = job_create(&hello_job);

  ...
}
```

To give a `Job` data, you must call `job_write_data(Job*,void*, uint32_t)` with the job to write data to,
the data to write, and the size of the data to write:

```
job_write_data(job, "john", sizeof("john"));
```

To submit the job to be executed, call `worker_submit(Worker*,Job*)` to add the passed job to the passed
worker's queue. Note that the job passed may or may not be immediately executed, it is merely added to
the worker's queue. `mission-control` implements a "work-stealing" model where other workers who have no
work left in their queues may steal work from other worker's queues. Therefore, the worker you submit to
may not be the worker that actually executes the job. Because of this design, it does not matter what
queue you submit your job to, it can be processed by any of the available workers at the time.

```
worker_submit(worker, job);
```

As stated before `worker_submit` pushes the passed job to the passed worker's queue. The result of the job
will then become available at some point in the future. If you require a job to complete before doing
subsequent operations, you can call `worker_wait(Worker*,Job*)` to have a worker wait for a job to complete.

```
worker_wait(worker, job);
```

And that's it! Basics covered.

# Advanced mechanics

## Parent Jobs
`Jobs` can exist in a parent-child relationship. You can add a job as a child of another job by calling
`job_create_child(Job* parent, JobFunc job)`. By calling `worker_wait` on a job with children, it will wait for
all the child jobs to complete as well.

## Parallel-For Jobs
You can elect to have some singular process/computation done in parallel. By using 
`parallel_for(void* data, uint32_t data_size, ParFunc function)` you can have the the passed function
run on the passed data in groups. `parallel_for` will handle the splittig for you.

# Other interesting tidbits
* Workers will go to sleep if they have no jobs in there queue and no jobs available to steal
  to avoid spinning uselessly. They will be woken up when jobs become available.
* To avoid false-sharing, the `data` member of a job also doubles as padding. The result is that
  data should be the size of a cache-line minus the size of the other members in the struct.
* Jobs are allocated by thread local ring buffers. Therefore, no more than `MAX_JOB_COUNT` jobs should
  be allocated in a given frame.


