/*
 * Echo.hpp
 */

#ifndef ECHO_HPP_
#define ECHO_HPP_

#include "Widget.hpp"
#include "Pixel.hpp"

class Echo : public Widget {
public:
    Echo(char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint)
        : Widget(_name, _subscription, _sub_endpoint, _publication, _pub_endpoint, true, true) {}
    Echo() : Widget("", "", "", "", "", false, false) {}
    ~Echo() {}

protected:
    virtual void local_setup();
    virtual bool local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload);
    virtual void local_shutdown();
};

#endif /* ECHO_HPP_ */
