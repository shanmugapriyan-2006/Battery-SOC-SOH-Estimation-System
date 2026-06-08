#include <Arduino.h>

// -------------------- Pins --------------------
const int voltageSensorPin = 33;
const int currentSensorPin = 34;

// -------------------- ADC Parameters --------------------
const float ADC_RESOLUTION = 4095.0;
const float REFERENCE_VOLTAGE = 3.3;

// -------------------- Voltage Sensor --------------------
// Adjust according to your voltage divider
const float MAX_INPUT_VOLTAGE = 16.5;

// -------------------- Current Sensor --------------------
const float CURRENT_SENSOR_SENSITIVITY = 38.938; // mV/A
float currentSensorOffset = 0.0;

// --------------------------------------------------------
// Calibrate Current Sensor
// Ensure NO current flows during startup
// --------------------------------------------------------
float calibrateCurrentSensor()
{
    const int samples = 500;
    long totalADC = 0;

    for (int i = 0; i < samples; i++)
    {
        totalADC += analogRead(currentSensorPin);
        delay(2);
    }

    float averageADC = (float)totalADC / samples;

    return (averageADC / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
}

// --------------------------------------------------------
// Read Voltage
// --------------------------------------------------------
float readVoltage()
{
    const int samples = 50;
    long totalADC = 0;

    for (int i = 0; i < samples; i++)
    {
        totalADC += analogRead(voltageSensorPin);
        delay(2);
    }

    float averageADC = (float)totalADC / samples;

    return (averageADC / ADC_RESOLUTION) * MAX_INPUT_VOLTAGE;
}

// --------------------------------------------------------
// Read Current
// --------------------------------------------------------
float readCurrent()
{
    const int samples = 200;
    long totalADC = 0;

    for (int i = 0; i < samples; i++)
    {
        totalADC += analogRead(currentSensorPin);
        delay(2);
    }

    float averageADC = (float)totalADC / samples;

    float sensorVoltage =
        (averageADC / ADC_RESOLUTION) * REFERENCE_VOLTAGE;

    float current =
        (sensorVoltage - currentSensorOffset) /
        (CURRENT_SENSOR_SENSITIVITY / 1000.0);

    // Remove small noise around zero
    if (abs(current) < 0.05)
        current = 0.0;

    return current;
}

// -------------------- Setup --------------------
void setup()
{
    Serial.begin(115200);

    pinMode(voltageSensorPin, INPUT);
    pinMode(currentSensorPin, INPUT);

    // Better ADC range utilization
    analogSetPinAttenuation(voltageSensorPin, ADC_11db);
    analogSetPinAttenuation(currentSensorPin, ADC_11db);

    Serial.println("\nCalibrating Current Sensor...");
    Serial.println("Make sure NO current is flowing.");

    currentSensorOffset = calibrateCurrentSensor();

    Serial.print("Current Sensor Offset = ");
    Serial.print(currentSensorOffset, 4);
    Serial.println(" V");

    Serial.println("System Ready\n");
}

// -------------------- Loop --------------------
void loop()
{
    float voltage = readVoltage();
    float current = readCurrent();

    if (voltage < 0.5)
        voltage = 0.0;

    Serial.print("Voltage : ");
    Serial.print(voltage, 2);
    Serial.print(" V");

    Serial.print("    Current : ");
    Serial.print(current, 2);
    Serial.println(" A");

    delay(1000);
}