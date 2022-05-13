#pragma once

#include <iostream>

#include <cxxabi.h>
#include <fmt/format.h>

namespace clt {
/**
 * Ref: https://cpplover.blogspot.com/2010/03/gcc.html
 * Ref: https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html#76957f5810098d2ffb5c62f43eda1c6d
 */
class Demangle {
private:
    int status_;
    char *demangled_;

public:
    Demangle() = delete;
    Demangle(const Demangle &) = delete;
    Demangle(Demangle &&) = delete;
    Demangle &operator=(const Demangle &) = delete;
    explicit Demangle(const std::type_info &tinfo) : status_(-3), demangled_(0)
    {
        demangled_ = abi::__cxa_demangle(tinfo.name(), 0, 0, &status_);
    }
    ~Demangle() { std::free(demangled_); }
    // operator const char *() const { return demangled_; }
    operator bool() const { return status_ == 0; }
    friend std::ostream &operator<<(std::ostream &ost, const Demangle &x)
    {
        if (x.status_ == 0) {
            ost << fmt::format("{}", x.demangled_);
        } else {
            ost << fmt::format("(invalid)");
        }
        return ost;
    }
    auto get_status() const { return status_; }
};
} // namespace clt
