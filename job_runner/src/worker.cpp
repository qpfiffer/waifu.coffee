#include <turbojpeg.h>
#include <sstream>
#include <thread>
#include "utils.h"
#include "waifu.h"

using namespace waifu;

worker::worker(Job job) : main_job(job), main_thread(std::thread(&worker::work, this)) {
}

worker::~worker() {
}

bool worker::decompose_jpeg(const string &filepath) {
    // Get the file size so we know how big to build our in-memory buffer
    const int file_size = utils::get_file_size(filepath);
    cout << "[-] Worker opening file of size " << file_size << endl;

    /*
    mmap

    long unsigned int jpeg_size;
    int jpegSubsamp, width, height;

    tjhandle tj_decompress = tjInitDecompress();
    tjDecompressHeader2(_jpegDecompressor, _compressedImage, _jpegSize, &width, &height, &jpegSubsamp);
    unsigned char memory_buffer[width*height*3];

    tjDecompress2(_jpegDecompressor, _compressedImage, _jpegSize, buffer, width, 0, height, TJPF_RGB, TJFLAG_FASTDCT);

    tjDestroy(_jpegDecompressor);
    */

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
