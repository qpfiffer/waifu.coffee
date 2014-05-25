#include <cmath>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include "haar.h"
#include "utils.h"

using namespace waifu;

void utils::zmq_to_msgpack(zmq::message_t *request, msgpack::unpacked *unpacked_body, msgpack::object *obj) {
    msgpack::unpack(unpacked_body, (char *)request->data(), request->size());
    *obj = unpacked_body->get();
}

int utils::get_file_size(const std::string &filepath) {
    int fd;
    fd = open(filepath.c_str(), O_RDONLY);
    if (fd == -1)
        return -1;

    struct stat sb;
    if (fstat(fd, &sb) == -1)
        return -1;
    close(fd);

    return (int)sb.st_size;
}

/* Scale an image to what haar2D expects, averaging the pixels along the
 * way. */
void utils::scale_image(const unsigned char *source_array, const int &s_width, const int &s_height,
                 unsigned char *dest_array) {
    // Used for looping through our array

    // NUM_PIXELS from haar.h
    const double w_ratio = s_width / NUM_PIXELS;
    const double h_ratio = s_height / NUM_PIXELS;

    const int w_ratio_i = floor(w_ratio);
    const int h_ratio_i = floor(h_ratio);

    const double wxh = w_ratio * h_ratio;

    int x,y,xi,yi;
    for (y = 0; y < s_width; y++) {
        for (x = 0; x < s_height; x++){
            unsigned int red = 0;
            unsigned int green = 0;
            unsigned int blue = 0;

            int srcx, srcy;
            // we have to use here the floating point value w_ratio, h_ratio
            // otherwise towards the end it can get a little wrong
            // this multiplication can be optimized similarly to Bresenham's line
            srcx = floor(x * w_ratio);
            srcy = floor(y * h_ratio);

            // here we use floored value otherwise it might overflow src bitmap
            for (yi = 0; yi < h_ratio_i; yi++) {
                for (xi = 0; xi < w_ratio_i; xi++) {
                    // The array we're looking at is essentially
                    // [R,G,B,R,G,B,R,G,B] so we need to compute each
                    // part.
                    // Reference a 1D array like it's 2D:
                    const int computed_pos = s_width * (srcx + xi) + (srcy + yi);
                    red += source_array[computed_pos];
                    green += source_array[computed_pos + 1];
                    blue += source_array[computed_pos + 2];
                }
            }
            const int dest_pos = NUM_PIXELS * x + y;
            dest_array[dest_pos] = red / wxh;
            dest_array[dest_pos + 1] = green / wxh;
            dest_array[dest_pos + 2] = blue / wxh;
        }
    }
}
