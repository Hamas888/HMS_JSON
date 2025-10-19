#ifndef HMS_JSON_VALUE_H
#define HMS_JSON_VALUE_H
#include "HMS_JSON_Config.h"

namespace HMS {
    struct JsonValue;

    using JsonArray  = std::vector<JsonValue>;
    using JsonObject = std::map<std::string, JsonValue>;

    struct JsonValue {
        using Variant = std::variant<
            std::nullptr_t, 
            bool, double, 
            std::string, 
            JsonObject,
            JsonArray
        >;

        Variant JsonVariant;

        // Constructors
        JsonValue()                             : JsonVariant(nullptr)                      {}
        JsonValue(int i)                        : JsonVariant(static_cast<double>(i))       {}
        JsonValue(bool b)                       : JsonVariant(b)                            {}
        JsonValue(double d)                     : JsonVariant(d)                            {}
        JsonValue(const char* s)                : JsonVariant(std::string(s))               {}
        JsonValue(JsonArray&& a)                : JsonVariant(std::move(a))                 {}
        JsonValue(JsonObject&& o)               : JsonVariant(std::move(o))                 {}
        JsonValue(std::nullptr_t)               : JsonVariant(nullptr)                      {}
        JsonValue(std::string&& s)              : JsonVariant(std::move(s))                 {}
        JsonValue(const JsonArray& a)           : JsonVariant(a)                            {}
        JsonValue(const JsonObject& o)          : JsonVariant(o)                            {}
        JsonValue(const std::string& s)         : JsonVariant(s)                            {}


        bool isNull()   const { return std::holds_alternative<std::nullptr_t>(JsonVariant);  }
        bool isBool()   const { return std::holds_alternative<bool>(JsonVariant);            }
        bool isArray()  const { return std::holds_alternative<JsonArray>(JsonVariant);       }
        bool isNumber() const { return std::holds_alternative<double>(JsonVariant);          }
        bool isString() const { return std::holds_alternative<std::string>(JsonVariant);     }
        bool isObject() const { return std::holds_alternative<JsonObject>(JsonVariant);      }


        
        
        bool asBool()                  const { return std::get<bool>(JsonVariant);           }
        double asNumber()              const { return std::get<double>(JsonVariant);         }
        const JsonArray& asArray()     const { return std::get<JsonArray>(JsonVariant);      }
        const JsonObject& asObject()   const { return std::get<JsonObject>(JsonVariant);     }
        const std::string& asString()  const { return std::get<std::string>(JsonVariant);    }

        JsonArray& getArray() {  if (!isArray()) JsonVariant = JsonArray{};
            return std::get<JsonArray>(JsonVariant);
        }

        JsonObject& getObject() {
            if (!isObject()) JsonVariant = JsonObject{};
            return std::get<JsonObject>(JsonVariant);
        }

        JsonValue& operator[](const std::string& key) {
            return getObject()[key];
        }
        JsonValue& operator[](std::size_t idx) {
            auto &a = getArray();
            if (idx >= a.size()) a.resize(idx + 1);
            return a[idx];
        }

    };
}

#endif // HMS_JSON_VALUE_H