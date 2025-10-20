/*
 * HMS_JSON Library - Quick Start Guide
 * 
 * This file demonstrates the most common use cases in minimal code
 */

#include "HMS_JSON.h"
#include <iostream>

using namespace HMS;

int main() {
    
    // =============================================
    // 1. CREATING JSON (Building from scratch)
    // =============================================
    
    JsonValue user;
    user["name"] = "John Doe";
    user["age"] = 30;
    user["active"] = true;
    user["email"] = JsonValue(nullptr);  // null value
    
    // Add array
    user["hobbies"][0] = "reading";
    user["hobbies"][1] = "coding";
    user["hobbies"][2] = "gaming";
    
    // Add nested object
    user["address"]["street"] = "123 Main St";
    user["address"]["city"] = "Anytown";
    user["address"]["zipcode"] = 12345;
    
    std::cout << "Created JSON:\n" << serialize(user, true) << "\n\n";
    
    
    // =============================================
    // 2. PARSING JSON (String to Object)
    // =============================================
    
    std::string jsonText = R"({
        "product": "Arduino Uno",
        "price": 25.99,
        "in_stock": true,
        "specs": {
            "cpu": "ATmega328P",
            "voltage": "5V",
            "pins": [2, 3, 4, 5, 6, 7, 8, 9]
        }
    })";
    
    #if HMS_JSON_EXCEPTIONS_ENABLED
        JsonValue product = deserialize(jsonText);
    #else
        ParseError error;
        JsonValue product = parse(jsonText, error);
        if (error) {
            std::cout << "Parse error: " << error.what << std::endl;
            return 1;
        }
    #endif
    
    
    // =============================================
    // 3. ACCESSING DATA (Type-safe reading)
    // =============================================
    
    // Check types before accessing
    if (product["product"].isString()) {
        std::cout << "Product: " << product["product"].asString() << std::endl;
    }
    
    if (product["price"].isNumber()) {
        std::cout << "Price: $" << product["price"].asNumber() << std::endl;
    }
    
    if (product["in_stock"].isBool()) {
        std::cout << "In Stock: " << (product["in_stock"].asBool() ? "Yes" : "No") << std::endl;
    }
    
    // Access nested object
    if (product["specs"]["cpu"].isString()) {
        std::cout << "CPU: " << product["specs"]["cpu"].asString() << std::endl;
    }
    
    // Access array
    if (product["specs"]["pins"].isArray()) {
        const auto& pins = product["specs"]["pins"].asArray();
        std::cout << "Digital Pins: ";
        for (size_t i = 0; i < pins.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << (int)pins[i].asNumber();
        }
        std::cout << std::endl;
    }
    
    
    // =============================================
    // 4. MODIFYING JSON (Dynamic updates)
    // =============================================
    
    // Update existing values
    product["price"] = 23.99;  // Price change
    product["in_stock"] = false;
    
    // Add new fields
    product["manufacturer"] = "Arduino";
    product["year"] = 2021;
    
    // Add to existing array
    auto& pins = product["specs"]["pins"].getArray();
    pins.push_back(JsonValue(10));
    pins.push_back(JsonValue(11));
    
    std::cout << "\nUpdated JSON:\n" << serialize(product, true) << std::endl;
    
    
    // =============================================
    // 5. ERROR HANDLING
    // =============================================
    
    std::string badJson = "{\"key\": invalid}";
    
    #if HMS_JSON_EXCEPTIONS_ENABLED
        try {
            JsonValue result = deserialize(badJson);
        } catch (const ParseError& e) {
            std::cout << "\nParse Error: " << e.what() 
                     << " at line " << e.pos.line 
                     << ", column " << e.pos.col << std::endl;
        }
    #else
        ParseError err;
        JsonValue result = parse(badJson, err);
        if (err) {
            std::cout << "\nParse Error: " << err.what 
                     << " at line " << err.pos.line 
                     << ", column " << err.pos.col << std::endl;
        }
    #endif
    
    
    // =============================================
    // 6. COMMON PATTERNS
    // =============================================
    
    // Build API response
    JsonValue response;
    response["status"] = "success";
    response["code"] = 200;
    response["data"]["users"][0]["id"] = 1;
    response["data"]["users"][0]["name"] = "Alice";
    response["data"]["users"][1]["id"] = 2;
    response["data"]["users"][1]["name"] = "Bob";
    response["timestamp"] = 1634567890;
    
    // Serialize for network transmission
    std::string apiResponse = serialize(response);  // Compact
    std::cout << "\nAPI Response: " << apiResponse << std::endl;
    
    return 0;
}