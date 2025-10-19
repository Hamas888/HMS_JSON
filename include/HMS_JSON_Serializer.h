#ifndef HMS_JSON_SERIALIZER_H
#define HMS_JSON_SERIALIZER_H

#include "HMS_JSON_Value.h"

namespace HMS {
    class JsonSerializer {
        public:
            static std::string toString(const JsonValue& v, bool pretty=false, int indent=2);
            static void serialize(const JsonValue& v, std::ostream& out, bool pretty=false, int indent=2);

        private:
            static std::string escape(const std::string& s);
            static void serializeInternal(const JsonValue& v, std::ostream& out, bool pretty, int indent, int level);
    };
}

#endif // HMS_JSON_SERIALIZER_H