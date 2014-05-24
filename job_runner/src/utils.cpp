#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
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
