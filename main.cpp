#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <random>
#include <__charconv/chars_format.h>
#include <__random/random_device.h>

#include "Client.h"
#include "Vehicle.h"
#define  RCLSIZE 1
#define TIME 300
#define PSIZE 20
#define WINRATE 80
double grasp(std::vector<Vehicle> &vehicles,std::vector<Client> clients, int maxCap,Client depot) {
    double solLen=0;
    srand(time(NULL));
    std::vector<int> rcl;
    std::vector<Client> unserved=std::move(clients);
    int xd=depot.getX();
    int yd=depot.getY();
    int depoDue=depot.getDueDate();
    int k=0;
    std::sort(unserved.begin(),unserved.end(),[](Client& lhs, Client& rhs) {
             //double l= lhs.getDistance(x,y)+t>lhs.getReadyTime()?lhs.getDistance(x,y):lhs.getReadyTime()-static_cast<double>(t);
           // double r=rhs.getDistance(x,y)+t>rhs.getReadyTime()?rhs.getDistance(x,y):rhs.getReadyTime()-static_cast<double>(t);
             return lhs.getDemand()>rhs.getDemand();
         });
    while(!unserved.empty()) {
        vehicles.emplace_back(Vehicle(xd,yd,maxCap));
        while(1) {
            int x=vehicles.at(k).getX();
            int y=vehicles.at(k).getY();
            double t=vehicles.at(k).getTime();
            Vehicle& vehicle=vehicles.at(k);
            int cap=vehicle.getCapacity();


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
double getVal(std::vector<Client> clients, int maxCap, Client depot, std::vector<std::vector<int>> sol) {
    double maxt=0;
    for (int route = 0; route < sol.size(); route++) {
        double t=0;
        int cap=0;
        int x=depot.getX();
        int y=depot.getY();
        for (int vertex = 0; vertex < sol.at(route).size(); vertex++) {
            Client clientI=clients.at(sol.at(route).at(vertex)-1);
            double distance=clientI.getDistance(x,y);
            double finalDistance=distance+t>=clientI.getReadyTime()? distance+clientI.getServiceTime():clientI.getReadyTime()+clientI.getServiceTime()-t;
            bool canReturn=t+finalDistance+depot.getDistance(clientI.getX(),clientI.getY())<=depot.getDueDate();
            cap+=clientI.getDemand();
            if(cap>maxCap || !canReturn || t+finalDistance-clientI.getServiceTime()>clientI.getDueDate()) {
                return -1;
            }
            x=clientI.getX();
            y=clientI.getY();
            t+=finalDistance;
        }
        maxt+=t+depot.getDistance(x,y);
    }
    return maxt;
}
std::pair<int,double> checkSolution(std::vector<Client> clients, int maxCap, Client depot, std::vector<int> v) {
    int x=depot.getX();
    int y=depot.getY();
    int cap=0;
    double t=0;
    double maxt=0;
    int masz=0;
    int i=0;
    while (i<v.size()) {
        Client clientI=clients.at(v.at(i)-1);
        if(x==depot.getX() && y==depot.getY() && clientI.getX()!= depot.getX() && clientI.getY()!= depot.getY()) {
            double distance=clientI.getDistance(x,y);
            double finalDistance=distance>clientI.getReadyTime()? distance+clientI.getServiceTime():clientI.getReadyTime()+clientI.getServiceTime();
            bool canReturn=finalDistance+depot.getDistance(clientI.getX(),clientI.getY())<=depot.getDueDate();
            if(!canReturn || clientI.getDemand() >maxCap) {
                return {-1,-1};
            }
                x=clientI.getX();
                y=clientI.getY();
                t+=finalDistance;
                cap+=clientI.getDemand();
                i++;
            //std::cout<<clientI.getId()<<" ";
        }else {
            double distance=clientI.getDistance(x,y);
            double finalDistance=distance+t>=clientI.getReadyTime()? distance+clientI.getServiceTime():clientI.getReadyTime()+clientI.getServiceTime()-t;
            bool canReturn=t+finalDistance+depot.getDistance(clientI.getX(),clientI.getY())<=depot.getDueDate();
            if(canReturn && cap+clientI.getDemand()<=maxCap && t+finalDistance-clientI.getServiceTime()<=clientI.getDueDate()) {
                x=clientI.getX();
                y=clientI.getY();
                t+=finalDistance;
                cap+=clientI.getDemand();
                i++;
                //std::cout<<clientI.getId()<<" ";
            }else {
                t+=depot.getDistance(x,y);
                x=depot.getX();
                y=depot.getY();
                maxt+=t;
                t=0;
                cap=0;
                masz+=1;

            }

        }

    }
    maxt+=t;
    t=0;
    cap=0;
    masz+=1;
    return {masz,maxt};
}
double genetic(std::vector<std::vector<int>> &population,std::vector<Client> clients, int maxCap,Client depot) {
    std::vector<Vehicle> vehicles;
    std::vector<std::vector<int>> crossovers;
    std::vector<int> winners;
    for (int i = 0; i < PSIZE; i+=2) {
        std::pair<int,double> a=checkSolution(clients,maxCap,depot,population.at(i));
        std::pair<int,double> b=checkSolution(clients,maxCap,depot,population.at(i+1));
        if(a.first*100+a.second*0.001>100*b.first+0.001+b.second) {
            winners.push_back(i);
        }else {
            winners.push_back(i+1);
        }
    }
    for(int z=1; z<winners.size(); z++) {
        std::vector<int> first;
        int fir=winners.at(z-1);
        int sec=winners.at(z);

        int i=rand()%population.at(0).size();
        int j=rand()%population.at(0).size();
        while (j<i) {
            j=rand()%population.at(0).size();
        }
        //std::cout<<j<<std::endl;
        for(int w=0;w<i;w++) {
            first.push_back(-1);
        }
        for(int k=i;k<j;k++) {
                first.push_back(population.at(fir).at(k));
        }
        int circ=j;
        while(first.size()<clients.size()) {
            if(std::find(first.begin(), first.end(), population.at(sec).at(circ)) != first.end()) {
                if(circ==clients.size()-1) {
                    circ=0;
                }else {
                    circ++;
                }
            }else {
                first.push_back(population.at(sec).at(circ));
            }
        }
        int w=0;
        while(w<i) {
            if(std::find(first.begin(), first.end(), population.at(sec).at(circ)) != first.end()) {
                if(circ==clients.size()-1) {
                    circ=0;
                }else {
                    circ++;
                }
            }else {
                first.at(w)=population.at(sec).at(circ);
                w++;
            }

        }

      if(rand()%100<5) {
          for (int sw = 1; sw < first.size(); sw++) {
              std::pair<int,double>  prev=checkSolution(clients,maxCap,depot,first);
              int buff=first.at(sw-1);
              first.at(sw-1)= first.at(sw);
              first.at(sw) = buff;
              std::pair<int,double>  nsol=checkSolution(clients,maxCap,depot,first);
              if(prev.first<nsol.first || (prev.first==nsol.first && prev.second<nsol.second)) {
                  buff=first.at(sw-1);
                  first.at(sw-1)= first.at(sw);
                  first.at(sw) = buff;
              }
              // std::cout << sw<< std::endl;
          }

      }else  if(rand()%100<=0) {

            int  firstL=rand()%clients.size();
            int  secondL=rand()%clients.size();
            while (firstL==secondL) {
                secondL=rand()%clients.size();
            }
            int buff=first.at(firstL);
            first.at(firstL)= first.at(secondL);
            first.at(secondL) = buff;



      }
        crossovers.push_back(first);
    }
    while (population.size()>PSIZE/2+1) {
        //std::cout << population.size()<<std::endl;
        int actsol=0;
        std::pair<int,double> max=checkSolution(clients,  maxCap, depot,population.at(0));
        int out=0;
        for(auto v : population) {
            std::pair<int,double> act=checkSolution(clients, maxCap, depot, v);
            if(act.second*act.first>max.second*max.first) {
                out=actsol;
                max=act;
            }
            actsol++;
        }
        population.erase(population.begin()+out);
    }
    for(auto v : crossovers) {
        population.push_back(v);

    }
    return 0;
}
void remove(std::vector<std::vector<int>> &parent, int r1) {
    for (int k = 0; k < parent.size(); k++) {
        for (int j = 0; j < parent.at(k).size();j++) {
                if(parent.at(k).at(j) == r1) {
                    parent.at(k).erase(parent.at(k).begin()+j);
                    return;
                }
        }
    }
}
std::pair<int,double> Routes_clustered_aproach_DNA(std::vector<std::vector<std::vector<int>>> &population,  std::vector<Client> clients, int maxCap,Client depot) {
    //tournament
    std::vector<int> winners;
    std::vector<std::vector<std::vector<int>>> crossovers;
    srand(time(0));
    for(int i=0; i<PSIZE; i+=10) {
        int winner=i;
        for (int j = i+1; j < i+10; j++){
            if(population.at(winner).size()* getVal(clients,maxCap,depot,population.at(winner))>population.at(j).size()*getVal(clients,maxCap,depot,population.at(j)))  {
                winner=j;
            }
        }
        winners.push_back(winner);
    }
    for (int i = 0; i < winners.size()-1; i++) {
        std::vector<std::vector<int>> parent1=population.at(winners.at(i));
        std::vector<std::vector<int>> parent2=population.at(winners.at(i+1));
        int  parent1RandomRouteIndex=rand()%parent1.size();
        int  parent2RandomRouteIndex=rand()%parent2.size();
      //  parent1.erase(parent1.begin()+parent1RandomRouteIndex);
        //parent2.erase(parent2.begin()+parent2RandomRouteIndex);
        std::vector<int> r1= population.at(winners.at(i)).at(parent1RandomRouteIndex);
        std::vector<int> r2= population.at(winners.at(i+1)).at(parent1RandomRouteIndex);
        for (auto r: r2) {
            remove(parent1,r);
        }
        for (auto r: r1) {
            remove(parent2,r);
        }

        for (int k = 0; k < r2.size(); k++) {
            std::pair<int,int> bestfit={-1,-1};
           double bfval=100000000000;
            bool flag=false;
            for (int j = 0; j < parent1.size(); j++) {
                if(flag) {
                    break;
                }
                for (int z = 0; z < parent1.at(j).size(); z++) {
                    parent1.at(j).insert(parent1.at(j).begin()+z,r2.at(k));
                    double v=getVal(clients,maxCap,depot,parent1);
                    if(v==-1) {
                        parent1.at(j).erase(parent1.at(j).begin()+z);
                        continue;
                    }
                    if(v<bfval) {
                        bfval=v;
                        bestfit={j,z};
                    }
                    parent1.at(j).erase(parent1.at(j).begin()+z);
                }
            }
            if(bestfit.first!=-1) {
                parent1.at(bestfit.first).insert(parent1.at(bestfit.first).begin()+bestfit.second,r2.at(k));

            }else {
                std::vector<int> nr;
                nr.push_back(r2.at(k));
                parent1.push_back(nr);
            }

        }
        for (int k = 0; k < r1.size(); k++) {
            std::pair bestfit={-1,-1};
            double bfval=100000000000;
         for (int j = 0; j < parent2.size(); j++) {
                for (int z = 0; z < parent2.at(j).size(); z++) {
                    parent2.at(j).insert(parent2.at(j).begin()+z,r1.at(k));
                    double v=getVal(clients,maxCap,depot,parent2);
                    if(v==-1) {
                        parent2.at(j).erase(parent2.at(j).begin()+z);
                        continue;
                    }
                    if(v<bfval) {
                        bfval=v;
                        bestfit={j,z};
                    }
                    parent2.at(j).erase(parent2.at(j).begin()+z);
                }
            }
            if(bestfit.first!=-1) {
                parent2.at(bestfit.first).insert(parent2.at(bestfit.first).begin()+bestfit.second,r1.at(k));

            }else {
                std::vector<int> nr;
                nr.push_back(r1.at(k));
                parent2.push_back(nr);
            }

        }
        crossovers.push_back(parent1);
        crossovers.push_back(parent2);
    }
    double min=20000000;

    for(auto v : crossovers) {
        population.push_back(v);
    }
    while (population.size()>PSIZE) {
        //std::cout << population.size()<<std::endl;
        int actsol=0;
        double max=getVal(clients,maxCap,depot,population.at(0));
        int out=0;
        for(auto v : population) {
            double act=getVal(clients, maxCap, depot, v);
            if(v.size()*act>max*population.at(out).size()) {
                out=actsol;
                max=act;

            }else if(act<min) {
                min=act;
            }
            actsol++;
        }
        population.erase(population.begin()+out);
    }
    return {0,min};
}


int main(int argc, char *argv[])
{
    if(argc!=3) {
        argv[1]=(char*)"input/m2kvrptw-0.txt";
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
    double max=5576016.77462;

    int z=0;
    int min=0;
     std::vector<std::vector<std::vector<int>>>population;
    long beg =time(NULL);
 /*while(population.size()<PSIZE) {
        std::vector<int> sol;
        for(int i=1; i<=clients.size(); i++) {
            sol.push_back(i );
        }
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(sol.begin(),sol.end(),g);

        population.push_back(sol);

    }*/
  srand(time(NULL));
   while(z<PSIZE) {
       std::vector<std::vector<int>> sl;;
        double l=grasp(vehicles,clients,maxCap,depot);
        for (auto veh: vehicles) {
            sl.push_back(veh.getRoute());
        }
        population.emplace_back(sl);
        vehicles.clear();
        z++;
    }
    /*for(int i=0; i<PSIZE; i++) {
        for(int j=0; j<clients.size(); j++) {
            std::cout << " " << population.at(i).at(j);
        }
        std::cout << std::endl;

    }*/
    std::cout<<"done"<<time(NULL)-beg<<std::endl;
    int iters=0;
    double mm=20000000;
    int point=1400;
    while(1) {
        std::pair<int,double> value =Routes_clustered_aproach_DNA(population,clients,maxCap,depot);
        iters++;
        bool flag=true;
        while (flag) {
            flag=false;
            for (int i = 0; i <population.size() ; i++) {
                for (int j = 0; j <population.at(i).size() ; j++) {
                    if(population.at(i).at(j).size()==0) {
                        flag=true;
                        population.at(i).erase(population.at(i).begin()+j);
                    }
                }
            }

        }
        for (int i = 0; i <population.size() ; i++) {
            if(population.at(i).size()<=mm) {
                int sum=0;
                for (int k = 0; k < population.at(i).size(); k++) {
                    for (int z = 0; z < population.at(i).at(k).size(); z++) {
                        sum+=1;
                    }
                };
                mm=population.at(i).size();
                double check=getVal(clients,maxCap,depot,population.at(i));
                max=check<max?check:max;
            }
        }
            std::cout <<std::fixed<<std::setprecision(5 )<<time(NULL)-beg<<" "<< iters << " "<<max<<" "<<mm <<std::endl;

    }
    std::cout<<"done"<<time(NULL)-beg<<std::endl;
    std::cout<<std::fixed<<std::setprecision(5)<<max<<" "<<mm<<" "<<iters<<std::endl;
    file.close();
    Ofile.close();

    return 0;
}
