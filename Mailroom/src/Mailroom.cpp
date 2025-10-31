//============================================================================
// Name        : Mailroom.cpp
// Author      : Auto-scaffold
// Description : Divide-out router: IntersectResults -> {Shader|Background|Black|Lit}
//============================================================================

#include <iostream>
#include <unistd.h>
using namespace std;

#include "Mailroom.hpp"

void Mailroom::local_setup() {
    // Allow slow joiners
    usleep(100*1000);
}

bool Mailroom::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload) {
    Pixel pixel; Intersection ix;
    msgpack::object obj;
    unPackPart(header, &obj); obj.convert(pixel);

    if (pixel.type == iInvalid) {
        // EOF control: forward once to all four topics so downstream consumers flush
        header->clear(); payload->clear(); msgpack::pack(header, pixel); msgpack::pack(payload, ix); sendMessage(header, payload, "Black");
        header->clear(); payload->clear(); msgpack::pack(header, pixel); msgpack::pack(payload, ix); sendMessage(header, payload, "Lit");
        header->clear(); payload->clear(); msgpack::pack(header, pixel); msgpack::pack(payload, ix); sendMessage(header, payload, "Shader");
        header->clear(); payload->clear(); msgpack::pack(header, pixel); msgpack::pack(payload, ix); sendMessage(header, payload, "Background");
        return false;
    }

    msgpack::object obj2;
    unPackPart(payload, &obj2); obj2.convert(ix);

    const bool isShadow = (pixel.type == iShadow);
    const char* topic = nullptr;
    if (isShadow) {
        // Shadow rays: hit -> Black, miss -> Lit
        topic = (ix.gothit && (ix.distance[0] < pixel.distance)) ? "Black" : "Lit";
        pixel.gothit = true;
    } else {
        // Other rays: hit -> Shader, miss -> Background
        topic = ix.gothit ? "Shader" : "Background";
    }

    // Repack and send to chosen topic
    header->clear(); payload->clear();
    msgpack::pack(header, pixel);
    msgpack::pack(payload, ix);
    sendMessage(header, payload, topic);
    return false; // we already sent the message in-place
}

void Mailroom::local_shutdown() {
    std::cout << "Mailroom shutting down... ";
}

int main(int argc, char* argv[]) {
    cout << "starting up" << endl;
    if (argc != 6) {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
    Mailroom mr(argv[1], argv[2], argv[3], argv[4], argv[5]);

    // Allow binding the subscriber in isolated/stepwise runs
    const char* bind_sub = std::getenv("MAILROOM_BIND_SUB");
    if (bind_sub && *bind_sub && *bind_sub != '0') {
        mr.forceBindSubscriber();
    }

    cout << "running" << endl;
    mr.run();
    cout << "shutting down" << endl;
    return 0;
}
