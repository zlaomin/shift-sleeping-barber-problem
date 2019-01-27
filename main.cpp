#include <iostream>
#include <thread>
#include <time.h>
#include <unistd.h>// sleep conmmand
#include <semaphore.h>
#include <vector>
#include <mutex>
#include <cmath>//cmath is for radom()
using namespace std;
//------------
mutex seat_mutex;
mutex threads_mutex;
mutex working_lock;
vector <thread>  threads;
vector <int> seat;
//varble  declared in a function will store in stock area. 
//While in this program, muti-threads might a lot of space in it.
//To advoid that, I perfer to use stack 

//------------
void show();
void new_customers(int m);
void new_customers(int m);


class customer
{
public:
	int id;
	customer(int id){
		this->id = id;
        // cout<<"customer "<<this->id<<" readly"<<endl;
	};
	~customer(void){

	};

};

class barber
{
public:
    int doing = -1;
    bool working =false;
    void barber_sleeping(){
        while(true){
            if(!working){
                seat_mutex.lock();
                if(seat.size())
                    //using "==" will spend more time
                    //someone waitng
                {
                    this->doing = seat.front();
                }
                seat_mutex.unlock();
            }
    }
    }
    void work_for_who(int i){
        working_lock.lock();//insure no one else come in
        this->working = true;
        this->doing = i;
        this->barber_working();
        this->working = false;
        working_lock.unlock();
        return;
    }
    void barber_working(){
        int time = (rand() % 5) + 1;// random number 1 from to 5
        // time = 3;// change time for test
        sleep(time);
        return;
    }
};
barber bar;

void each_customers(int i,int leave){//0: leave, 1: wait, 2: do
    customer c(i);
    //according to Problem Description, leave should be made by customer thread
    if (!leave)
    {
        cout<<"customer "<<c.id<<" leave"<<endl;
        return;
    }
    if(leave == 2){
        bar.working = true;
        bar.work_for_who(c.id);
        return;
    }
    while(true){
        if(!bar.working){
            if(bar.doing == c.id){
                bar.working = true;
                seat_mutex.lock();
                seat.erase(seat.begin());
                seat_mutex.unlock();
                threads_mutex.lock();
                threads.erase(threads.begin());
                threads_mutex.unlock();
                bar.work_for_who(c.id);
                return;
            }
        }
    }
}

void show(){ // for test
    while(true){
        sleep(1);
        if(bar.working){
            cout<<"Barber cutting the hair of customer "<<bar.doing<<endl;
        }
        else{
            cout<<"barber sleeping"<<endl;
        }
        cout<<"Waiting room: ";
        seat_mutex.lock();
        for(int i = 0;i<seat.size();i++){
            cout<<seat.at(i)<<" ";
        }
        seat_mutex.unlock();
        cout<<endl;
    }
}

void new_customers(int m){
    int i =0;

	while(true){
        sleep(3);
        i++;
        // cout<<"new thread";
        if(!bar.working && seat.size() == 0)
//no one waiting and not working 
        {
            threads_mutex.lock();
            threads.push_back(thread(each_customers,i,2));
            threads.back().detach();
            threads.pop_back();
            threads_mutex.unlock();
        }
        else
        {
            if(seat.size() >= m)
            {
                threads_mutex.lock();
                threads.push_back(thread(each_customers,i,0));
                threads.back().detach();
                threads.pop_back();
                threads_mutex.unlock();
            }
            else
            {
                seat_mutex.lock();
                seat.push_back(i);
                seat_mutex.unlock();
                threads_mutex.lock();
                threads.push_back(thread(each_customers,i,1));
                threads.back().detach();
                threads_mutex.unlock();
            }
    }
        // cout<<"Customer "<<i<<" arrived"<<endl;

    }
}
void barber_on(){
    bar.barber_sleeping();
}
int main(){
	int m;
    while(true){
        cout<<"please in put the number of seat \n";
        cin>>m;
        if(m>=0){
            break;
        }
        else{
            cout<<"please re-input a number \n";
        }
    }
	thread new_customers_thread(new_customers,m);
    thread barber_thread(barber_on);
    thread show_thread(show);
    barber_thread.join();
    show_thread.join();
	new_customers_thread.join();
	// delete &c;
	return 0;
}