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

#ifndef _EXAMPLE_EVENT_H_
#define _EXAMPLE_EVENT_H_

#include "bcast.hpp"

struct simple_data_event : public bcast::bevent<simple_data_event>
{
    simple_data_event(int important_number_)
        : important_number(important_number_)
    {}

    int important_number;
};

struct string_message_event : public bcast::bevent<string_message_event>
{
    string_message_event(std::string message_)
        : message(message_)
    {}

    std::string message;
};

struct float_message_event : public bcast::bevent<float_message_event>
{
    float_message_event(float num_)
        : num(num_)
    {}

    float num;
};


struct class_1_to_2_event : public bcast::bevent<class_1_to_2_event>
{
    class_1_to_2_event(std::string message_, int num_)
        : message(message_)
        , num(num_)
    {}

    int num;
    std::string message;
};

struct class_2_to_1_event : public bcast::bevent<class_2_to_1_event>
{
    class_2_to_1_event(std::string message_, int num_)
        : message(message_)
        , num(num_)
    {}

    int num;
    std::string message;
};

#endif      //_EXAMPLE_EVENT_H_
