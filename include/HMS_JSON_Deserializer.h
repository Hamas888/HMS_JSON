#ifndef HMS_JSON_DESERIALIZER_H
#define HMS_JSON_DESERIALIZER_H

#include "HMS_JSON_Value.h"
#include "HMS_JSON_Exceptions.h"

namespace HMS {
    class JsonDeserializer {
        public:
            #if HMS_JSON_EXCEPTIONS_ENABLED
                static JsonValue deserialize(const std::string& src);
            #else
                static JsonValue deserialize(const std::string& src, ParseError& err_out);
            #endif

        private:
            const std::string   string;
            size_t              pos = 0;
            ErrorPos            posinfo{1,1};

            void advance();
            char peek() const;
            void skipWhitespace();

            #if HMS_JSON_EXCEPTIONS_ENABLED
                void expectChar(char c);

                JsonValue parseBool();
                JsonValue parseNull();
                JsonValue parseArray();
                JsonValue parseString();
                JsonValue parseObject();
                JsonValue parseNumber();
                JsonValue parseJsonValue();
                JsonValue deserializeInternal();
                [[noreturn]] void error(const std::string& msg);
            #else
                bool expectChar(char c, ParseError& err_out); 

                JsonValue parseBool(ParseError& err_out);
                JsonValue parseNull(ParseError& err_out);
                JsonValue parseArray(ParseError& err_out);
                JsonValue parseString(ParseError& err_out);
                JsonValue parseObject(ParseError& err_out);
                JsonValue parseNumber(ParseError& err_out);
                JsonValue parseJsonValue(ParseError& err_out);
                JsonValue deserializeInternal(ParseError& err_out);
                JsonValue parseJsonValueNoexcept(ParseError& err_out);
            #endif

            explicit JsonDeserializer(const std::string& src) : string(src), pos(0) {}
    };

}


#endif // HMS_JSON_DESERIALIZER_H