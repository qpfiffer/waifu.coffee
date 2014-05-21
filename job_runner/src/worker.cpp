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
    // 1. Load image from db
    // 2. Get coefficients
    // 3. Loop through all images in the DB and find similar ones
    // 4. Store new image in scored images DB
    // 5. Send results off to main loop
    // 6. Fucking kill self
}

std::string worker::get_id() {
    auto myid = this->main_thread.get_id();
    std::stringstream strm;
    strm << myid;

    return strm.str();
}
