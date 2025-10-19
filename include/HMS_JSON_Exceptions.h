#ifndef HMS_JSON_EXCEPTIONS_H
#define HMS_JSON_EXCEPTIONS_H

#include "HMS_JSON_Config.h"
#if HMS_JSON_EXCEPTIONS_ENABLED
#include <stdexcept>
#endif

namespace HMS {
    struct ErrorPos {
        int line = 1;
        int col  = 1;
    };

    #if HMS_JSON_EXCEPTIONS_ENABLED
        struct ParseError : public std::runtime_error {
            ErrorPos pos;
            ParseError(const std::string& msg, ErrorPos p) : std::runtime_error(msg), pos(p) {}
        };
    #else
        struct ParseError {
            std::string what;
            ErrorPos pos;
            ParseError() = default;
            ParseError(const std::string& w, ErrorPos p) : what(w), pos(p) {}
            explicit operator bool() const { return !what.empty(); }
        };
    #endif

}

#endif // HMS_JSON_EXCEPTIONS_H