//
// Created by Mateusz Sterma on 09/11/2024.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <cmath>
#include <string>

/*    fields: x,y,readyTime, dueDate, serviceTime
      Methods: getDistance
   */

class Client {
private:
    int id;
    int x;
    int y;
    int demand;
    int readyTime;
    int dueDate;
    int serviceTime;
    bool served;
public:
    Client(int id,int x, int y,int demand, int readyTime,int dueDate, int serviceTime) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->demand = demand;
        this->dueDate = dueDate;
        this->serviceTime = serviceTime;
        this->readyTime = readyTime;
        this->served = false;
    }
    int getId() {return id;}
    int getX(){return x;};
    int getY(){return y;};
    int getDemand() {
        return demand;
    }
    int getReadyTime() {
        return readyTime;
    };
    bool getServed() {
        return served;
    }
    void setServed(bool served) {
        this->served = served;
    }

    int getDueDate() {
        return dueDate;
    };
    int getServiceTime() {
        return serviceTime;
    };
    double getDistance(int xv,int yv) {
        return sqrt(pow(xv - this->x, 2) + pow(yv - this->y, 2));
    }
    bool operator<( const Client& c2) const { return dueDate < c2.dueDate; }

    std::string toString() {
        return std::to_string(id)+", "+std::to_string(x) + ", " + std::to_string(y)+", "+std::to_string(demand)+", "+std::to_string(readyTime)+", "+std::to_string(dueDate)+", "+std::to_string(serviceTime);

    }
};



#endif //CLIENT_H
