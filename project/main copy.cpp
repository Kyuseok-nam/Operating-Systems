#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <queue>
#include <list>

//class that defines each Process
class Process{
public:
    Process(std::string c,int a, int b){id = c; arrival_time = a; num_cpu_burst = b; io_end_time=-1; preempt = false;}
    
    //accessor
    int get_arrival_time() const{return arrival_time;}
    int get_num_cpu_burst() const{return num_cpu_burst;}
    std::string get_id() const{return id;}
    int get_io_time() const{return io_end_time;}
    int get_tau() const{return tau;}
    int get_remain_tau() const{return remain_tau;}
    int get_cpu_remain_time() const{return cpu_remain_time;}
    int get_actual_burst() const{return actual_cpu;}
    bool check_preempt() const{return preempt;}
    bool get_cpu_done() const{return cpu_done;}
    std::list<int> get_burst() const{return bursts;}
    
    
    //modifier
    void set_cpu_io(int n){bursts.push_back(n);}
    void set_id(std::string c){id = c;}
    void set_tau(int t){tau = t;}
    void set_remain_tau(int t){remain_tau =t;}
    void set_io_time(int t){io_end_time =t;}
    void pop_burst(){bursts.pop_front();}
    void set_remain_burst(int t){bursts.front()-= t;}
    void set_cpu_remain_time(int t){cpu_remain_time = t;}
    void set_actual_burst(int t){actual_cpu = t;}
    void reduce_num_cpu_burst(){num_cpu_burst--;}
    void set_preempt(bool check){preempt =check;}
    void set_cpu_done(bool b){cpu_done = b;}

private:
    std::string id;
    int arrival_time;
    int actual_cpu;
    int num_cpu_burst;
    int remain_tau;
    int tau;
    int io_end_time;
    int cpu_remain_time;
    bool preempt;
    bool cpu_done;
    std::list<int> bursts;
    //int total_wait_time;
};

//operator overloading for priorty queue with process class inside;
bool operator<(const Process& p1,const Process& p2){
    return p1.get_remain_tau() > p2.get_remain_tau() || (p1.get_remain_tau() == p2.get_remain_tau() && p1.get_id() > p2.get_id());
}
//function for printing ready queue
void print_q(std::queue<Process> q){
    std::string s ="";
    if(q.empty()){
        std::cout<<"empty";
        return;
    }
    while(!q.empty()){
        Process a = q.front();
        q.pop();
        std::cout<<a.get_id();
    }
    return;
}
//function for printing ready priority_queue
void print_pq(std::priority_queue<Process> q){
    std::string s ="";
    if(q.empty()){
        std::cout<<"empty";
        return;
    }
    while(!q.empty()){
        Process a = q.top();
        q.pop();
        std::cout<<a.get_id();
    }
    return;
}

void print_sim(std::ofstream& file,double b_t, double w_t, double t_a, int c_s, int p, double c_u){
    file<<std::fixed<<std::setprecision(3)<<"-- average CPU burst time: "<<b_t<<" ms"<<std::endl;
    file<<std::setprecision(3)<<"-- average wait time: "<<w_t<<" ms"<<std::endl;
    file<<std::setprecision(3)<<"-- average turnaround time: "<<t_a<<" ms"<<std::endl;
    file<<"-- total number of context switches: "<<c_s<<std::endl;
    file<<"-- total number of preemptions: "<<p<<std::endl;
    file<<std::setprecision(3)<<"-- CPU utilization: "<<c_u<<"%"<<std::endl;
}

//function for getting random number
double next_enp_r(double lambda,unsigned int th);
//function for getting drand number
double next_enp_d(double lambda,unsigned int th);
//sort function for process by arrival time
bool sort_processes(const Process &p1, const Process &p2);
//function for getting next anticipated tau value
int next_tau(double alpha, int tau, int burst);
//FCFC function
void FCFS(std::vector<Process> processes,int t_cs,int& w,int&c,int&rt);
//RR function
void RR(std::vector<Process> processes,int t_cs,int t_slice,int&w,int&c,int&p,int&rt);
//SJF function
void SJF(std::vector<Process> processes,int t_cs,double alpha,int&w,int&c,int&rt);
//SRF function
void SRT(std::vector<Process> processes,int t_cs,double alpha,int t_slice,int&w,int&c,int&p,int&rt);

int main(int argc, char* argv[]){
    char *ptr;
    int n = atoi(argv[1]);
    long int seed = strtol(argv[2],&ptr,10);
    double lambda = atof(argv[3]);
    unsigned int th = atoi(argv[4]);
    int t_cs = atoi(argv[5]);
    double alpha = atof(argv[6]);
    int t_slice = atoi(argv[7]);
    double initial_tau = 1/lambda;
    double total_cpu_burst = 0;
    double total_num_cpu = 0;
    
    //stores all the process in vector processes
    std::vector<Process> processes;
    srand48(seed);
    //process id
    char id = 'A';
    //loop for input number of processes
    for(int i=0;i<n;i++){
        std::string s = "";
        s.push_back(id);
        double r_num = next_enp_r(lambda,th);
        int a_time = floor(r_num);
        r_num = next_enp_d(lambda,th);
        int n_cpu_burst = ceil(r_num);
        total_num_cpu+=n_cpu_burst;
        int n_burst = ceil(r_num)*2-1;
        Process p(s,a_time,n_cpu_burst);
        p.set_tau(initial_tau);
        p.set_remain_tau(initial_tau);
        p.set_cpu_remain_time(p.get_burst().front());
        std::cout<<"Process "<<s<<" (arrival time "<<a_time<<" ms) "<<n_cpu_burst;
        if(n_cpu_burst==1){
            std::cout<<" CPU burst (tau ";
        }
        else{
            std::cout<<" CPU bursts (tau ";
        }
        std::cout<<p.get_tau()<<"ms)"<<std::endl;
        //store the cpu burst time and i/o burst time for each process
        for(int i=0;i<n_burst;i++){
            r_num = next_enp_r(lambda,th);
            int burst = ceil(r_num);
            if(i%2==0){
                p.set_cpu_io(burst);
                total_cpu_burst+=burst;
                std::cout<<"--> CPU burst "<<burst<<" ms";
            }
            else{
                p.set_cpu_io(burst*10);
                std::cout<<" --> I/O burst "<<burst*10<<" ms"<<std::endl;
            }
            
        }
        std::cout<<std::endl;
        //set the acutal first cpu burst time
        p.set_actual_burst(p.get_burst().front());
        p.set_cpu_done(true);
        //increment id
        id+=1;
        //store the process to processes vector
        processes.push_back(p);
        //sort processes vector by arrival time
    }
    std::sort(processes.begin(),processes.end(),sort_processes);
    std::cout<<std::endl;
    //create simout file
    std::ofstream file("simout.txt");
    double average_cpu_burst = ceil((total_cpu_burst/total_num_cpu)*1000)/1000;
    double average_wait_time;
    double average_turn_time;
    double cpu_util;
    int w = 0;
    int c = 0;
    int p = 0;
    int rt = 0;
    //FCFS algorithm
    FCFS(processes,t_cs,w,c,rt);
    average_wait_time=ceil((w/total_num_cpu)*1000)/1000;
    average_turn_time = ceil((total_cpu_burst+w)/total_num_cpu*1000)/1000 + t_cs;
    cpu_util = ceil((total_cpu_burst/rt*100)*1000)/1000;
    file<<"Algorithm FCFS"<<std::endl;
    print_sim(file,average_cpu_burst,average_wait_time,average_turn_time,c,0,cpu_util);
    std::cout<<std::endl;
    //SJF
    SJF(processes,t_cs,alpha,w,c,rt);
    average_wait_time=ceil((w/total_num_cpu)*1000)/1000;
    average_turn_time = ceil((total_cpu_burst+w)/total_num_cpu*1000)/1000 + t_cs;
    cpu_util = ceil((total_cpu_burst/rt*100)*1000)/1000;
    file<<"Algorithm SJF"<<std::endl;
    print_sim(file,average_cpu_burst,average_wait_time,average_turn_time,c,0,cpu_util);
    std::cout<<std::endl;
    //SRT
    SRT(processes,t_cs,alpha,t_slice,w,c,p,rt);
    average_wait_time=ceil((w/total_num_cpu)*1000)/1000;
    average_turn_time = ceil((total_cpu_burst+w)/total_num_cpu*1000)/1000 + t_cs;
    cpu_util = ceil((total_cpu_burst/rt*100)*1000)/1000;
    file<<"Algorithm SRT"<<std::endl;
    print_sim(file,average_cpu_burst,average_wait_time,average_turn_time,c,p,cpu_util);
    std::cout<<std::endl;
    //RR
    RR(processes,t_cs,t_slice,w,c,p,rt);
    average_wait_time=ceil((w/total_num_cpu)*1000)/1000;
    average_turn_time = ceil((total_cpu_burst+w)/total_num_cpu*1000)/1000 + t_cs;
    cpu_util = ceil((total_cpu_burst/rt*100)*1000)/1000;
    file<<"Algorithm RR"<<std::endl;
    print_sim(file,average_cpu_burst,average_wait_time,average_turn_time,c,p,cpu_util);
    
    
    
    file.close();
   
    
    
    return EXIT_SUCCESS;
}

//gets the next exponential number
double next_enp_r(double lambda,unsigned int th){
    double d = drand48();
    double r = -log(d)/lambda;
    if ( r > th ){
        while(r>th){
            d = drand48();
            r = -log(d)/lambda;
        }
    }
    return r;
}
//gets number from drand48()
double next_enp_d(double lambda,unsigned int th){
    double d = drand48();
    double r = -log(d)/lambda;
    if ( r > th ){
        while(r>th){
            d = drand48();
            r = -log(d)/lambda;
        }
    }
    return d*100;
}

//helper function for sorting the process by arrival time
bool sort_processes(const Process &p1, const Process &p2){
    return p1.get_arrival_time()<p2.get_arrival_time();
}

//gets the next anticipated burst time
int next_tau(double alpha, int tau, int burst){
    int n_tau;
    n_tau = ceil((alpha*burst) + (1-alpha)*tau);
    return n_tau;
}


void FCFS(std::vector<Process> processes,int t_cs,int &w,int&c,int&rt){
    //number of processes
    int n_pro = processes.size();
    int time = 0;
    //index for processes vector
    int index = 0;
    //number of done processes
    int n_done = 0;
    //int for checking when the process is done using CPU
    int cpu_end_time = -1;
    //boo for checking cpu is empty
    bool cpu_empty = true;
    int cpu_in_time = -1;
    int cpu_done_swith_time = -1;
    int total_wait_time = 0;
    int cs=0;
    //ready queue
    std::queue<Process> Q;
    //CPU
    std::vector<Process> cpu;
    //IO
    std::list<Process> IO;
    
    std::cout<<"time 0ms: Simulator started for FCFS [Q empty]"<<std::endl;
    
    //run loop until processes done are equal to total number of processes
    while(n_done<n_pro){
        //if processes arrvial time happens
        if(index<n_pro){
            if(processes[index].get_arrival_time()==time){
                //insert to ready queue
                Q.push(processes[index]);
                //string that tracks inside of Que
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<processes[index].get_id()<<" arrived; added to ready queue [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                index+=1;
            }
        }
        //check if a process is done with I/O operation
        std::list<Process>::iterator itr1 = IO.begin();
        while(itr1!=IO.end()){
            //if process io done time is time
            if(itr1->get_io_time()==time){
                Process t = *itr1;
                //erase that process out of IO
                itr1 = IO.erase(itr1);
                //insert to ready queue
                Q.push(t);
                //pprint statement
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<t.get_id()<<" completed I/O; added to ready queue [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                continue;
            }
            itr1++;
        }
        //if cpu is done with the process
        if(cpu_end_time==time){
            Process x = cpu[0];
            //remove process from cpu
            cpu.pop_back();
            //if that process terminates
            if(x.get_num_cpu_burst()==0){
                n_done+=1;
                std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" terminated [Q ";
                print_q(Q);
                std::cout<<"]"<<std::endl;
            }
            //if that process goes to I/O
            else{
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" completed a CPU burst; "<<x.get_num_cpu_burst();
                    if(x.get_num_cpu_burst()==1){
                        std::cout<<" burst to go [Q ";
                    }
                    else{
                        std::cout<<" bursts to go [Q ";
                    }
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                int io_time = time + (t_cs/2)+x.get_burst().front();
                x.set_io_time(io_time);
                x.pop_burst();
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" switching out of CPU; will block on I/O until time "<<io_time<<"ms [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                IO.push_back(x);
            }
            cpu_done_swith_time = time+t_cs/2;
        }
        //context switch out
        if(cpu_done_swith_time == time){
            cpu_empty = true;
            cpu_done_swith_time = -1;
        }
        //if cpu is empty and there is a process in the ready Queue
        if(cpu_empty && Q.size()>0){
            cpu_done_swith_time = -1;
            //get the first process in ready queue
            Process a = Q.front();
            //sets when the cpu is done using
            cpu_end_time = time + a.get_burst().front()+(t_cs/2);
            cpu_in_time = time+t_cs/2;
            //removes process from queue
            Q.pop();
            //put the process into the CPU
            cpu.push_back(a);
            cpu_empty = false;
        }
        //if context switch into the cpu is done
        if(time == cpu_in_time && cpu_empty == false){
            Process a = cpu[0];
            if(time<1000){
                std::cout<<"time "<<time<<"ms: Process "<<a.get_id()<<" started using the CPU for ";
                std::cout<<a.get_burst().front()<<"ms burst [Q ";
                print_q(Q);
                std::cout<<"]"<<std::endl;
            }
            //remove the first cpu burst from the Process
            a.pop_burst();
            //reduce the #of cpu burst for that process
            a.reduce_num_cpu_burst();
            //remove Process from the ready Queue
            //print statement
            cpu[0]= a;
            cpu_in_time = -1;
            cs+=1;
        }
        total_wait_time+=Q.size();
        time+=1;
    }
    time-=1;
    time+=(t_cs/2);
    w = total_wait_time;
    c = cs;
    rt = time;
    std::cout<<"time "<<time<<"ms: Simulator ended for FCFS [Q empty]"<<std::endl;
}


//Round Robin Algorithm
void RR(std::vector<Process> processes,int t_cs,int t_slice,int&w,int&c,int&p,int&rt){
    //number of processes
    int n_pro = processes.size();
    int time = 0;
    //index for processes vector
    int index = 0;
    //number of done processes
    int n_done = 0;
    //int for checking when the process is done using CPU
    int cpu_end_time = -1;
    //boo for checking cpu is empty
    bool cpu_empty = true;
    int cpu_done_swith_time = -1;
    int cpu_in_time = -1;
    int total_wait_time = 0;
    int cs =0;
    int pt = 0;
    //ready queue
    std::queue<Process> Q;
    //CPU
    std::vector<Process> cpu;
    //IO
    std::list<Process> IO;
    
    std::cout<<"time "<<time<<"ms: Simulator started for RR with time slice "<<t_slice<<"ms [Q empty]"<<std::endl;
    
    //run loop until processes done are equal to total number of processes
    while(n_done<n_pro){
        
        //if processes arrvial time happens
        if(index<n_pro){
            if(processes[index].get_arrival_time()==time){
                //insert to ready queue
                Q.push(processes[index]);
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<processes[index].get_id()<<" arrived; added to ready queue [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                index+=1;
            }
        }
        //check if a process is done with I/O operation
        std::list<Process>::iterator itr1 = IO.begin();
        while(itr1!=IO.end()){
            //if process io done time is time
            if(itr1->get_io_time()==time){
                Process t = *itr1;
                //erase that process out of IO
                itr1 = IO.erase(itr1);
                //insert to ready queue
                Q.push(t);
                //print statement
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<t.get_id()<<" completed I/O; added to ready queue [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                continue;
            }
            itr1++;
        }
        
        //if cpu is done with the process
        if(cpu_end_time==time){
            Process x = cpu[0];
            //remove process from cpu
            cpu.pop_back();
            //if that process terminates
            if(x.get_num_cpu_burst()==0){
                n_done+=1;
                std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" terminated [Q ";
                print_q(Q);
                std::cout<<"]"<<std::endl;
                //sets context switch out time;
                cpu_done_swith_time = time+t_cs/2;
            }
            //if that process was preempted
            else if(x.check_preempt() && Q.empty()==false){
                
                //sets context switch out time;
                cpu_done_swith_time = time+t_cs/2;
                //sets remaining time of cpu burst
                int remain_time = x.get_burst().front();
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Time slice expired; process "<<x.get_id()<<" preempted with "<<remain_time<<"ms to go [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                pt+=1;
                //add the process back to the ready queue
                Q.push(x);
            }
            //if time slice occurs but Q is empty stay in CPU with no context switch
            else if(x.check_preempt() && Q.empty()==true){
                //string that tracks inside of Que
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Time slice expired; no preemption because ready queue is empty [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                cpu_empty = false;
                //if cpu burst time is less than t_slice
                if(x.get_burst().front() <= t_slice){
                    //sets when the cpu is done using
                    cpu_end_time = time + x.get_burst().front();
                    x.set_preempt(false);
                    //remove the first cpu burst from the Process
                    x.pop_burst();
                    //reduce the #of cpu burst for that process
                    x.reduce_num_cpu_burst();
                }
                //if cpu burst time is longer than t_slice
                else{
                    x.set_preempt(true);
                    x.set_remain_burst(t_slice);
                    cpu_end_time = time + t_slice;
                }
                cpu.push_back(x);
            }
            //if that process goes to I/O
            else{
                //sets context switch out time;
                cpu_done_swith_time = time+t_cs/2;
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" completed a CPU burst; "<<x.get_num_cpu_burst();
                    if(x.get_num_cpu_burst()==1){
                        std::cout<<" burst to go [Q ";
                    }
                    else{
                        std::cout<<" bursts to go [Q ";
                    }
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                int io_time = time + (t_cs/2)+x.get_burst().front();
                x.set_io_time(io_time);
                x.pop_burst();
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" switching out of CPU; will block on I/O until time "<<io_time<<"ms [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                IO.push_back(x);
            }
        }
        //sets context swith out time as
        if(cpu_done_swith_time == time){
            cpu_empty = true;
            cpu_done_swith_time = -1;
        }
        
        //if cpu is empty and there is a process in the ready Queue
        if(cpu_empty && Q.size()>0){
            //get the first process in ready queue
            Process a = Q.front();
            //if cpu burst time is less or euqal than t_slice
            if(a.get_burst().front() <= t_slice){
                //sets when the cpu is done using
                cpu_end_time = time + a.get_burst().front()+(t_cs/2);
                a.set_preempt(false);
            }
            //if cpu burst time is longer than t_slice
            else{
                a.set_preempt(true);
                a.set_remain_burst(t_slice);
                cpu_end_time = time + t_slice +(t_cs/2);
            }
            cpu_in_time = time+t_cs/2;
            //removes process from queue
            Q.pop();
            //put the process into the CPU
            cpu.push_back(a);
            cpu_empty = false;
        }
        //if context switch into the cpu is done
        if(time == cpu_in_time && cpu_empty == false){
            Process a = cpu[0];
            if(time<1000){
                std::cout<<"time "<<time<<"ms: Process "<<a.get_id()<<" started using the CPU for ";
            }
            if(a.check_preempt()==true){
                if(time<1000){
                    std::cout<<"remaining "<<a.get_burst().front()<<"ms of "<<a.get_burst().front()<<"ms burst [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
            }
            else{
                if(time<1000){
                    std::cout<<a.get_burst().front()<<"ms burst [Q ";
                    print_q(Q);
                    std::cout<<"]"<<std::endl;
                }
                //remove the first cpu burst from the Process
                a.pop_burst();
                //reduce the #of cpu burst for that process
                a.reduce_num_cpu_burst();
            }
            //print statement
            cpu[0]= a;
            cpu_in_time = -1;
            cs+=1;
        }
        time+=1;
        total_wait_time+=Q.size();
    }
    time-=1;
    time+=(t_cs/2);
    w = total_wait_time;
    c = cs;
    p = pt;
    rt = time;
    std::cout<<"time "<<time<<"ms: Simulator ended for RR [Q empty]"<<std::endl;
    
}


void SJF(std::vector<Process> processes,int t_cs,double alpha,int&w,int&c,int&rt){
    //number of processes
    int n_pro = processes.size();
    int time = 0;
    //index for processes vector
    int index = 0;
    //number of done processes
    int n_done = 0;
    //int for checking when the process is done using CPU
    int cpu_end_time = -1;
    //boo for checking cpu is empty
    bool cpu_empty = true;
    //cpu switch out time
    int cpu_done_swith_time = -1;
    int cpu_in_time = -1;
    int total_wait_time = 0;
    int cs = 0;
    
    //ready queue
    std::priority_queue<Process> Q;
    //CPU
    std::vector<Process> cpu;
    //IO
    std::list<Process> IO;
    
    std::cout<<"time 0ms: Simulator started for SJF [Q empty]"<<std::endl;
    
    //run loop until processes done are equal to total number of processes
    while(n_done<n_pro){
        //if processes arrvial time happens
        if(index<n_pro){
            if(processes[index].get_arrival_time()==time){
                //insert to ready queue
                Process a = processes[index];
                a.set_remain_tau(a.get_tau());
                Q.push(a);
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<processes[index].get_id()<<" (tau "<<processes[index].get_tau()<<"ms) arrived; added to ready queue [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
                index+=1;
            }
        }
        //check if a process is done with I/O operation
        std::list<Process>::iterator itr1 = IO.begin();
        while(itr1!=IO.end()){
            //if process io done time is time
            if(itr1->get_io_time()==time){
                Process t = *itr1;
                //erase that process out of IO
                itr1 = IO.erase(itr1);
                //insert to ready queue
                Q.push(t);
                //print statement
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<t.get_id()<<" (tau "<<t.get_tau()<<"ms) completed I/O; added to ready queue [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
                continue;
            }
            itr1++;
        }
        
        //if cpu is done with the process
        if(cpu_end_time==time){
            Process x = cpu[0];
            //remove process from cpu
            cpu.pop_back();
            //set the next tau
            int c_tau = x.get_tau();
            int n_tau = next_tau(alpha,x.get_tau(),x.get_burst().front());
            //remove the first cpu burst from the Process
            x.pop_burst();
            //reduce the #of cpu burst for that process
            x.reduce_num_cpu_burst();
            //set process a's next anticipated cpu burst
            x.set_tau(n_tau);
            x.set_remain_tau(x.get_tau());
            
            //if that process terminates
            if(x.get_num_cpu_burst()==0){
                n_done+=1;
                std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" terminated [Q ";
                print_pq(Q);
                std::cout<<"]"<<std::endl;
            }
            //if that process goes to I/O
            else{
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" (tau "<<c_tau<<"ms) completed a CPU burst; "<<x.get_num_cpu_burst();
                    if(x.get_num_cpu_burst()==1){
                        std::cout<<" burst to go [Q ";
                    }
                    else{
                        std::cout<<" bursts to go [Q ";
                    }
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                    std::cout<<"time "<<time<<"ms: Recalculated tau from "<<c_tau<<"ms to "<<n_tau<<"ms for process "<<x.get_id()<<" [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
                int io_time = time + (t_cs/2)+x.get_burst().front();
                x.set_io_time(io_time);
                x.pop_burst();
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" switching out of CPU; will block on I/O until time "<<io_time<<"ms [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
                //insert to I/O
                IO.push_back(x);
                
            }
            cpu_done_swith_time = time+t_cs/2;
        }
        //context switch
        if(cpu_done_swith_time == time){
            cpu_empty = true;
            cpu_done_swith_time = -1;
        }
        
        //if cpu is empty and there is a process in the ready Queue
        if(cpu_empty && Q.size()>0){

            //get the first process in ready queue
            Process a = Q.top();
            //sets when the cpu is done using
            cpu_end_time = time + a.get_burst().front()+(t_cs/2);
            cpu_in_time = time+t_cs/2;
            //removes process from queue
            Q.pop();
            //put the process into the CPU
            cpu.push_back(a);
            cpu_empty = false;
        }
        //if context switch into the cpu is done
        if(time == cpu_in_time && cpu_empty == false){
            Process a = cpu[0];
            if(time<1000){
                std::cout<<"time "<<time<<"ms: Process "<<a.get_id()<<" (tau "<<a.get_tau()<<"ms) started using the CPU for ";
                std::cout<<a.get_burst().front()<<"ms burst [Q ";
                print_pq(Q);
                std::cout<<"]"<<std::endl;
            }
            //reset cpu in time
            cpu_in_time = -1;
            cs+=1;
        }
        time+=1;
        total_wait_time+=Q.size();
    }
    time-=1;
    time+=(t_cs/2);
    w = total_wait_time;
    c = cs;
    rt = time;
    std::cout<<"time "<<time<<"ms: Simulator ended for SJF [Q empty]"<<std::endl;
}


void SRT(std::vector<Process> processes,int t_cs,double alpha,int t_slice,int&w,int&c,int&p,int&rt){
    //number of processes
    int n_pro = processes.size();
    int time = 0;
    //index for processes vector
    int index = 0;
    //number of done processes
    int n_done = 0;
    //int for checking when the process is done using CPU
    int cpu_end_time = -1;
    //boo for checking cpu is empty
    bool cpu_empty = true;
    //cpu switch out time
    int cpu_done_swith_time = -1;
    int cpu_in_time = -1;
    bool switch_done = false;
    int total_wait_time = 0;
    int cs =0;
    int pt =0;
    
    //ready queue
    std::priority_queue<Process> Q;
    //CPU
    std::vector<Process> cpu;
    //IO
    std::list<Process> IO;
    
    std::cout<<"time 0ms: Simulator started for SRT [Q empty]"<<std::endl;
    //run loop until processes done are equal to total number of processes
    while(n_done<n_pro){
        //if cpu is not empty and context switch is done
        if(!cpu.empty() && switch_done){
            //decrement of the remaining cpu burst time by 1
            Process a = cpu[0];
            cpu.pop_back();
            int r_t = a.get_cpu_remain_time() -1;
            a.set_cpu_remain_time(r_t);
            a.set_remain_tau(a.get_remain_tau()-1);
            cpu.push_back(a);
        }
        //if processes arrvial time happens
        if(index<n_pro){
            if(processes[index].get_arrival_time()==time){
                //insert to ready queue
                Process a = processes[index];
                a.set_cpu_remain_time(a.get_burst().front());
                a.set_remain_tau(a.get_tau());
                Q.push(a);
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<processes[index].get_id()<<" (tau "<<processes[index].get_tau()<<"ms) arrived; added to ready queue [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
                index+=1;
            }
        }
        //check if a process is done with I/O operation
        std::list<Process>::iterator itr1 = IO.begin();
        while(itr1!=IO.end()){
            //if process io done time is time
            if(itr1->get_io_time()==time){
                Process t = *itr1;
                //set total actual burst time
                t.set_actual_burst(t.get_burst().front());
                //erase that process out of IO
                itr1 = IO.erase(itr1);
                //insert to ready queue
                Q.push(t);
                //get the first process in ready queue
                Process b = Q.top();
                //if there is a process in ready que that has less cpu burst time than
                //the process currently in cpu, preempt it
                if(cpu.empty()==false && Q.empty()==false && cpu[0].get_remain_tau() > Q.top().get_tau()&&switch_done){
                    Process a = cpu[0];
                    cpu.pop_back();
                    if(a.get_burst().front()==a.get_cpu_remain_time()){
                        a.set_cpu_done(true);
                    }
                    else{
                        a.set_cpu_done(false);
                    }
                    //print statement
                    if(time<1000){
                        std::cout<<"time "<<time<<"ms: Process "<<b.get_id()<<" (tau "<<b.get_tau()<<"ms) completed I/O; preempting "<<a.get_id()<<" [Q ";
                        print_pq(Q);
                        std::cout<<"]"<<std::endl;
                    }
                    pt+=1;
                    //pop process from queue
                    Q.pop();
                    //set remain cpu burst time for the process that is getting switched out from cpu
                    a.set_remain_burst(a.get_burst().front()-a.get_cpu_remain_time());
                    Q.push(a);
                    //switch in the other process to cpu
                    cpu.push_back(b);
                    switch_done = false;
                    cpu_empty = false;
                    cpu_in_time = time+t_cs;
                    cpu_end_time = time+t_cs+b.get_burst().front();
                }
                else{
                    //print statement
                    if(time<1000){
                        std::cout<<"time "<<time<<"ms: Process "<<t.get_id()<<" (tau "<<t.get_tau()<<"ms) completed I/O; added to ready queue [Q ";
                        print_pq(Q);
                        std::cout<<"]"<<std::endl;
                    }
                }
                continue;
            }
            itr1++;
        }
        
        //if cpu is done with the process
        if(cpu_end_time==time){
            Process x = cpu[0];
            //remove process from cpu
            cpu.pop_back();
            //set the next tau
            int c_tau = x.get_tau();
            int n_tau = next_tau(alpha,x.get_tau(),x.get_actual_burst());
            //remove the first cpu burst from the Process
            x.pop_burst();
            //reduce the #of cpu burst for that process
            x.reduce_num_cpu_burst();
            //set process a's next anticipated cpu burst
            x.set_tau(n_tau);
            x.set_remain_tau(n_tau);
            //if that process terminates
            if(x.get_num_cpu_burst()==0){
                n_done+=1;
                std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" terminated [Q ";
                print_pq(Q);
                std::cout<<"]"<<std::endl;
            }
            //if that process goes to I/O
            else{
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" (tau "<<c_tau<<"ms) completed a CPU burst; "<<x.get_num_cpu_burst();
                    if(x.get_num_cpu_burst()==1){
                        std::cout<<" burst to go [Q ";
                    }
                    else{
                        std::cout<<" bursts to go [Q ";
                    }
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                    std::cout<<"time "<<time<<"ms: Recalculated tau from "<<c_tau<<"ms to "<<n_tau<<"ms for process "<<x.get_id()<<" [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
                int io_time = time + (t_cs/2)+x.get_burst().front();
                x.set_io_time(io_time);
                x.pop_burst();
                if(time<1000){
                    std::cout<<"time "<<time<<"ms: Process "<<x.get_id()<<" switching out of CPU; will block on I/O until time "<<io_time<<"ms [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
                //reset cpu remain time
                x.set_cpu_remain_time(x.get_burst().front());
                x.set_cpu_done(true);
                IO.push_back(x);
            }
            switch_done=false;
            cpu_done_swith_time = time+t_cs/2;
        }
        //context switch
        if(cpu_done_swith_time == time){
            cpu_empty = true;
            cpu_done_swith_time = -1;
        }
        //if cpu is empty and there is a process in the ready Queue
        if(cpu_empty && Q.size()>0){
            //get the first process in ready queue
            Process a = Q.top();
            //sets when the cpu is done using
            cpu_end_time = time + a.get_burst().front()+(t_cs/2);
            cpu_in_time = time+t_cs/2;
            //removes process from queue
            Q.pop();
            //put the process into the CPU
            cpu.push_back(a);
            cpu_empty = false;
        }
        //if context switch into the cpu is done
        if(time == cpu_in_time && cpu_empty == false){
            Process b = cpu[0];
            if(time<1000){
                if(b.get_cpu_done()){
                    std::cout<<"time "<<time<<"ms: Process "<<b.get_id()<<" (tau "<<b.get_tau()<<"ms) started using the CPU for ";
                    std::cout<<b.get_burst().front()<<"ms burst [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
                else{
                    std::cout<<"time "<<time<<"ms: Process "<<b.get_id()<<" (tau "<<b.get_tau()<<"ms) started using the CPU for remaining ";
                    std::cout<<b.get_burst().front()<<"ms of "<<b.get_actual_burst()<<"ms burst [Q ";
                    print_pq(Q);
                    std::cout<<"]"<<std::endl;
                }
            }
            //reset cpu in time
            cpu_in_time = -1;
            switch_done=true;
            cs+=1;
        }
        //if process gets preempted from ready queue
        if(cpu.empty()==false && Q.empty()==false && cpu[0].get_remain_tau() > Q.top().get_tau()&&switch_done){
            Process a = cpu[0];
            cpu.pop_back();
            if(a.get_burst().front()==a.get_cpu_remain_time()){
                a.set_cpu_done(true);
            }
            else{
                a.set_cpu_done(false);
            }
            Process b = Q.top();
            //print statement
            if(time<1000){
                std::cout<<"time "<<time<<"ms: Process "<<b.get_id()<<" (tau "<<b.get_tau()<<"ms) will preempt "<<a.get_id()<<" [Q ";
                print_pq(Q);
                std::cout<<"]"<<std::endl;
            }
            pt+=1;
            //pop process from queue
            Q.pop();
            //set remain cpu burst time for the process that is getting switched out from cpu
            a.set_remain_burst(a.get_burst().front()-a.get_cpu_remain_time());
            Q.push(a);
            //switch in the other process to cpu
            cpu.push_back(b);
            switch_done = false;
            cpu_empty = false;
            cpu_in_time = time+t_cs;
            cpu_end_time = time+t_cs+b.get_burst().front();
        }
        time+=1;
        total_wait_time+=Q.size();
    }
    time-=1;
    time+=(t_cs/2);
    w = total_wait_time;
    c = cs;
    p = pt;
    rt = time;
    std::cout<<"time "<<time<<"ms: Simulator ended for SRT [Q empty]"<<std::endl;
}

