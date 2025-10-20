/*
 * HMS_JSON Library - Comprehensive Feature Showcase
 * 
 * This example demonstrates all major features of the HMS_JSON library:
 * - Creating JSON objects and arrays
 * - All data types (null, bool, number, string, object, array)
 * - Serialization with pretty printing
 * - Deserialization/parsing
 * - Error handling (both exception and error code modes)
 * - Type checking and safe value access
 * - Dynamic object/array manipulation
 */

#include "HMS_JSON.h"
#include <iostream>

using namespace HMS;

void showcase_creation_and_serialization() {
    std::cout << "=== JSON Creation & Serialization ===\n";
    
    // Create a complex JSON structure
    JsonValue root;
    
    // Basic data types
    root["name"] = "HMS_JSON Library";
    root["version"] = 1.0;
    root["active"] = true;
    root["deprecated"] = JsonValue(nullptr);  // null value
    
    // Nested object
    root["author"]["name"] = "Hamas Saeed";
    root["author"]["email"] = "hamasaeed@gmail.com";
    root["author"]["location"] = "Pakistan";
    
    // Array with mixed types
    root["features"][0] = "Cross-platform";
    root["features"][1] = "Modern C++";
    root["features"][2] = "Exception handling";
    root["features"][3] = "UTF-8 support";
    
    // Nested array of objects
    root["platforms"][0]["name"] = "Arduino";
    root["platforms"][0]["supported"] = true;
    root["platforms"][1]["name"] = "ESP-IDF";
    root["platforms"][1]["supported"] = true;
    root["platforms"][2]["name"] = "STM32";
    root["platforms"][2]["supported"] = true;
    
    // Numbers and special characters
    root["stats"]["downloads"] = 1234.56;
    root["stats"]["users"] = 789;
    root["description"] = "A library with \"quotes\" and \n newlines \t tabs";
    
    // Serialize to compact JSON
    std::string compact = serialize(root);
    std::cout << "Compact JSON:\n" << compact << "\n\n";
    
    // Serialize to pretty JSON
    std::string pretty = serialize(root, true, 2);
    std::cout << "Pretty JSON:\n" << pretty << "\n\n";
}

void showcase_parsing_and_access() {
    std::cout << "=== JSON Parsing & Data Access ===\n";
    
    std::string jsonText = R"({
        "product": "Sensor Module",
        "id": 12345,
        "price": 29.99,
        "available": true,
        "tags": ["electronics", "sensor", "arduino"],
        "specifications": {
            "voltage": "3.3V-5V",
            "temperature_range": [-40, 85],
            "dimensions": {
                "width": 25.4,
                "height": 15.2,
                "unit": "mm"
            }
        },
        "warranty": null
    })";
    
    #if HMS_JSON_EXCEPTIONS_ENABLED
        try {
            JsonValue product = deserialize(jsonText);
            
            // Type checking and safe access
            std::cout << "Product parsing successful!\n";
            
            // Access basic values
            if (product["product"].isString()) {
                std::cout << "Product: " << product["product"].asString() << "\n";
            }
            
            if (product["price"].isNumber()) {
                std::cout << "Price: $" << product["price"].asNumber() << "\n";
            }
            
            if (product["available"].isBool()) {
                std::cout << "Available: " << (product["available"].asBool() ? "Yes" : "No") << "\n";
            }
            
            if (product["warranty"].isNull()) {
                std::cout << "Warranty: Not specified\n";
            }
            
            // Access arrays
            if (product["tags"].isArray()) {
                std::cout << "Tags: ";
                const auto& tags = product["tags"].asArray();
                for (size_t i = 0; i < tags.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << tags[i].asString();
                }
                std::cout << "\n";
            }
            
            // Access nested objects
            if (product["specifications"]["dimensions"].isObject()) {
                const auto& dims = product["specifications"]["dimensions"].asObject();
                std::cout << "Dimensions: " 
                         << dims.at("width").asNumber() << "x" 
                         << dims.at("height").asNumber() << " " 
                         << dims.at("unit").asString() << "\n";
            }
            
            // Access array elements
            if (product["specifications"]["temperature_range"].isArray()) {
                const auto& tempRange = product["specifications"]["temperature_range"].asArray();
                std::cout << "Temperature Range: " 
                         << tempRange[0].asNumber() << "°C to " 
                         << tempRange[1].asNumber() << "°C\n";
            }
            
        } catch (const ParseError& e) {
            std::cout << "Parse error at line " << e.pos.line 
                     << ", column " << e.pos.col << ": " << e.what() << "\n";
        }
    #else
        ParseError error;
        JsonValue product = parse(jsonText, error);
        
        if (error) {
            std::cout << "Parse error at line " << error.pos.line 
                     << ", column " << error.pos.col << ": " << error.what << "\n";
        } else {
            std::cout << "Product parsing successful!\n";
            // ... same access code as above
        }
    #endif
    
    std::cout << "\n";
}

void showcase_dynamic_manipulation() {
    std::cout << "=== Dynamic JSON Manipulation ===\n";
    
    JsonValue config;
    
    // Build configuration dynamically
    config["server"]["host"] = "localhost";
    config["server"]["port"] = 8080;
    config["server"]["ssl"] = false;
    
    // Add array of allowed origins
    config["cors"]["origins"][0] = "http://localhost:3000";
    config["cors"]["origins"][1] = "https://example.com";
    config["cors"]["methods"][0] = "GET";
    config["cors"]["methods"][1] = "POST";
    config["cors"]["methods"][2] = "PUT";
    
    // Add database configurations
    config["databases"]["primary"]["type"] = "postgresql";
    config["databases"]["primary"]["connection_string"] = "postgresql://user:pass@localhost/db";
    config["databases"]["cache"]["type"] = "redis";
    config["databases"]["cache"]["host"] = "localhost";
    config["databases"]["cache"]["port"] = 6379;
    
    std::cout << "Initial configuration:\n" << serialize(config, true) << "\n";
    
    // Modify existing values
    config["server"]["port"] = 9000;
    config["server"]["ssl"] = true;
    
    // Add new array element
    auto& origins = config["cors"]["origins"].getArray();
    origins.push_back(JsonValue("https://api.example.com"));
    
    // Add new object
    config["logging"]["level"] = "info";
    config["logging"]["file"] = "/var/log/app.log";
    
    std::cout << "Modified configuration:\n" << serialize(config, true) << "\n";
}

void showcase_error_handling() {
    std::cout << "=== Error Handling ===\n";
    
    // Test various malformed JSON strings
    std::vector<std::string> malformedJson = {
        R"({"key": })",                    // Missing value
        R"({"key": "unterminated string)",  // Unterminated string
        R"({key: "value"})",               // Unquoted key
        R"({"key": "value",})",            // Trailing comma
        R"([1, 2, 3,])",                  // Trailing comma in array
        R"({"number": 123.45.67})",       // Invalid number
    };
    
    for (size_t i = 0; i < malformedJson.size(); ++i) {
        std::cout << "Testing malformed JSON #" << (i + 1) << ":\n";
        
        #if HMS_JSON_EXCEPTIONS_ENABLED
            try {
                JsonValue result = deserialize(malformedJson[i]);
                std::cout << "  Unexpectedly succeeded!\n";
            } catch (const ParseError& e) {
                std::cout << "  Error at line " << e.pos.line 
                         << ", col " << e.pos.col << ": " << e.what() << "\n";
            }
        #else
            ParseError error;
            JsonValue result = parse(malformedJson[i], error);
            if (error) {
                std::cout << "  Error at line " << error.pos.line 
                         << ", col " << error.pos.col << ": " << error.what << "\n";
            } else {
                std::cout << "  Unexpectedly succeeded!\n";
            }
        #endif
    }
    std::cout << "\n";
}

void showcase_unicode_support() {
    std::cout << "=== Unicode & Escape Sequences ===\n";
    
    JsonValue unicode;
    unicode["english"] = "Hello World";
    unicode["arabic"] = "مرحبا بالعالم";
    unicode["chinese"] = "你好世界";
    unicode["emoji"] = "🌍🚀✨";
    unicode["escaped"] = "Line 1\nLine 2\tTabbed";
    unicode["quotes"] = "He said \"Hello!\"";
    unicode["unicode_escape"] = "\u0048\u0065\u006C\u006C\u006F"; // "Hello"
    
    std::string serialized = serialize(unicode, true);
    std::cout << "Unicode JSON:\n" << serialized << "\n";
    
    // Parse it back
    #if HMS_JSON_EXCEPTIONS_ENABLED
        JsonValue parsed = deserialize(serialized);
        std::cout << "Parsed back successfully!\n";
        std::cout << "Arabic text: " << parsed["arabic"].asString() << "\n";
        std::cout << "Unicode escape: " << parsed["unicode_escape"].asString() << "\n\n";
    #else
        ParseError error;
        JsonValue parsed = parse(serialized, error);
        if (!error) {
            std::cout << "Parsed back successfully!\n";
            std::cout << "Arabic text: " << parsed["arabic"].asString() << "\n";
            std::cout << "Unicode escape: " << parsed["unicode_escape"].asString() << "\n\n";
        }
    #endif
}

int main() {
    std::cout << "HMS_JSON Library - Feature Showcase\n";
    std::cout << "====================================\n\n";
    
    #if HMS_JSON_EXCEPTIONS_ENABLED
        std::cout << "Running in EXCEPTION mode\n\n";
    #else
        std::cout << "Running in ERROR CODE mode\n\n";
    #endif
    
    showcase_creation_and_serialization();
    showcase_parsing_and_access();
    showcase_dynamic_manipulation();
    showcase_error_handling();
    showcase_unicode_support();
    
    std::cout << "All features demonstrated successfully!\n";
    return 0;
}