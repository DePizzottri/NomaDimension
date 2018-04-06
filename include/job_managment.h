#ifndef NOMA_DIMENSION_job_management_INCLUDED
#define NOMA_DIMENSION_job_management_INCLUDED

#include "concurrentqueue.h"

#include <functional>

using namespace std;

using Job = function<void()>;

using Queue = moodycamel::ConcurrentQueue<Job>;

void consumer(Queue & q, atomic_bool & stopped) {
    while (!stopped) {
        Job job;
        while(q.try_dequeue(job))
            job();
    }
}

#endif NOMA_DIMENSION_job_management_INCLUDED