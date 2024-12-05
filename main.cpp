#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <__charconv/chars_format.h>

#include "Client.h"
#include "Vehicle.h"
#define  RCLSIZE 2
#define TIME 180
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
double checkSolution(std::vector<Client> clients, int maxCap, Client depot, std::vector<int> v) {
    int x=depot.getX();
    int y=depot.getY();
    int cap=0;
    double maxt=0;
    double t=0;
    int maszruty=0;
    for(int vi: v) {
        Client cl=clients.at(vi-1);
        double dist=cl.getDistance(x,y);
        double d=dist+t>=cl.getReadyTime()?cl.getDistance(x,y):(static_cast<double>(cl.getReadyTime())-t);
        if(cap+cl.getDemand()>maxCap || dist+t>cl.getDueDate() || d+depot.getDistance(x,y)>depot.getDueDate()) {
            maxt+=t+depot.getDistance(x,y);
            maszruty++;
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
    maszruty++;
    return maxt*sqrt(maszruty);

}
double genetic(std::vector<std::vector<int>> &population,std::vector<Client> clients, int maxCap,Client depot) {
    std::vector<Vehicle> vehicles;
    srand(time(NULL));
    std::vector<std::vector<int>> crossovers;
    std::vector<std::pair<int,double>> solVals;
    int i=0;
    for(auto v : population) {
        solVals.push_back({i,checkSolution(clients,maxCap,depot,v)});
        i++;
    }
    std::sort(solVals.begin(), solVals.end(),[](std::pair<int,double> a,std::pair<int,double> b ){ return a.second<b.second;});
    /*for(auto v: solVals) {
        std::cout<<v.first<<" "<<std::fixed<<std::setprecision(5)<<v.second<<std::endl;
    }*/

    for(int z=1; z<population.size()*0.5; z++) {
        std::vector<int> first;
        std::vector<int> second;
        int j=rand()%population.at(0).size();
        //std::cout<<j<<std::endl;
        for(int k=0;k<j;k++) {
                first.push_back(population.at(z-1).at(k));
                second.push_back(population.at(z).at(k));
        }
        int circ=j;
        while(first.size()<clients.size()) {
            if(std::find(first.begin(), first.end(), population.at(z).at(circ)) != first.end()) {
                if(circ==clients.size()-1) {
                    circ=0;
                }else {
                    circ++;
                }
            }else {
                first.push_back(population.at(z).at(circ));
            }
        }
        circ=j;
        while(second.size()<clients.size()) {
            if(std::find(second.begin(), second.end(), population.at(z-1).at(circ)) != second.end()) {
                if(circ==clients.size()-1) {
                    circ=0;
                }else {
                    circ++;
                }
            }else {
                second.push_back(population.at(z-1).at(circ));
            }
        }
      if(rand()%1000==3) {
          int  firstL=rand()%clients.size();
          int  secondL=rand()%clients.size();
          while (firstL==secondL) {
              secondL=rand()%clients.size();
          }
          int buff=first.at(firstL);
          first.at(firstL)= first.at(secondL);
          first.at(secondL) = buff;
      }
        if(rand()%1000==3) {
            int  firstL=rand()%clients.size();
            int  secondL=rand()%clients.size();
            while (firstL==secondL) {
                secondL=rand()%clients.size();
            }
            int buff=second.at(firstL);
            second.at(firstL)= second.at(secondL);
            second.at(secondL) = buff;
        }
    }
    for (auto v: crossovers) {
        population.push_back(v);

    }
    while (population.size()>100) {
        int actsol=0;
        double max=checkSolution(clients,  maxCap, depot,population.at(0));
        int out=0;
        for(auto v : population) {
            double act=checkSolution(clients, maxCap, depot, v);
            if(act>max) {
                out=actsol;
                max=act;
            }
            actsol++;

        }
        population.erase(population.begin()+out);
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
    double max=4576016.77462*1048;

    int z=0;
    int min=0;
    std::vector<std::vector<int>> population;
    long beg =time(NULL);
    while(z<50) {
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
    std::cout<<"done"<<time(NULL)-beg<<std::endl;
    int iters=0;
    while(time(NULL)-beg<TIME) {
       // std::cout<<iters<<std::endl;
        for(auto v: population) {
            double sol=checkSolution(clients,maxCap,depot,v);
            if(sol<max) {
                max=sol;
                int x=depot.getX();
                int y=depot.getY();
                int cap=0;
                double maxt=0;
                double t=0;
                int maszruty=0;
                for(int vi: v) {
                    Client cl=clients.at(vi-1);
                    double dist=cl.getDistance(x,y);
                    double d=dist+t>=cl.getReadyTime()?cl.getDistance(x,y):(static_cast<double>(cl.getReadyTime())-t);
                    if(cap+cl.getDemand()>maxCap || dist+t>cl.getDueDate() || d+depot.getDistance(x,y)>depot.getDueDate()) {
                        maxt+=t+depot.getDistance(x,y);
                        maszruty++;
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
                maszruty++;
                std::cout<<std::fixed<<std::setprecision(5)<<maxt<<" "<<maszruty<<" "<<iters<<std::endl;
            }
        }
        genetic(population,clients,maxCap,depot);
        iters++;

    }

    file.close();
    Ofile.close();

    return 0;
}
