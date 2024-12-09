//
// Created by Mateusz Sterma on 09/12/2024.
//

#ifndef SOLUTION_H
#define SOLUTION_H
#include <utility>
#include <vector>


class Solution {
    double value;
    int routeNumber;
    std::vector<int> route;
    public:
    Solution(double value, int routeNumber, std::vector<int>  route) {
        this->value = value;
        this->routeNumber = routeNumber;
        this->route = std::move(route);
    }
    std::vector<int>  getRoute() {
        return route;
    }
    double getValue() {
        return value;
    }
    int getRouteNumber() {
        return routeNumber;
    }
    void setValue(double value) {
        this->value = value;
    }
    void setRouteNumber(int routeNumber) {
        this->routeNumber = routeNumber;
    }

};



#endif //SOLUTION_H
