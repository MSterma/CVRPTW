#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include "Client.h"
#include "Vehicle.h"
#define  RCLSIZE 3
#define TIME 300
double grasp(std::vector<Vehicle> &vehicles,std::vector<Client> clients, int maxCap,Client depot) {
    double solLen=0;
    bool canLoad;
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
            std::sort(unserved.begin(),unserved.end(),[x,y,t](Client& lhs, Client& rhs) {
                double l= lhs.getDistance(x,y)+t>lhs.getReadyTime()?lhs.getDistance(x,y):lhs.getReadyTime()-static_cast<double>(t);
               double r=rhs.getDistance(x,y)+t>rhs.getReadyTime()?rhs.getDistance(x,y):rhs.getReadyTime()-static_cast<double>(t);
                //double l= lhs.getDistance(x,y);
               // double r= rhs.getDistance(x,y);
                return l<r;
            });
            int i=0;
            int cap=vehicles.at(k).getCapacity();
            while (rcl.size()<RCLSIZE && i<unserved.size()) {
                double dis=unserved.at(i).getDistance(x,y)+t>=unserved.at(i).getReadyTime()?unserved.at(i).getDistance(x,y):static_cast<double>(unserved.at(i).getReadyTime())-t;
                double ndsist=unserved.at(i).getDistance(xd,yd)+vehicles.at(k).getTime()+unserved.at(i).getServiceTime()+dis;
                if (cap>=unserved.at(i).getDemand() && unserved.at(i).getDistance(x,y)+t<unserved.at(i).getDueDate() && ndsist<=static_cast<double>(depoDue)) {
                    rcl.push_back(i);
                }
                i++;
            }
            if(rcl.size()>0) {
                int randNum = rand()%(rcl.size());
                Client cl=unserved.at(rcl.at(randNum));
                double d=cl.getDistance(vehicles.at(k).getX(),vehicles.at(k).getY())+t>=static_cast<double>(cl.getReadyTime())?cl.getDistance(x,y):(static_cast<double>(cl.getReadyTime())-vehicles.at(k).getTime());
                vehicles.at(k).add(cl.getId());
                vehicles.at(k).setX(cl.getX());
                vehicles.at(k).setY(cl.getY());
                vehicles.at(k).load(cl.getDemand());
                vehicles.at(k).incTime(d+static_cast<double>(cl.getServiceTime()));
                unserved.erase(unserved.begin()+rcl.at(randNum));
                rcl.clear();
            }else if (vehicles.at(k).getX()==xd && vehicles.at(k).getY()==yd) {
                return -1;
            }else{
                vehicles.at(k).incTime(depot.getDistance(vehicles.at(k).getX(),vehicles.at(k).getY()));
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
int main(int argc, char *argv[])
{
    if(argc!=3) {
        argv[1]=(char*)"input/rc21010.txt";
        argv[2]=(char*)"solution.txt";


    }

    double max=11111111;
    std::string testFile=argv[1];
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
        file>>vals[0]>>vals[1]>>vals[2]>>vals[3]>>vals[4]>>vals[5]>>vals[6];
         Client depot= Client(vals[0],vals[1],vals[2],vals[3],vals[4],vals[5],vals[6]);
        while (file>>vals[0]>>vals[1]>>vals[2]>>vals[3]>>vals[4]>>vals[5]>>vals[6]) {
            client_num++;
            clients.emplace_back(vals[0],vals[1],vals[2],vals[3],vals[4],vals[5],vals[6]);
        }// end of parsing
    std::string solution="";
    file.close();
    while(time(NULL)-beg<TIME) {
            double loc=grasp(vehicles,clients,maxCap,depot);
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
    std::ofstream Ofile (argv[2]);
    Ofile << solution;
    Ofile.close();
    return 0;
}
