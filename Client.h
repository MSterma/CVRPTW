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
public:
    Client(int id,int x, int y,int demand, int serviceTime, int dueDate,int readyTime) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->demand = demand;
        this->dueDate = dueDate;
        this->serviceTime = serviceTime;
        this->readyTime = readyTime;
    }
    int getX(){return x;};
    int getY(){return y;};
    int getDemand() {
        return demand;
    }
    int getReadyTime() {
        return readyTime;
    };
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
        return std::to_string(id)+", "+std::to_string(x) + ", " + std::to_string(y)+", "+std::to_string(demand)+", "+std::to_string(dueDate)+", "+std::to_string(serviceTime)+", "+std::to_string(readyTime);

    }
};



#endif //CLIENT_H
