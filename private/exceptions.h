#pragma once

#include <stdexcept>

namespace reflect {

class bad_member_access : public std::logic_error {
public:
    using std::logic_error::logic_error;
};

}
