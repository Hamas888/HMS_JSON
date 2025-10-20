/*
 * HMS_JSON Library - Arduino Example
 * 
 * Simple Arduino sketch demonstrating HMS_JSON usage for IoT sensors
 * Perfect for ESP32, ESP8266, Arduino boards
 */

#include "HMS_JSON.h"

using namespace HMS;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("HMS_JSON Arduino Example");
    Serial.println("========================");
    
    // Example 1: Create sensor data JSON
    createSensorDataExample();
    
    // Example 2: Parse configuration JSON
    parseConfigExample();
    
    // Example 3: Handle sensor readings
    handleSensorReadingsExample();
}

void loop() {
    // In a real application, you might:
    // 1. Read sensor data
    // 2. Create JSON payload
    // 3. Send to server/MQTT
    // 4. Parse server responses
    
    delay(5000);
}

void createSensorDataExample() {
    Serial.println("\n--- Creating Sensor Data JSON ---");
    
    JsonValue sensorData;
    
    // Device info
    sensorData["device_id"] = "ESP32_001";
    sensorData["timestamp"] = 1634567890;
    sensorData["location"] = "Living Room";
    
    // Sensor readings
    sensorData["sensors"]["temperature"] = 23.5;
    sensorData["sensors"]["humidity"] = 45.2;
    sensorData["sensors"]["pressure"] = 1013.25;
    sensorData["sensors"]["air_quality"] = 150;
    
    // Status flags
    sensorData["status"]["wifi_connected"] = true;
    sensorData["status"]["battery_level"] = 87;
    sensorData["status"]["last_error"] = JsonValue(nullptr);
    
    // Convert to JSON string
    String jsonString = serialize(sensorData).c_str();
    Serial.println("Sensor Data JSON:");
    Serial.println(jsonString);
    
    // Pretty print version
    String prettyJson = serialize(sensorData, true, 2).c_str();
    Serial.println("\nPretty JSON:");
    Serial.println(prettyJson);
}

void parseConfigExample() {
    Serial.println("\n--- Parsing Configuration JSON ---");
    
    // Simulate receiving config from server
    String configJson = R"({
        "wifi": {
            "ssid": "MyWiFi",
            "password": "password123",
            "timeout": 30
        },
        "mqtt": {
            "server": "iot.example.com",
            "port": 1883,
            "topic": "sensors/esp32_001",
            "enabled": true
        },
        "sensors": {
            "read_interval": 10,
            "calibration": [1.0, 0.95, 1.02],
            "enabled_sensors": ["temp", "humidity", "pressure"]
        }
    })";
    
    #if HMS_JSON_EXCEPTIONS_ENABLED
        try {
            JsonValue config = deserialize(configJson.c_str());
            
            // Extract WiFi settings
            if (config["wifi"].isObject()) {
                String ssid = config["wifi"]["ssid"].asString().c_str();
                String password = config["wifi"]["password"].asString().c_str();
                int timeout = (int)config["wifi"]["timeout"].asNumber();
                
                Serial.println("WiFi Config:");
                Serial.println("  SSID: " + ssid);
                Serial.println("  Password: " + password);
                Serial.println("  Timeout: " + String(timeout) + "s");
            }
            
            // Extract MQTT settings
            if (config["mqtt"]["enabled"].asBool()) {
                String server = config["mqtt"]["server"].asString().c_str();
                int port = (int)config["mqtt"]["port"].asNumber();
                String topic = config["mqtt"]["topic"].asString().c_str();
                
                Serial.println("MQTT Config:");
                Serial.println("  Server: " + server);
                Serial.println("  Port: " + String(port));
                Serial.println("  Topic: " + topic);
            }
            
            // Extract sensor settings
            int interval = (int)config["sensors"]["read_interval"].asNumber();
            Serial.println("Sensor Config:");
            Serial.println("  Read Interval: " + String(interval) + "s");
            
            // Process calibration array
            if (config["sensors"]["calibration"].isArray()) {
                const auto& cal = config["sensors"]["calibration"].asArray();
                Serial.print("  Calibration: [");
                for (size_t i = 0; i < cal.size(); i++) {
                    if (i > 0) Serial.print(", ");
                    Serial.print(cal[i].asNumber());
                }
                Serial.println("]");
            }
            
        } catch (const ParseError& e) {
            Serial.print("Parse error: ");
            Serial.println(e.what());
        }
    #else
        ParseError error;
        JsonValue config = parse(configJson.c_str(), error);
        
        if (error) {
            Serial.print("Parse error: ");
            Serial.println(error.what.c_str());
        } else {
            Serial.println("Configuration parsed successfully!");
            // ... same processing code as above
        }
    #endif
}

void handleSensorReadingsExample() {
    Serial.println("\n--- Handling Multiple Sensor Readings ---");
    
    JsonValue readings;
    
    // Simulate reading from multiple sensors
    float temp = 24.3;
    float humidity = 52.1;
    float pressure = 1015.2;
    bool motionDetected = true;
    
    // Create readings array
    JsonValue tempReading;
    tempReading["sensor"] = "DHT22";
    tempReading["type"] = "temperature";
    tempReading["value"] = temp;
    tempReading["unit"] = "°C";
    tempReading["timestamp"] = millis();
    
    JsonValue humidityReading;
    humidityReading["sensor"] = "DHT22";
    humidityReading["type"] = "humidity";
    humidityReading["value"] = humidity;
    humidityReading["unit"] = "%";
    humidityReading["timestamp"] = millis();
    
    JsonValue pressureReading;
    pressureReading["sensor"] = "BMP280";
    pressureReading["type"] = "pressure";
    pressureReading["value"] = pressure;
    pressureReading["unit"] = "hPa";
    pressureReading["timestamp"] = millis();
    
    JsonValue motionReading;
    motionReading["sensor"] = "PIR";
    motionReading["type"] = "motion";
    motionReading["value"] = motionDetected;
    motionReading["unit"] = JsonValue(nullptr);
    motionReading["timestamp"] = millis();
    
    // Add to readings array
    readings["device"] = "ESP32_001";
    readings["batch_timestamp"] = millis();
    readings["readings"][0] = tempReading;
    readings["readings"][1] = humidityReading;
    readings["readings"][2] = pressureReading;
    readings["readings"][3] = motionReading;
    
    // Convert to compact JSON for transmission
    String payload = serialize(readings).c_str();
    Serial.println("Sensor Batch Payload:");
    Serial.println(payload);
    
    // In a real application, you would:
    // WiFiClient client;
    // client.print("POST /api/sensors HTTP/1.1\r\n");
    // client.print("Content-Type: application/json\r\n");
    // client.print("Content-Length: " + String(payload.length()) + "\r\n\r\n");
    // client.print(payload);
    
    Serial.println("\nPayload ready for transmission!");
}

// Utility function to demonstrate error handling
void demonstrateErrorHandling() {
    Serial.println("\n--- Error Handling Demo ---");
    
    String badJson = "{\"temp\": 25.5, \"humidity\":}"; // Missing value
    
    #if HMS_JSON_EXCEPTIONS_ENABLED
        try {
            JsonValue parsed = deserialize(badJson.c_str());
            Serial.println("Unexpectedly succeeded!");
        } catch (const ParseError& e) {
            Serial.print("Caught parse error at line ");
            Serial.print(e.pos.line);
            Serial.print(", column ");
            Serial.print(e.pos.col);
            Serial.print(": ");
            Serial.println(e.what());
        }
    #else
        ParseError error;
        JsonValue parsed = parse(badJson.c_str(), error);
        if (error) {
            Serial.print("Parse error: ");
            Serial.println(error.what.c_str());
        }
    #endif
}