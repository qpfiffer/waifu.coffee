#include <fcntl.h>
#include <sstream>
#include <sys/mman.h>
#include <thread>
#include <turbojpeg.h>
#include <unistd.h>
#include "haar.h"
#include "utils.h"
#include "waifu.h"

using namespace waifu;

worker::worker(Job job) : main_job(job), main_thread(std::thread(&worker::work, this)) {
}

worker::~worker() {
}

bool worker::compute_jpeg_coeef(const string &filepath) {
    return true;
}

void worker::work() {
    std::cout << "[-] Worker with id " << this->main_thread.get_id()
              << " reporting in." << std::endl;
    // TODO: Actually check if it's a jpeg here.
    bool is_jpeg = true;
    if (is_jpeg) {
        // 1. Load image from filesystem
        // 2. Get coefficients
        string filename = this->main_job["filepath"];
        this->compute_jpeg_coeef(filename);
        // 3. Parallel map/reduce through all images in the DB and find similar ones 
        // 4. Store new image in scored images DB
        // 5. Send results off to main loop
        // 6. Fucking kill self
    }
}

std::string worker::get_id() {
    auto myid = this->main_thread.get_id();
    std::stringstream strm;
    strm << myid;

    return strm.str();
}
