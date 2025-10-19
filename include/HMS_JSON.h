/*
 ============================================================================================================================================
 * File:        HMS_JSON.h
 * Author:      Hamas Saeed
 * Version:     Rev_1.0.3
 * Date:        Sep 20 2025
 * Brief:       This Package provides JSON parsing and serialization for Desktop & embedded systems (Arduino, ESP-IDF, Zephyr, STM32 HAL).
 ============================================================================================================================================
 * License: MIT License
 * 
 * Copyright (c) 2025 Hamas Saeed
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do 
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For any inquiries, contact Hamas Saeed at hamasaeed@gmail.com
 ============================================================================================================================================
 */

#ifndef HMS_JSON_H
#define HMS_JSON_H

#include "HMS_JSON_Value.h"
#include "HMS_JSON_Serializer.h"
#include "HMS_JSON_Exceptions.h"
#include "HMS_JSON_Deserializer.h"

namespace HMS {
    #if HMS_JSON_EXCEPTIONS_ENABLED
        inline JsonValue deserialize(const std::string& s) { return JsonDeserializer::deserialize(s); }
    #else
        inline JsonValue parse(const std::string& s, ParseError& err) { return JsonDeserializer::deserialize(s, err); }
    #endif

    inline std::string serialize(const JsonValue& v, bool pretty=false, int indent=2) {
        return JsonSerializer::toString(v, pretty, indent);
    }
}

#endif // HMS_JSON_H