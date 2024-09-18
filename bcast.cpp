// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
// Original Author: Kyle Eide, Kyle.E.Eide@gmail.com

#include "bcast.hpp"

// compile with: g++ -shared -fPIC bcast.cpp -o libbcast.so
// if needed, append paths to boost libraries: -L/path/to/boost/lib -I/path/to/boost

// Notes || -- boost ::bind, ::function used because std:: analogs cause pointer memory leak
//          -- library not thread safe, may be in future
//          -- events safely pass through multiple shared libraries
//          -- if std:: containers are used, subscribers must be passed in as pointers
//              example:
//              class subscriber;
//              typedef boost::shared_ptr<subscriber> subscriber_ptr;
//              typedef std::vector<subscriber_ptr> s_ptr_vec;
//              s_ptr_vec svec;
//              svec.push_back(subscriber_ptr(new subscriber()));
//              svec.push_back(subscriber_ptr(new subscriber()));


namespace bcast
{

key::key(std::string event_name, std::string listener_id)
    : _event_name(event_name)
    , _listener_id(listener_id)
{}


void subcription_publisher_manager::remove(key_vec accounts)
{
    for (key_vec_iter k = accounts.begin(); accounts.end() != k; ++k)
    {
        ev_cb_map_iter it;
        it = _sub_pub_map.find( k->_event_name );
        if (_sub_pub_map.end() == it)
            std::cerr << "Report serious error in bcast library" << std::endl;
        else
        {
            for (cb_vec_iter cb_iter = it->second.begin(); it->second.end() != cb_iter; ++cb_iter)
            {
                if ((*cb_iter)->_listener_id == k->_listener_id)
                {
                    delete *cb_iter;
                    it->second.erase(cb_iter);
                    break;
                }
            }
        }
        if (it->second.empty())
            _sub_pub_map.erase(it);
    }
}

cb_vec subcription_publisher_manager::get_map(std::string event_name)
{
    ev_cb_map_iter it;
    it = _sub_pub_map.find( event_name );
    if (_sub_pub_map.end() != it)
    {
        return it->second;
    }
    else
    {
        return cb_vec();
    }
}


void account_list::add_account(key k)
{
    _accounts.push_back(k);
}

void account_list::clear()
{
    get_sub_pub_man()->remove(_accounts);
}

subcription_publisher_manager* subcription_publisher_manager::_singleton = 0;
subcription_publisher_manager* get_sub_pub_man()  // convenience accessor
{ return subcription_publisher_manager::get_sub_pub_man(); }

}   //namespace bcast
