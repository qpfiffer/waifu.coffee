#include <cstdio> //libjpeg needs size_t stuff
#include <jpeglib.h>
#include <sstream>
#include <thread>
#include "waifu.h"

using namespace waifu;

worker::worker(Job job) : main_job(job), main_thread(std::thread(&worker::work, this)) {
}

worker::~worker() {
}

bool worker::decompose_jpeg(const string &filepath) {
    // This shit is all right out of the libjpeg documentation.
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE *infile = NULL;
    infile = fopen(filepath.c_str(), "rb");
    if (infile == NULL) {
        cerr << "[X] Worker Could not open " << filepath << " for processing." << endl;
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    cout << "[-] Worker process: Found image of width " << cinfo.image_width <<
        " and height " << cinfo.image_height << endl;

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
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
        this->decompose_jpeg(filename);
        // 3. Loop through all images in the DB and find similar ones
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
