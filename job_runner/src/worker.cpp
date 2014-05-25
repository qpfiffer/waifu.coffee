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
    const int COLOR_COMPONENTS = 3;
    // Get the file size so we know how big to build our in-memory buffer
    const int file_size = utils::get_file_size(filepath);
    cout << "[-] Worker opening file of size " << file_size << endl;

    // Open jpeg
    int fd;
    fd = open(filepath.c_str(), O_RDONLY);
    assert(fd != -1);

    // Map the file into memory
    void *addr = NULL;
    addr = mmap(NULL, (size_t)file_size, PROT_READ, MAP_PRIVATE, fd, 0);

    int jpeg_subsamp, width, height;
    // Init turbo jpeg decompressor
    tjhandle tj_decompress = tjInitDecompress();

    tjDecompressHeader2(tj_decompress, (unsigned char *)addr, file_size,
                        &width, &height, &jpeg_subsamp);

    cout << "[-] Worker has opened file of width " << width
         << " and height " << height << endl;
    // Now that we know the width and height of our image we can allocate
    // our decompressed buffer
    const int DECOMPRESSED_SIZE = width * height * COLOR_COMPONENTS;
    unsigned char *memory_buffer = new unsigned char[DECOMPRESSED_SIZE];
    memset(memory_buffer, '\0', DECOMPRESSED_SIZE);

    // Decompress into memory_buffer
    tjDecompress2(tj_decompress, (unsigned char *)addr, file_size, memory_buffer,
                  width, 0, height, TJPF_RGB, TJFLAG_FASTDCT);

    // So now that we have an array of RGB components in memory, we have to do
    // stuff with it. We first need to scale the image down to what Haar2D
    // expects:
    unsigned char *scaled_image = new unsigned char[NUM_PIXELS_SQUARED * 3];
    utils::scale_image(memory_buffer, width, height, scaled_image);

    // Tidy up
    tjDestroy(tj_decompress);
    delete[] scaled_image;
    delete[] memory_buffer;
    munmap(addr, (size_t)file_size);
    close(fd);

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
