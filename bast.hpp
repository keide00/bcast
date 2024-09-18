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

#ifndef _BCAST_H_
#define _BCAST_H_

// compile with: g++ -shared -fPIC bcast.cpp -o libbcast.so

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <utility>
#include <typeinfo>

#ifdef __USE_BOOST
#include <boost/function.hpp>
#include <boost/bind.hpp>
#else
#include <functional>
#endif

#if defined (MAKE_SHARED_LIBRARY) && defined (_WIN32) // for windows ...
#   define _LIB_EXPORT_ __declspec(dllexport)
#   define _LIB_IMPORT_ __declspec(dllimport)
#elif defined (_WIN32)
#   define _LIB_EXPORT_ __declspec(dllimport)
#   define _LIB_IMPORT_
#else
#   define _LIB_EXPORT_     // for linux
#   define _LIB_IMPORT_
#endif

namespace bcast
{

class callback_base         // base class for storing pointers to derived template class
{
public:
    std::string _event_name;
    std::string _listener_id;
};

template<typename derived>
class callback : public callback_base   // callback that is actually used
{
public:
    callback(std::string listener_id_, std::function<void(const derived&)> cb_)
    {
        _cb = cb_;
        _listener_id = listener_id_;
    }

    std::function<void(const derived&)> _cb;
};

typedef std::vector<callback_base*> cb_vec;
typedef cb_vec::iterator cb_vec_iter;
typedef std::map<std::string, cb_vec> ev_cb_map;
typedef ev_cb_map::iterator ev_cb_map_iter;


class key
{
public:

    key(std::string event_name, std::string listener_id);

    std::string _event_name;
    std::string _listener_id;
};

typedef std::vector<key> key_vec; // held in account_list class
typedef key_vec::iterator key_vec_iter;


class subcription_publisher_manager  // singleton that manages the connections between events to callbacks
{
public:

    template <typename derived>
    key add(std::function<void(const derived&)> cb_, std::string event_name, std::string listener_id)
    {
        ev_cb_map_iter it;
        it = _sub_pub_map.find( event_name );
        if (_sub_pub_map.end() == it)
        {
            _sub_pub_map.insert(std::pair<std::string, cb_vec>(event_name, cb_vec() ) );
            it = _sub_pub_map.find( event_name );
            it->second.push_back(new callback<derived>(listener_id, cb_));
        }
        else
        {
            cb_vec_iter cb_iter = it->second.begin();
            uint32_t already_in_map(0);
            for ( ; it->second.end() != cb_iter; ++cb_iter)
                 if( (*cb_iter)->_listener_id == listener_id)
                    already_in_map = 1;
            if (0 == already_in_map)
                it->second.push_back(new callback<derived>(listener_id, cb_));
        }

        return key(event_name, listener_id);
    }

    void remove(key_vec accounts);
    cb_vec get_map(std::string event_name);

    inline static subcription_publisher_manager* get_sub_pub_man()
    {
        if (0 == _singleton)
            _singleton = new subcription_publisher_manager();
        return _singleton;
    }

private:

    ev_cb_map _sub_pub_map;
    static subcription_publisher_manager* _singleton;
};


subcription_publisher_manager* get_sub_pub_man() ; // convenience accessor


template <typename derived>
class _LIB_EXPORT_ bevent     // base class for unique events
{
public:

    void send()
    {
        std::string event_name = typeid(derived).name();
        cb_vec derived_cb_vec = get_sub_pub_man()->get_map(event_name);

        if (derived_cb_vec.empty())
            std::cout << "bcast library- Attempting to send event (" << event_name << ") with no listeners." << std::endl;
        cb_vec_iter cb_iter = derived_cb_vec.begin();
        for ( ; derived_cb_vec.end() != cb_iter; ++cb_iter)
            ((callback<derived>*)(*cb_iter))->_cb((const derived&) *this); // pull the trigger
    }

    template <typename object>
    static key add(object* Obj, void (object::*callback)(const derived& ev) )
    {
        std::string name( typeid(derived).name() );
        std::stringstream ss;
        ss << Obj;  
        std::string listener_id = ss.str();
	    std::function<void(const derived&)> cb_ = std::bind( callback, Obj, std::placeholders::_1);
        return get_sub_pub_man()->add( cb_, name, listener_id);
    }

    static key add(void (*callback)(const derived& ev) )
    {
        std::string name( typeid(derived).name() );
        std::stringstream ss;
        ss << typeinfo(callback).name();  
        std::string listener_id = ss.str();
        return get_sub_pub_man()->add( callback, name, listener_id);
    }
};

class account_list  // held by subscriber objects
{
public:
    void add_account(key k);
    void clear();

private:
    key_vec _accounts; // keys to be removed at object disinstantiation
};


}   //namespace bcast


#endif   // #define _BCAST_H_
