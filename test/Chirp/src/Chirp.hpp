/*
 * Chirp.hpp
 *
 *  Created on: Oct 10, 2019 (this day was a 10/10 :D)
 *      Author: jimbo
 */

#ifndef CHIRP_HPP_
#define CHIRP_HPP_
#include "Widget.hpp"

class Chirp : public Widget
{
public:
	Chirp( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint )
	{
	};
	~Chirp(){};

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload ) { return false; };
	virtual void local_shutdown(){};

private:
};


#endif /* CHIRP_HPP_ */
