#pragma once
#include <kcpolydb.h>
#include <msgpack.hpp>
#include <thread>
#include <utility>

using namespace kyotocabinet;
using namespace msgpack;
using namespace std;

// Each job uses this prefix when being inserted/read from the db.
#define JOBS_QUEUE_PREFIX "query_job"

// The KC DB where jobs are serialized
#define JOBS_DB "jobs.kct"

// How many threads to spin up for processing jobs:
#define DEFAULT_JOB_PROCESSORS 1

#define DEFAULT_URI "tcp://*:1979"
#define SCHEDULER_URI "ipc://scheduler.push"

namespace waifu {
    typedef map<string, string> Job;
    typedef pair<bool, map<string, string>> ScheduleResult;

    int main_loop(int argc, char *argv[]);

    /* The main worker object.
     * This class is responsible for processing queries, adding new images,
     * etc.
     */
    class worker {
        public:
            worker(Job job);
            ~worker();

            /* Returns a serialized string uniquely identifying this worker. */
            string get_id();
        private:
            Job main_job;
            std::thread main_thread;

            void work();
    };

    /* The main scheduler object.
     * This class is responsible for creating new workers, making sure Jobs
     * are serialized and communicating with the main loop about whats going
     * on.
     */
    class scheduler {
        public:
            scheduler();
            ~scheduler();

            void spawn_thread();
        private:
            /* Jobs database */
            PolyDB db;
            /* Tracks all allocated workers */
            unordered_map<string, worker *> workers;
            /* The main scheduling process thread. */
            thread main_thread;

            /* Gets all of the current jobs from the db. */
            vector<Job> *get_jobs_from_db();
            /* The main thread proc that controls interaction with other nodes. */
            void main_loop();
            /* Schedules a new job to be worked on. */
            bool new_query(Job new_job, ScheduleResult *out);
            /* Responsible for turning strings like add, query, delete, etc.
             * into useful commands. */
            sbuffer *process_request(msgpack::object *request);
            /* Processes a job. Returns the string representation of the thread working on the job. */
            string process_query(Job job);
    };
}
