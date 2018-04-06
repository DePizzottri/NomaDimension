#ifndef NOMA_DIMENSION_job_management_INCLUDED
#define NOMA_DIMENSION_job_management_INCLUDED

#include "blockingconcurrentqueue.h"

#include <functional>

using namespace std;

using Job = function<void()>;

using Queue = moodycamel::BlockingConcurrentQueue<Job>;

void consumer(Queue & q, atomic_bool & stopped) {
    while (!stopped) {
        Job job;
        while (q.wait_dequeue_timed(job, std::chrono::milliseconds(100))) {
            job();
        }
    }
}

#endif// NOMA_DIMENSION_job_management_INCLUDED
