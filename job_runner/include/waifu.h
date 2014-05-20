#pragma once
#include <kcpolydb.h>
#include <msgpack.hpp>

using namespace kyotocabinet;
using namespace msgpack;
using namespace std;

// This is how we reference the job queue in Kyoto Cabinet:
#define JOBS_QUEUE_NAME "query_jobs"

// How many threads to spin up for processing jobs:
#define DEFAULT_JOB_PROCESSORS 1

namespace waifu {
    typedef map<string, string> job;

    int main_loop(int argc, char *argv[]);

    class scheduler {
        public:
            sbuffer *process_request(unpacked *request);
        private:
            /* Schedules a new job to be worked on. */
            bool new_query(job new_job);
            /* Gets all of the current jobs from the db. */
            vector<job> *get_jobs_from_db(PolyDB *db);
    };

    class worker {
    };
}
