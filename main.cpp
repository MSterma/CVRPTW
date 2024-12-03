#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>

#include "Client.h"
#include "Vehicle.h"
#define  RCLSIZE 2
#define TIME 1
double grasp(std::vector<Vehicle> &vehicles,std::vector<Client> clients, int maxCap,Client depot) {
    double solLen=0;
    srand(time(NULL));
    std::vector<int> rcl;
    std::vector<Client> unserved=std::move(clients);
    int xd=depot.getX();
    int yd=depot.getY();
    int depoDue=depot.getDueDate();
    int k=0;
    while(!unserved.empty()) {
        vehicles.emplace_back(Vehicle(xd,yd,maxCap));
        while(1) {
            int x=vehicles.at(k).getX();
            int y=vehicles.at(k).getY();
            double t=vehicles.at(k).getTime();
            Vehicle& vehicle=vehicles.at(k);
            int cap=vehicle.getCapacity();

            std::sort(unserved.begin(),unserved.end(),[x,y,t](Client& lhs, Client& rhs) {
                double l= lhs.getDistance(x,y)+t>lhs.getReadyTime()?lhs.getDistance(x,y):lhs.getReadyTime()-static_cast<double>(t);
               double r=rhs.getDistance(x,y)+t>rhs.getReadyTime()?rhs.getDistance(x,y):rhs.getReadyTime()-static_cast<double>(t);
                return l<r;
            });
            int i=0;

            while (rcl.size()<RCLSIZE && i<unserved.size()) {
                double dis=unserved.at(i).getDistance(x,y)+t>=unserved.at(i).getReadyTime()?unserved.at(i).getDistance(x,y):static_cast<double>(unserved.at(i).getReadyTime())-t;
                double ndsist=unserved.at(i).getDistance(xd,yd)+vehicles.at(k).getTime()+unserved.at(i).getServiceTime()+dis;
                if (cap>=unserved.at(i).getDemand() && dis+t<=unserved.at(i).getDueDate() && ndsist<=static_cast<double>(depoDue)) {
                    rcl.push_back(i);
                }
                i++;
            }
            if(rcl.size()>0) {
                int randNum = rand()%(rcl.size());
                Client cl=unserved.at(rcl.at(randNum));
                double d=cl.getDistance(x,y)+t>=static_cast<double>(cl.getReadyTime())?cl.getDistance(x,y):(static_cast<double>(cl.getReadyTime())-t);
               vehicle.add(cl.getId());
                vehicle.setX(cl.getX());
                vehicle.setY(cl.getY());
                vehicle.load(cl.getDemand());
                vehicle.incTime(d+static_cast<double>(cl.getServiceTime()));
                unserved.erase(unserved.begin()+rcl.at(randNum));
                rcl.clear();
            }else if (x==xd && y==yd && vehicle.getRoute().empty()) {
                return -1;
            }else{
                vehicle.incTime(depot.getDistance(vehicles.at(k).getX(),vehicles.at(k).getY()));
                break;
            }
        }
        k++;
    }
    for (auto vehicle: vehicles) {
        solLen+=vehicle.getTime();
    }
    return solLen;
}
double genetic(std::vector<std::vector<int>> &population,std::vector<Client> clients, int maxCap,Client depot) {
    std::vector<Vehicle> vehicles;
    srand(time(NULL));
    std::vector<std::vector<int>> crossovers;
    std::map<int,double> solVals;
    int i=0;
    for(auto v : population) {
        int x=depot.getX();
        int y=depot.getY();
        int cap=0;
        double maxt=0;
        double t=0;
        for(int vi: v) {
            Client cl=clients.at(vi-1);

            if(vi==-1) {
                cl=depot;
            }
            double dist=cl.getDistance(x,y);
            double d=dist+t>=cl.getReadyTime()?cl.getDistance(x,y):(static_cast<double>(cl.getReadyTime())-t);
            if(cap+cl.getDemand()>maxCap || dist+t>cl.getDueDate() || d+depot.getDistance(x,y)>depot.getDueDate()) {
                maxt+=t+depot.getDistance(x,y);
                x=depot.getX();
                y=depot.getY();
                cap=0;

                t=0;
                dist=cl.getDistance(x,y);
                d=dist>=cl.getReadyTime()?cl.getDistance(x,y):(static_cast<double>(cl.getReadyTime()));
            }
            cap+=cl.getDemand();
            x=cl.getX();
            y=cl.getY();
            t+=d+cl.getServiceTime();
        }
        maxt+=t+depot.getDistance(x,y);
        solVals.insert({i,maxt});
        i++;
    }
    for (auto v: solVals) {
        std::cout<<v.first<<" "<<v.second<<std::endl;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc!=3) {
        argv[1]=(char*)"input/rc21010.txt";
        argv[2]=(char*)"solution.txt";


    }


    std::string testFile=argv[1];
    std::ifstream file (testFile);
        std::vector<Vehicle> vehicles;
        std::vector<Client> clients;
        int vehicles_num;

        int maxCap;
        int client_num = 0;
        std::string fileName;
        std::string buf;
        //file parsing:
        file >> fileName;
        for (int i = 0; i < 3; ++i) {
            file >> buf;
        }
        file >> vehicles_num;
        file >> maxCap;
        for(int i = 0; i < 4; i++) {
            getline(file,buf);
        }
        int vals[7];
        file>>vals[0]>>vals[1]>>vals[2]>>vals[3]>>vals[4]>>vals[5]>>vals[6];
         Client depot= Client(vals[0],vals[1],vals[2],vals[3],vals[4],vals[5],vals[6]);
         std::ofstream Ofile (argv[2]);
        while (file>>vals[0]>>vals[1]>>vals[2]>>vals[3]>>vals[4]>>vals[5]>>vals[6]) {
            client_num++;
            clients.emplace_back(vals[0],vals[1],vals[2],vals[3],vals[4],vals[5],vals[6]);
        }// end of parsing
    double max=11111111;
    int z=0;
    int min=0;
    std::vector<std::vector<int>> population;
    while(z<10) {
        std::vector<int> sl;
        double l=grasp(vehicles,clients,maxCap,depot);
        for(auto vehicle: vehicles) {
            for (int route: vehicle.getRoute()) {
                sl.emplace_back(route);
            }
        }
        population.emplace_back(sl);
        vehicles.clear();
        z++;
    }
    genetic(population,clients,maxCap,depot);
    file.close();
    Ofile.close();

    return 0;
}
