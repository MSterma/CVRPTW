//
// Created by Mateusz Sterma on 09/11/2024.
//

#ifndef VEHICLE_H
#define VEHICLE_H
#include <string>
#include <vector>


class Vehicle {
    int x;
    int y;
    int maxCapacity;
    int capacity;
    double time;
    std::vector<int> route;

    public:
    Vehicle(int maxCapacity) {
        this->x = 0;
        this->y = 0;
        this->capacity = maxCapacity;
        this->maxCapacity = maxCapacity;
        this->time = 0;
    }
    int getX() {
        return x;
    }
    int getY() {
        return y;
    }
    int  setCapacity(int capacity) {
        this->capacity = capacity;
    }
    int setX(int x) {
        this->x = x;
    }
    int setY(int y) {
        this->y = y;
    }
    void add(int id) {
        this->route.push_back(id);
    }
    double getTime() {
        return this->time;
    }
    void incTime(double time) {
        this->time += time;
    }
    void pop() {
        this->route.pop_back();
    }
    int getMaxCapacity() {return maxCapacity;}
    int getCapacity() {return capacity;}
    void load(int cap) {
        this->capacity -= cap;
    }
    std::vector<int> getRoute() {
        return route;
    }
    std::string toString() {
        return std::to_string(x) + "," + std::to_string(y)+ "," + std::to_string(capacity)+", "+std::to_string(maxCapacity);
    }

};



#endif //VEHICLE_H
