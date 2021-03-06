#pragma once

#include <stdexcept>

namespace reflect {

class member_access_error : public std::logic_error {
public:
   using std::logic_error::logic_error;
};

class invalid_requested_member_type : public member_access_error {
public:
   using member_access_error::member_access_error;
};

class member_assignment_error : public member_access_error {
public:
   using member_access_error::member_access_error;
};

class invalid_assignment_to_const : public member_assignment_error {
public:
   using member_assignment_error::member_assignment_error;
};

class invalid_non_static_assignment : public member_assignment_error {
public:
   using member_assignment_error::member_assignment_error;
};

class invalid_assignment_type : public member_assignment_error {
public:
   using member_assignment_error::member_assignment_error;
};

class invalid_assignment_to_function : public invalid_assignment_type {
public:
   using invalid_assignment_type::invalid_assignment_type;
};

class invalid_function_call : public member_access_error {
public:
   using member_access_error::member_access_error;
};

class invalid_function_arguments : public invalid_function_call {
public:
   using invalid_function_call::invalid_function_call;
};

class invalid_non_static_function_call : public invalid_function_call {
public:
   using invalid_function_call::invalid_function_call;
};

class invalid_call_to_non_callable_member : public invalid_function_call {
public:
   using invalid_function_call::invalid_function_call;
};

}
