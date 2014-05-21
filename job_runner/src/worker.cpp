#include <sstream>
#include <thread>
#include "waifu.h"

using namespace waifu;

worker::worker(Job job) : main_job(job), main_thread(std::thread(&worker::work, this)) {
}

worker::~worker() {
}

void worker::work() {
    std::cout << "[-] Worker with id " << this->main_thread.get_id()
              << " reporting in." << std::endl;
}

std::string worker::get_id() {
    auto myid = this->main_thread.get_id();
    std::stringstream strm;
    strm << myid;
    return strm.str();
}
