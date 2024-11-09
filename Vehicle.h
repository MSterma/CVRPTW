//
// Created by Mateusz Sterma on 09/11/2024.
//

#ifndef VEHICLE_H
#define VEHICLE_H
#include <string>


class Vehicle {
    int x;
    int y;
    int maxCapacity;
    int capacity;
    public:
    Vehicle(int maxCapacity) {
        this->x = 0;
        this->y = 0;
        this->capacity = maxCapacity;
        this->maxCapacity = maxCapacity;
    }
    int getX() {
        return x;
    }
    int getY() {
        return y;
    }
    int getMaxCapacity() {return maxCapacity;}
    int getCapacity() {return capacity;}
    std::string toString() {
        return std::to_string(x) + "," + std::to_string(y)+ "," + std::to_string(capacity)+", "+std::to_string(maxCapacity);
    }

};



#endif //VEHICLE_H
