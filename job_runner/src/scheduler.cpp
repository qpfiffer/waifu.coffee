#include <assert.h>
#include <iostream>
#include <kcpolydb.h>
#include <kcutil.h>
#include <msgpack.hpp>
#include <thread>
#include <zmq.hpp>
#include "waifu.h"
#include "utils.h"

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
    // Create a new worker, get it's ID and keep track of it.
    worker *new_worker = new worker(job);
    string worker_id = new_worker->get_id();
    this->workers[worker_id] = new_worker;
    return worker_id;
}

vector<Job> *scheduler::get_jobs_from_db() {
    vector<string> matches;
    if (!db.match_prefix(JOBS_QUEUE_PREFIX, &matches)) {
        cerr << "get error: " << db.error().name() << endl;
        return NULL;
    }

    vector<Job> *jobs = new vector<Job>;

    for (auto & val : matches) {
        Job *job = new Job;

        // Deserialize
        msgpack::unpacked msg;
        msgpack::unpack(&msg, val.data(), val.size());

        msgpack::object obj = msg.get();
        obj.convert(job);

        jobs->push_back(*job);
    }

    return jobs;
}

bool scheduler::new_query(Job new_job, ScheduleResult *out) {
    bool success = true;
    std::cout << "[-] Filepath: " << new_job["filepath"] << std::endl;

    // Begin processing query.
    string worker_id = this->process_query(new_job);
    new_job["worker_id"] = worker_id;

    // Serialize
    msgpack::sbuffer new_sjob;;
    msgpack::pack(new_sjob, new_job);

    // Create the key for this job insertion
    stringstream strm;
    strm << JOBS_QUEUE_PREFIX << worker_id;

    if (!db.set(strm.str(), new_sjob.data())) {
        std::cerr << "set error: " << db.error().name() << endl;
        success = false;
    }

    if (success) {
        string hash_str = strm.str();

        uint64_t query_key = kyotocabinet::hashmurmur(hash_str.c_str(), hash_str.size());
        char *encoded = kyotocabinet::baseencode(&query_key, sizeof(query_key));

        map<string, string> thing = {{"query_key", encoded}};
        *out = make_pair(success, thing);

        cout << "[-] Scheduled new processing job: " << encoded << endl;
    }

    return success;
}

msgpack::sbuffer *scheduler::process_request(msgpack::object *obj) {
    // Setup our response object
    ScheduleResult response_list;

    std::map<std::string, std::string> conv_request;
    obj->convert(&conv_request);

    /* Figure out how to route this request */
    if (conv_request["cmd"] == "query") {
        std::string filepath = conv_request["filepath"];
        // BE EXPLICIT. B.E. EXPLICIT!
        Job new_job = {{"filepath", filepath}};
        this->new_query(new_job, &response_list);
    } else {
        /* Don't know what that command was, fail */
        map<string, string> thing;
        response_list = make_pair(false, thing);;
    }

    msgpack::sbuffer *to_return = new msgpack::sbuffer;
    msgpack::pack(to_return, response_list);

    return to_return;
}

void scheduler::spawn_thread() {
    this->main_thread = std::thread(&scheduler::main_loop, this);
}
void scheduler::main_loop() {
    zmq::context_t context(2);
    zmq::socket_t socket(context, ZMQ_PULL);
    socket.connect(SCHEDULER_URI);

    cout << "[-] Socket bound. Scheduler init, yo." << endl;

    while(true) {
        cout << "[-] Scheduler start of loop." << endl;
        zmq::message_t request;
        assert(socket.recv(&request) == true);

        msgpack::object obj;
        msgpack::unpacked unpacked;
        utils::zmq_to_msgpack(&request, &unpacked, &obj);

        cout << "[-] Scheduler received: " << obj << endl;

        // Process the job
        msgpack::sbuffer *result = this->process_request(&obj);

        // Copy result data into response buffer
        //zmq::message_t response(strlen("OK"));
        //memcpy((void*)response.data(), "OK", strlen("OK"));
        delete result;
    }
}

