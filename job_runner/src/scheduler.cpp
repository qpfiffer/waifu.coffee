#include <iostream>
#include <kcpolydb.h>
#include <msgpack.hpp>
#include <thread>
#include <zmq.hpp>
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

bool scheduler::new_query(Job new_job) {
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
        return false;
    }

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

void scheduler::spawn_thread() {
    this->main_thread = std::thread(&scheduler::main_loop, this);
}
void scheduler::main_loop() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PULL);
    socket.bind(SCHEDULER_URI);

    while(true) {
        zmq::message_t request;
        socket.recv(&request);

        // Convert to string
        std::string raw_data = static_cast<char*>(request.data());

        // Convert to msgpack object
        msgpack::unpacked unpacked_body;
        msgpack::unpack(&unpacked_body, raw_data.data(), raw_data.size());

        // Process the job
        msgpack::sbuffer *result = this->process_request(&unpacked_body);

        // Copy result data into response buffer
        //zmq::message_t response(result->size());
        //memcpy((void *)response.data(), result->data(), result->size());
        delete result;
    }
}

