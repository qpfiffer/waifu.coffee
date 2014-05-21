#include <iostream>
#include <kcpolydb.h>
#include <msgpack.hpp>
#include <thread>
#include "waifu.h"

using namespace kyotocabinet;
using namespace msgpack;
using namespace std;
using namespace waifu;

scheduler::scheduler() {
    if (!db.open(JOBS_DB, PolyDB::OWRITER | PolyDB::OCREATE)) {
        std::cerr << "[X] Could not open DB: " << db.error().name() << std::endl;
    }
}

scheduler::~scheduler() {
    db.close();
}

string scheduler::process_query(Job job) {
    worker *new_worker = new worker(job);
    this->workers.push_back(new_worker);

    return new_worker->get_id();
}

vector<Job> *scheduler::get_jobs_from_db() {
    std::string value;
    if (!db.get(JOBS_QUEUE_NAME, &value)) {
        cerr << "get error: " << db.error().name() << endl;
        return NULL;
    }

    // Deserialize
    msgpack::unpacked msg;
    msgpack::unpack(&msg, value.data(), value.size());
    msgpack::object obj = msg.get();

    //cout << "Current jobs: " << obj << endl;

    vector<Job> *jobs = new vector<Job>;
    obj.convert(jobs);

    return jobs;
}

bool scheduler::new_query(Job new_job) {
    std::cout << "[-] Filepath: " << new_job["filepath"] << std::endl;

    // Get the list of jobs to-be-processed
    vector<Job> *jobs = this->get_jobs_from_db();
    if (jobs == NULL) {
        jobs = new vector<Job>;
    }

    // Begin processing query.
    string worker_id = this->process_query(new_job);
    new_job["worker_id"] = worker_id;
    jobs->push_back(new_job);

    // Serialize
    msgpack::sbuffer new_jobs_list;
    msgpack::pack(new_jobs_list, *jobs);

    if (!db.set(JOBS_QUEUE_NAME, new_jobs_list.data())) {
        std::cerr << "set error: " << db.error().name() << endl;
        delete jobs;
        return false;
    }

    delete jobs;
    return true;
}

msgpack::sbuffer *scheduler::process_request(msgpack::unpacked *request) {
    // Setup our response object
    std::map<std::string, bool> response_list;
    response_list["success"] = false;

    // Convert the request into something useful
    msgpack::object obj = request->get();

    std::map<std::string, std::string> conv_request;
    obj.convert(&conv_request);

    /* Figure out how to route this request */
    if (conv_request["cmd"] == "query") {
        std::string filepath = conv_request["filepath"];
        // BE EXPLICIT. B.E. EXPLICIT!
        Job new_job = {{"hash", ""}, {"filepath", filepath}};
        response_list["success"] = this->new_query(new_job);
    } else {
        /* Don't know what that command was, fail */
        response_list["success"] = false;
    }

    msgpack::sbuffer *to_return = new msgpack::sbuffer;
    msgpack::pack(to_return, response_list);

    return to_return;
}

