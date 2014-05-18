#pragma once
#include <kcpolydb.h>
#include <msgpack.hpp>

using namespace kyotocabinet;
using namespace msgpack;
using namespace std;

#define JOBS_QUEUE_NAME "query_jobs"

namespace waifu {
    typedef map<string, string> job;

    int main_loop(int argc, char *argv[]);

    class waifuScheduler {
        public:
            sbuffer *process_request(unpacked *request);
        private:
            bool new_query(job new_job);
            vector<job> *get_jobs_from_db(PolyDB *db);
    };
}
