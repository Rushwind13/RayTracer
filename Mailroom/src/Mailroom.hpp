/*
 * Mailroom.hpp
 *
 * A simple divide-out actor: subscribes to an aggregated IntersectResults stream
 * and routes messages to Shader/Background/Black/Lit topics according to hit/miss and ray type.
 */

#ifndef MAILROOM_HPP_
#define MAILROOM_HPP_
#include "Widget.hpp"
#include "Pixel.hpp"
#include "Intersection.hpp"

class Mailroom : public Widget {
public:
    Mailroom(char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint)
        : Widget(_name, _subscription, _sub_endpoint, _publication, _pub_endpoint, false, false) {}
    ~Mailroom() {}

protected:
    virtual void local_setup();
    virtual bool local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload);
    virtual void local_shutdown();
};

#endif /* MAILROOM_HPP_ */
