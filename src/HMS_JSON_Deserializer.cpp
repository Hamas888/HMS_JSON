#include "HMS_JSON_Deserializer.h"

namespace HMS {
    #if HMS_JSON_EXCEPTIONS_ENABLED
        JsonValue JsonDeserializer::deserialize(const std::string& src) {
            JsonDeserializer deser{src};
            return deser.deserializeInternal();
        }

        [[noreturn]] void JsonDeserializer::error(const std::string& msg) {
            throw ParseError(msg, posinfo);
        }

        JsonValue JsonDeserializer::deserializeInternal() {
            skipWhitespace();
            JsonValue v = parseJsonValue();
            skipWhitespace();
            if (pos != string.size()) error("Trailing data after JSON");
            return v;
        }

        JsonValue JsonDeserializer::parseJsonValue() {
            if (pos >= string.size()) error("Unexpected end of input");
            char c = string[pos];
            if (c == 'n') return parseNull();
            if (c == 't' || c == 'f') return parseBool();
            if (c == '"') return parseString();
            if (c == '[') return parseArray();
            if (c == '{') return parseObject();
            if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber();
            error(std::string("Unexpected character '") + c + "'");
            return JsonValue{};
        }

        JsonValue JsonDeserializer::parseNumber() {
            size_t start = pos;
            if (string[pos] == '-') advance();
                while (pos < string.size() && std::isdigit(static_cast<unsigned char>(string[pos]))) advance();
                if (pos < string.size() && string[pos] == '.') {
                    advance();
                    while (pos < string.size() && std::isdigit(static_cast<unsigned char>(string[pos]))) advance();
                }
                if (pos < string.size() && (string[pos] == 'e' || string[pos] == 'E')) {
                    advance();
                    if (string[pos] == '+' || string[pos] == '-') advance();
                while (pos < string.size() && std::isdigit(static_cast<unsigned char>(string[pos]))) advance();
            }
            std::string tok = string.substr(start, pos - start);
            try {
                double d = std::stod(tok);
                return JsonValue(d);
            } catch (...) {
                error("Invalid number format");
                return JsonValue{};
            }
        }

        JsonValue JsonDeserializer::parseString() {
            expectChar('"');
            std::string out;
            bool found_closing_quote = false;
            while (pos < string.size()) {
                char c = string[pos++];
                if (c == '"') { posinfo.col++; found_closing_quote = true; break; }
                if (c == '\\') {
                    if (pos >= string.size()) error("Invalid escape");
                    char e = string[pos++];
                    posinfo.col++;
                    switch (e) {
                        case '"':   out.push_back('"');     break;
                        case '\\':  out.push_back('\\');    break;
                        case '/':   out.push_back('/');     break;
                        case 'b':   out.push_back('\b');    break;
                        case 'n':   out.push_back('\n');    break;
                        case 'r':   out.push_back('\r');    break;
                        case 't':   out.push_back('\t');    break;
                        case 'u': {
                            // basic \uXXXX -> UTF-8
                            if (pos + 4 > string.size()) error("Invalid \\u escape");
                            unsigned code = 0;
                            for (int k=0;k<4;++k) {
                                char ch = string[pos++];
                                posinfo.col++;
                                code <<= 4;
                                if (ch >= '0' && ch <= '9') code += static_cast<unsigned>(ch - '0');
                                else if (ch >= 'a' && ch <= 'f') code += static_cast<unsigned>(10 + ch - 'a');
                                else if (ch >= 'A' && ch <= 'F') code += static_cast<unsigned>(10 + ch - 'A');
                                else error("Invalid hex in \\u escape");
                            }
                            // convert to UTF-8
                            if (code <= 0x7F) out.push_back(static_cast<char>(code));
                            else if (code <= 0x7FF) {
                                out.push_back(static_cast<char>(0xC0 | ((code >> 6) & 0x1F)));
                                out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                            } else {
                                out.push_back(static_cast<char>(0xE0 | ((code >> 12) & 0x0F)));
                                out.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
                                out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                            }
                        } break;
                        default:
                            error(std::string("Invalid escape \\") + e);
                    }
                } else {
                    out.push_back(c);
                    if (c == '\n') { posinfo.line++; posinfo.col = 1; }
                    else posinfo.col++;
                }
            }
            if (!found_closing_quote) error("Unterminated string");
            return JsonValue(std::move(out));
        }

        JsonValue JsonDeserializer::parseObject() {
            expectChar('{');
            skipWhitespace();
            JsonObject obj;
            if (peek() == '}') { advance(); return JsonValue(std::move(obj)); }
            while (true) {
                skipWhitespace();
                if (peek() != '"') error("Object keys must be strings");
                JsonValue keyv = parseString();
                skipWhitespace();
                expectChar(':');
                skipWhitespace();
                JsonValue val = parseJsonValue();
                obj.emplace(keyv.asString(), std::move(val));
                skipWhitespace();
                if (peek() == '}') { advance(); break; }
                if (peek() == ',') { advance(); skipWhitespace(); continue; }
                error("Expected ',' or '}' in object");
            }
            return JsonValue(std::move(obj));
        }

        JsonValue JsonDeserializer::parseArray() {
            expectChar('[');
            skipWhitespace();
            JsonArray arr;
            if (peek() == ']') { advance(); return JsonValue(std::move(arr)); }
            while (true) {
                skipWhitespace();
                arr.push_back(parseJsonValue());
                skipWhitespace();
                if (peek() == ']') { advance(); break; }
                if (peek() == ',') { advance(); skipWhitespace(); continue; }
                error("Expected ',' or ']' in array");
            }
            return JsonValue(std::move(arr));
        }

        JsonValue JsonDeserializer::parseNull() {
            if (string.compare(pos,4,"null") == 0) { 
                pos += 4; 
                posinfo.col += 4; 
                return JsonValue(nullptr); 
            }
            error("Invalid token, expected 'null'");
            return JsonValue{};
        }

        JsonValue JsonDeserializer::parseBool() {
            if (string.compare(pos,4,"true") == 0) { 
                pos += 4; 
                posinfo.col += 4; 
                return JsonValue(true); 
            }
            if (string.compare(pos,5,"false") == 0) { 
                pos += 5; 
                posinfo.col += 5; 
                return JsonValue(false); 
            }
            error("Invalid token, expected 'true' or 'false'");
            return JsonValue{};
        }
    
        void JsonDeserializer::expectChar(char c) {
            if (peek() != c) error(std::string("Expected '") + c + "'");
            advance();
        }
    #else
        JsonValue JsonDeserializer::deserialize(const std::string& src, ParseError& err_out) {
            JsonDeserializer deser{src};
            return deser.deserializeInternal(err_out);
        }

        JsonValue JsonDeserializer::deserializeInternal(ParseError& err_out) {
            err_out = ParseError{};
            skipWhitespace();
            JsonValue v = parseJsonValue(err_out);
            if (!err_out.what.empty()) return JsonValue{};
            skipWhitespace();
            if (pos != string.size()) {
                err_out = ParseError("Trailing data after JSON", posinfo);
                return JsonValue{};
            }
            return v;
        }

        JsonValue JsonDeserializer::parseJsonValue(ParseError& err_out) {
            if (pos >= string.size()) { err_out = ParseError("Unexpected end of input", posinfo); return JsonValue{}; }
            char c = string[pos];
            if (c == 'n') return parseNull(err_out);
            if (c == 't' || c == 'f') return parseBool(err_out);
            if (c == '"') return parseString(err_out);
            if (c == '[') return parseArray(err_out);
            if (c == '{') return parseObject(err_out);
            if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber(err_out);
            err_out = ParseError(std::string("Unexpected character '") + c + "'", posinfo);
            return JsonValue{};
        }
    
        JsonValue JsonDeserializer::parseNumber(ParseError& err_out) { 
            size_t start = pos;
            if (string[pos] == '-') advance();
            while (pos < string.size() && std::isdigit(static_cast<unsigned char>(string[pos]))) advance();
            if (pos < string.size() && string[pos] == '.') {
                advance();
                while (pos < string.size() && std::isdigit(static_cast<unsigned char>(string[pos]))) advance();
            }
            if (pos < string.size() && (string[pos] == 'e' || string[pos] == 'E')) {
                advance();
                if (string[pos] == '+' || string[pos] == '-') advance();
                while (pos < string.size() && std::isdigit(static_cast<unsigned char>(string[pos]))) advance();
            }
            std::string tok = string.substr(start, pos - start);
            char *endptr = nullptr;
            double d = std::strtod(tok.c_str(), &endptr);
            if (endptr != tok.c_str() + tok.size()) {
                err_out = ParseError("Invalid number format", posinfo);
                return JsonValue{};
            }
            return JsonValue(d);
        }
        
        JsonValue JsonDeserializer::parseString(ParseError& err_out) {
            if (!expectChar('"', err_out)) return JsonValue{};
            std::string out;
            bool found_closing_quote = false;
            while (pos < string.size()) {
                char c = string[pos++];
                posinfo.col++;
                if (c == '"') { found_closing_quote = true; break; }
                if (c == '\\') {
                    if (pos >= string.size()) { err_out = ParseError("Invalid escape", posinfo); return JsonValue{}; }
                    char e = string[pos++];
                    posinfo.col++;
                    switch (e) {
                        case '"': out.push_back('"'); break;
                        case '\\': out.push_back('\\'); break;
                        case '/': out.push_back('/'); break;
                        case 'b': out.push_back('\b'); break;
                        case 'f': out.push_back('\f'); break;
                        case 'n': out.push_back('\n'); break;
                        case 'r': out.push_back('\r'); break;
                        case 't': out.push_back('\t'); break;
                        case 'u': {
                            if (pos + 4 > string.size()) { err_out = ParseError("Invalid \\u escape", posinfo); return JsonValue{}; }
                            unsigned code = 0;
                            for (int k=0;k<4;++k) {
                                char ch = string[pos++];
                                posinfo.col++;
                                code <<= 4;
                                if (ch >= '0' && ch <= '9') code += static_cast<unsigned>(ch - '0');
                                else if (ch >= 'a' && ch <= 'f') code += static_cast<unsigned>(10 + ch - 'a');
                                else if (ch >= 'A' && ch <= 'F') code += static_cast<unsigned>(10 + ch - 'A');
                                else { err_out = ParseError("Invalid hex in \\u escape", posinfo); return JsonValue{}; }
                            }
                            if (code <= 0x7F) out.push_back(static_cast<char>(code));
                            else if (code <= 0x7FF) {
                                out.push_back(static_cast<char>(0xC0 | ((code >> 6) & 0x1F)));
                                out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                            } else {
                                out.push_back(static_cast<char>(0xE0 | ((code >> 12) & 0x0F)));
                                out.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
                                out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                            }
                        } break;
                        default:
                            err_out = ParseError(std::string("Invalid escape \\") + e, posinfo); return JsonValue{};
                    }
                } else {
                    out.push_back(c);
                    if (c == '\n') { posinfo.line++; posinfo.col = 1; }
                }
            }
            if (!found_closing_quote) { err_out = ParseError("Unterminated string", posinfo); return JsonValue{}; }
            return JsonValue(std::move(out));
        }
        
        JsonValue JsonDeserializer::parseObject(ParseError& err_out) {
            if (!expectChar('{', err_out)) return JsonValue{};
            skipWhitespace();
            JsonObject obj;
            if (peek() == '}') { advance(); return JsonValue(std::move(obj)); }
            while (true) {
                skipWhitespace();
                if (peek() != '"') { err_out = ParseError("Object keys must be strings", posinfo); return JsonValue{}; }
                JsonValue keyv = parseString(err_out);
                if (!err_out.what.empty()) return JsonValue{};
                skipWhitespace();
                if (!expectChar(':', err_out)) return JsonValue{};
                skipWhitespace();
                JsonValue val = parseJsonValue(err_out);
                if (!err_out.what.empty()) return JsonValue{};
                obj.emplace(keyv.asString(), std::move(val));
                skipWhitespace();
                if (peek() == '}') { advance(); break; }
                if (peek() == ',') { advance(); skipWhitespace(); continue; }
                err_out = ParseError("Expected ',' or '}' in object", posinfo);
                return JsonValue{};
            }
            return JsonValue(std::move(obj));
        }
        
        JsonValue JsonDeserializer::parseArray(ParseError& err_out) {
            if (!expectChar('[', err_out)) return JsonValue{};
            skipWhitespace();
            JsonArray arr;
            if (peek() == ']') { advance(); return JsonValue(std::move(arr)); }
            while (true) {
                skipWhitespace();
                arr.push_back(parseJsonValue(err_out));
                if (!err_out.what.empty()) return JsonValue{};
                skipWhitespace();
                if (peek() == ']') { advance(); break; }
                if (peek() == ',') { advance(); skipWhitespace(); continue; }
                err_out = ParseError("Expected ',' or ']' in array", posinfo);
                return JsonValue{};
            }
            return JsonValue(std::move(arr));
        }
        
        JsonValue JsonDeserializer::parseNull(ParseError& err_out) {
            if (string.compare(pos,4,"null") == 0) { 
                pos += 4; 
                posinfo.col += 4; 
                return JsonValue(nullptr); 
            }
            err_out = ParseError("Invalid token, expected 'null'", posinfo);
            return JsonValue{};
        }
        
        JsonValue JsonDeserializer::parseBool(ParseError& err_out) {
            if (string.compare(pos,4,"true") == 0) { 
                pos += 4; 
                posinfo.col += 4; 
                return JsonValue(true); 
            }
            if (string.compare(pos,5,"false") == 0) { 
                pos += 5; 
                posinfo.col += 5; 
                return JsonValue(false); 
            }
            err_out = ParseError("Invalid token, expected 'true' or 'false'", posinfo);
            return JsonValue{};
        }
        
        bool JsonDeserializer::expectChar(char c, ParseError& err_out) {
            if (peek() != c) { err_out = ParseError(std::string("Expected '") + c + "'", posinfo); return false; }
            advance(); return true;
        }
    #endif

        void JsonDeserializer::advance() {
            if (pos >= string.size()) return;
            if (string[pos] == '\n') { posinfo.line++; posinfo.col = 1; }
            else posinfo.col++;
            pos++;
        }

        char JsonDeserializer::peek() const {
            if (pos >= string.size()) return '\0';
            return string[pos];
        }

        void JsonDeserializer::skipWhitespace() {
            while (
                pos < string.size() && std::isspace(
                    static_cast<unsigned char>(string[pos])
                )
            ) advance();
        }
}
