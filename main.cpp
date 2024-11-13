#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "Client.h"
#include "Vehicle.h"
#define  RCLSIZE 3
#define TIME 60
double grasp(std::vector<Vehicle> &vehicles,std::vector<Client> clients, int maxCap,int depoDue) {
    double solLen=0;
    bool canLoad;
    std::vector<int> rcl;
    std::vector<Client> unserved=clients;
    int k=0;
    while(unserved.size()>0) {
        vehicles.push_back(maxCap);
        canLoad=true;
        while(canLoad) {
            int x=vehicles.at(k).getX();
            int y=vehicles.at(k).getY();
            int t=vehicles.at(k).getTime();
            std::sort(unserved.begin(),unserved.end(),[x,y,t](Client& lhs, Client& rhs) {
                double l= lhs.getDistance(x,y)+t>lhs.getReadyTime()?lhs.getDistance(x,y):lhs.getReadyTime()-(double)t;
               double r=rhs.getDistance(x,y)+t>rhs.getReadyTime()?rhs.getDistance(x,y):rhs.getReadyTime()-(double)t;
                //double l= lhs.getDistance(x,y);
               // double r= rhs.getDistance(x,y);
                return l<r;
            });
            int i=0;
            canLoad=false;
            int cap=vehicles.at(k).getCapacity();
            while (rcl.size()<RCLSIZE && i<unserved.size()) {
                double dis=unserved.at(i).getDistance(x,y)+t>=unserved.at(i).getReadyTime()?unserved.at(i).getDistance(x,y):(double)unserved.at(i).getReadyTime()-t;
                double ndsist=unserved.at(i).getDistance(0,0)+vehicles.at(k).getTime()+unserved.at(i).getServiceTime()+dis;
                if (cap>=unserved.at(i).getDemand() && unserved.at(i).getDistance(x,y)+t<unserved.at(i).getDueDate() && ndsist<=(double)depoDue) {
                    rcl.push_back(i);
                    canLoad=true;
                }
                i++;
            }
            if(rcl.size()>0) {
                int randNum = rand()%(rcl.size());
                Client cl=unserved.at(rcl.at(randNum));
                double d=cl.getDistance(x,y)+t>(double)cl.getReadyTime()?cl.getDistance(x,y):(double)cl.getReadyTime()-(double)vehicles.at(k).getTime();
                vehicles.at(k).add(cl.getId());
                vehicles.at(k).setX(cl.getX());
                vehicles.at(k).setY(cl.getY());
                vehicles.at(k).load(cl.getDemand());
                vehicles.at(k).incTime(d+(double)cl.getServiceTime());
                unserved.erase(unserved.begin()+rcl.at(randNum));
                rcl.clear();
            }else if (vehicles.at(k).getX()==0 && vehicles.at(k).getY()==0) {
                return -1;
            }else{

                vehicles.at(k).incTime(sqrt(pow( vehicles.at(k).getX(), 2) + pow(vehicles.at(k).getY(), 2)));
                vehicles.at(k).setX(0);
                vehicles.at(k).setY(0);
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
int main()
{
    double max=11111111;
    std::string testFile="input/m2kvrptw-0.txt";
    time_t beg=time(NULL);
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
        int depoDue=0;
        while (file>>vals[0]>>vals[1]>>vals[2]>>vals[3]>>vals[4]>>vals[5]>>vals[6]) {
            client_num++;
            if(vals[0]==0) {
                depoDue=vals[5];
                continue;
            }
            clients.emplace_back(vals[0]+1,vals[1],vals[2],vals[3],vals[4],vals[5],vals[6]);
        }// end of parsing
    std::string solution="";
    file.close();
    while(time(NULL)-beg<TIME) {
            double loc=grasp(vehicles,clients,maxCap,depoDue);
            if(loc<max) {
                solution="";
                max=loc;
                solution+=std::to_string(vehicles.size());
                solution+=" "+std::to_string(loc);
                solution+="\n";
                for (auto veh: vehicles) {
                    solution+=veh.toString();
                }
            }
        vehicles.clear();
    }
    std::ofstream Ofile ("solution.txt");
    Ofile << solution;
    Ofile.close();
    return 0;
}
