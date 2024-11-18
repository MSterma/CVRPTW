//
// Created by Mateusz Sterma on 09/11/2024.
//

#ifndef VEHICLE_H
#define VEHICLE_H
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
/*
 *1048 4576016.774624*/

class Vehicle {
    int x;
    int y;
    int maxCapacity;
    int capacity;
    double time;
    std::vector<int> route;

    public:
    Vehicle(const int x, const int y,int maxCapacity) {
        this->x = x;
        this->y = y;
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
    void setX(int x) {
        this->x = x;
    }
    void setY(int y) {
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
        std::string r="";
        for (auto r1: route) {
            r += std::to_string(r1);
            r+=" ";
        }
        r+="\n";
        return r;
    }

};



#endif //VEHICLE_H
