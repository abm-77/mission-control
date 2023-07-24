#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <core/jobs.h>

void empty_job(Job* job, void* data) {
    printf("job\n");
}

void hello_job(Job* job, void* data) {
    printf("hi, %s\n", (char*) data);
}

typedef struct Particle {
    i32 x;
    i32 x_vel;
} Particle;

#define N_PARTICLES 100

Particle g_particles[N_PARTICLES];

void update_particles(void* data, u32 count) {
    Particle* particles = (Particle*) data;
   for (i32 i = 0; i < count; ++i) {
       particles[i].x += particles[i].x_vel;

       u32 idx = (data - (void*) &g_particles) + i;
       printf("idx: %d, x: %d, x_vel: %d\n", idx, g_particles[idx].x, g_particles[idx].x_vel);
   }
}

int main (void) {
    srand(time(NULL));

    job_system_init();

    Worker* worker = job_system_thread_worker();

    Job* root = job_create(&empty_job);
    for (int i = 0; i < _job_system.n_workers; ++i) {
        Job* j = job_create_child(root, &empty_job);
        worker_submit(worker, j);
    }
    worker_submit(worker, root);

    worker_wait(worker, root);

    Job* j = job_create(&hello_job);
    job_write_data(j, "ace", sizeof("ace"));
    worker_submit(worker, j);
    worker_wait(worker, j);
    for (i32 i = 0; i < N_PARTICLES; ++i) {
        g_particles[i].x = i*i;
        g_particles[i].x_vel = i;
    }

    Job* p = parallel_for(&g_particles, N_PARTICLES, &update_particles);
    worker_submit(worker, p);
    worker_wait(worker, p);

    for (i32 i = 0; i < N_PARTICLES; ++i) {
    }

    return 0;
}
