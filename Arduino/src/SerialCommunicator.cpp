#include "SerialCommunicator.h"

SerialCommunicator::SerialCommunicator(){
    active = true;
}

void SerialCommunicator::bindTemp(TemperatureTask* tTask){
    temperatureTask = tTask;
}

void SerialCommunicator::bindWaste(WasteTask* wTask){
    wasteTask = wTask;
}

void SerialCommunicator::sendTemperature(float celsius){
    Serial.print("T:");
    Serial.print(celsius);
    Serial.println(';');
}

//percentage is a value from 0 to 1
void SerialCommunicator::sendFillPercentage(float percentage){
    Serial.print("F:");
    Serial.print(percentage);
    Serial.println(';');
}

void SerialCommunicator::execute(){
    //By design the events are either "E;" or "T;", so we read until we find a ';'
    //and then we check the character before to get the event type
    //in this way we filter possible garbage that might be received

    //If this logic is too heavy, we can switch it to read one byte at a time and manually ignore the terminator character
    if(Serial.available() > 0){
        uint8_t buffer[SERIAL_RX_BUFFER_SIZE];
        size_t len = Serial.readBytesUntil(';',buffer, SERIAL_RX_BUFFER_SIZE);
        if(len > 0){
            char c = buffer[len-1];
            //Not 100% sure that Serial.read returns the ascii value of the byte read
            switch(c){
                case 'E':
                    if(wasteTask != nullptr)
                        wasteTask->onEmptyEvent();
                    break;
                case 'T':
                    if(temperatureTask != nullptr)
                        temperatureTask->onFixTemperatureEvent();
                    break;
                default:
            }
        }
    }
}