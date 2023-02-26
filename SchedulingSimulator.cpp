#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

//using namespace std;

vector<int> process_ids, arrival_times, burst_times, priorities, waiting_times, response_times, turnaround_times, begin_times, end_times, process_queue;
int time_quantum, total_time, total_processes, current_process, cpu_time, idle_time, wt, rt, tt;
float awt, art, att;
fstream file;

//Sorts the processes by a given attribute in ascending order to aid with scheduling 
//Returns list of process ids sorted by attribute
vector<int> sort_by(vector<int> attribute)
{
    vector<int> result;
    vector <pair <int, int>> processes_by_attribute; // = create_pair(attribute, process_ids);

    for (int i = 0; i < total_processes; i++)
        processes_by_attribute.push_back(make_pair(attribute[i], process_ids[i]));

    sort(processes_by_attribute.begin(), processes_by_attribute.end());

    for (int j = 0; j < total_processes; j++)
        result.push_back(processes_by_attribute[j].second);
    return result;
}

//First Come First Serve - each process is executed in the order of time of arrival (non-preemptive)
void fcfs()
{
    std::cout << "\n---------- First Come First Serve (FCFS) ----------\n";
    process_queue = sort_by(arrival_times);

    //Core execution algorithm - loops through list of processes (sorted by arrival in this case) until all are completed
    //Assumes that no CPU time is wasted in this case
    int t = 0;
    while (process_queue.size() > 0 || t < total_time)
    {
        current_process = process_queue[0] - 1; //Queue holds actual id's of processes, -1 gives their corresponding index for vectors

        begin_times[current_process] = t; std::cout << "\nProcess " << current_process + 1 << " starts at CPU time " << t;
        //Increment CPU time until current process' burst time is depleted
        for(int j = burst_times[current_process]; j > 0; j--, t++, cpu_time++)
            std::cout << "\nProgress left for Process " << current_process + 1 << " at time " << t << " is: " << j;             
        end_times[current_process] = t; std::cout << "\nProcess " << current_process + 1 << " ends at CPU time " << t;

        //Remove process from queue (beginning of vector) after completing burst
        process_queue.erase(process_queue.begin());          
    }
    std::cout << "\nAll processes done at time: " << t;
}

//Shortest Job First - each process is executed in order of burst time (non-preemptive)
void sjf()
{
    std::cout << "\n---------- Shortest Job First (SJF) ----------\n";

    //Sorting by arrival is highest priority, other attributes can be checked during execution
    process_queue = sort_by(arrival_times);

    int t = 0;
    while (process_queue.size() > 0 || t < total_time)
    {
        current_process = process_queue[0] - 1;

        //Must check for new case where two or more processes are ready and next-in-queue has larger burst time than subsequent process
        //Picks lowest burst time out of remaining processes
        for (int k = 1; k < process_queue.size(); k++)
        {
            auto next_in_queue = process_queue[k] - 1;
            
            if(burst_times[next_in_queue] < burst_times[current_process] && arrival_times[next_in_queue] <= t)
            {
                current_process = next_in_queue;
                swap(process_queue.at(0), process_queue.at(k));
            }
        }

        begin_times[current_process] = t; std::cout << "\nProcess " << current_process + 1 << " starts at CPU time " << t;
        for(int j = burst_times[current_process]; j > 0; j--, t++, cpu_time++)
        {
            std::cout << "\nProgress left for Process " << current_process + 1 << " at time " << t << " is: " << j;
        }             
        end_times[current_process] = t; std::cout << "\nProcess " << current_process + 1 << " ends at CPU time " << t;

        process_queue.erase(process_queue.begin());            
    }
    std::cout << "\nAll processes done at time: " << t;
}

//Preemptive Priority Scheduling - each process is executed in order of priority, and process can be preempted by a higher priority arriving
void pps()
{
    std::cout << "\n---------- Preemptive Priority Scheduling ----------\n";
    process_queue = sort_by(arrival_times);

    int t = 0;
    vector <int> cpu_time_left = burst_times;
    while (process_queue.size() > 0 || t < total_time)
    {
        current_process = process_queue[0] - 1;

        for(int j = 0; cpu_time_left[current_process] > 0; cpu_time_left[current_process]--, t++, cpu_time++)
        {   
            //Check to see if a process with a higher priority (lower integer value) has entered the queue at time t, and if so, make that process the current one and suspend the previous process
            //Must perform check at each time unit
            for (int k = 1; k < process_queue.size(); k++)
            {
                auto next_in_queue = process_queue[k] - 1;
                
                if(priorities[next_in_queue] < priorities[current_process] && arrival_times[next_in_queue] <= t)
                {
                    std::cout << "\nProcess " << next_in_queue + 1 << " (Priority " << priorities[next_in_queue] << ") arrives and preempts Process " << current_process + 1 << " (Priority " << priorities[current_process] << ") at time " << t;

                    //Save progress on current process before switching to next
                    current_process = next_in_queue;
                    swap(process_queue.at(0), process_queue.at(k));
                    
                }
            }
            if(cpu_time_left[current_process] == burst_times[current_process])
            { 
                begin_times[current_process] = t; 
                std::cout << "\nProcess " << current_process + 1 << " starts at CPU time " << t; 
            }
            std::cout << "\nProgress left for Process " << current_process + 1 << " at time " << t << " is: " << cpu_time_left[current_process];
        }             
        end_times[current_process] = t; std::cout << "\nProcess " << current_process + 1 << " ends at CPU time " << t;

        process_queue.erase(process_queue.begin());            
    }
    std::cout << "\nAll processes done at time: " << t;
}

//Round Robin - processes are in order of arrival time, but every n seconds (time quantum) the current process is preempted by the next-in-queue
void rr()
{
    std::cout << "\n---------- Round Robin (RR) ----------\n";
    std::cout << "Enter time quantum: ";
    std::cin >> time_quantum;
    process_queue = sort_by(arrival_times);

    int t = 0;
    while (process_queue.size() > 0 || t < total_time)
    {
        current_process = process_queue[0] - 1; 

        begin_times[current_process] = t; std::cout << "\nProcess " << current_process + 1 << " starts at CPU time " << t;

        for(int j = burst_times[current_process]; j > 0; j--, t++, cpu_time++)
            std::cout << "\nProgress left for Process " << current_process + 1 << " at time " << t << " is: " << j;             
        end_times[current_process] = t; std::cout << "\nProcess " << current_process + 1 << " ends at CPU time " << t;

        process_queue.erase(process_queue.begin());          
    }
    std::cout << "\nAll processes done at time: " << t;
}

void print_results()
{
    std::cout << "\n\n---------- STATISTICS AND RESULTS ----------";
    for(int k = 0; k < total_processes; k++)
    {
        rt = (begin_times[k] - arrival_times[k]); art += rt;
        tt = (end_times[k] - arrival_times[k]); att += tt;
        wt = (tt - burst_times[k]); awt += wt;
        std::cout << "\n\nProcess " << process_ids[k] << "\n\tBurst time: " << burst_times[k] << "\n\tPriority: " << priorities[k] << "\n\tArrival time: " << arrival_times[k] << "\n\tBegin time: " << begin_times[k] << "\n\tEnd time: " << end_times[k]  <<  "\n\tWaiting time: " << wt << "\n\tResponse time: " << rt << "\n\tTurnaround time: " << tt;
    }
    std::cout << "\n\nAverage waiting time: " << float(awt / total_processes);
    std::cout << "\nAverage response time: " << float(art / total_processes);
    std::cout << "\nAverage turnaround time: " << float(att / total_processes);
    std::cout << "\nCPU utilization rate: " << float(((cpu_time - idle_time) / cpu_time) * 100) << "%";
}

//Driver code
int main()
{
    std::cout << "Opening input file...\n";
    file.open("input.txt");

    switch(file.is_open())
    {
        case false:
            std::cout << "Could not open input.txt, please restart and try again.\n";
            break;
        default:
            std::cout << "Reading file contents...";
            int pid, at, bt, pr, n;
            while(!file.eof())
            {
                file >> pid >> at >> bt >> pr;
                process_ids.push_back(pid); //std::cout << "\nProcess ID: " << process_ids[n];
                arrival_times.push_back(at); //std::cout << "\nArrival time: " << arrival_times[n];
                burst_times.push_back(bt); total_time += bt; //std::cout << "\nBurst time: " << burst_times[n] << "\nCumulative total time: " << total_time;
                priorities.push_back(pr); //std::cout << "\nPriority: " << priorities[n];
                total_processes++; //std::cout << "\nTotal processes: " << total_processes;
                n++;
            }
            std::cout << "\nProcess data loaded!\n";
            file.close();
            
            waiting_times.resize(total_processes, 0);
            turnaround_times.resize(total_processes, 0);
            response_times.resize(total_processes, 0);
            begin_times.resize(total_processes, 0);
            end_times.resize(total_processes, 0);

            std::cout << "\nPlease choose a scheduling algorithm to test: \nFirst Come First Serve: 1\nShortest Job First: 2\nPre-emptive Priority Scheduling: 3\nRound Robin: 4\nExit program: 5\nSelection number: ";
            
            int input = 0;
            while(input < 1 || input > 5)
            {
                std::cin >> input;
                switch(input)
                {
                    case 1:
                        fcfs();
                        print_results();
                        break;
                    case 2:
                        sjf();
                        print_results();
                        break;
                    case 3:
                        pps();
                        print_results();
                        break;
                    case 4:
                        rr();
                        print_results();
                        break;
                    case 5:
                        std::cout << "\nExiting program... Goodbye!\n";
                        break;
                    default:
                        std::cout << "\nInput not recognized, please choose a valid option.\n";
                        break;
                }
            }
            break;
    }
    std::cout << "\n\n-----------------------------\n\nDone!";
}