#include <iostream>
#include <fstream>
#include <bits/stdc++.h>

#define TIME_QUANTUM 5000
using namespace std;

class PCB
{
public:
    // populating from input
    int PID;
    string name;
    bool cpu_bound;
    bool is_burst2; // for deciding whether the first cpu burst is completed or not
    int priority;
    int arrival_time, original_arrival_time;
    int arrival_time_copy;
    int cpu_burst1;
    int io_burst;
    int cpu_burst2;
    string burst_type;
    // derived variables
    string state;
    int total_cpu_time;
    int start_time;       // timestamp of first-time cpu allocation
    int completion_time;  // timestamp of termination
    int waiting_time;     // turnaround time - cpu_time
    int turn_around_time; // completion time - arrival time
    int response_time;    // start time - arrival time
    // constructor
    PCB()
    {
        name = "idle";
        is_burst2 = false;
    }
    PCB(int _PID, string _name, bool _cpu_bound, int _priority, int _arrival_time, int _burst_time, int _io_burst, int _burst_time2)
    {
        PID = _PID;
        name = _name;
        cpu_bound = _cpu_bound;
        state = "READY";
        is_burst2 = false;
        priority = _priority;
        arrival_time = _arrival_time;
        original_arrival_time = arrival_time;
        arrival_time_copy = arrival_time;
        cpu_burst1 = _burst_time;
        io_burst = _io_burst;
        cpu_burst2 = _burst_time2;
        burst_type = "cpu_burst1";
        total_cpu_time = cpu_burst1 + cpu_burst2;
    }

    void setState(string state_name)
    {
        state = state_name;
    }

    void setBurstTime(string burst_type, int new_burst_time)
    {
        if (burst_type == "cpu_burst1")
        {
            cpu_burst1 = new_burst_time;
        }
        else if (burst_type == "cpu_burst2")
        {
            cpu_burst2 = new_burst_time;
        }
    }

    void setCompletionTime(int _completion_time)
    {
        completion_time = _completion_time;
    }

    void setStartTime(int _start_time)
    {
        start_time = _start_time;
    }

    void setArrivalTime(int time)
    {
        arrival_time = time;
    }

    void setBurstType(string type)
    {
        burst_type = type;
    }

    void setTurnAroundTime()
    {
        turn_around_time = completion_time - original_arrival_time;
    }

    void setResponseTime()
    {
        response_time = start_time - arrival_time;
    }

    void setWaitingTime()
    {
        waiting_time = turn_around_time - total_cpu_time;
    }

    int getStartTime()
    {
        return start_time;
    }

    int getCompletionTime()
    {
        return completion_time;
    }

    int getBurstTime(string burst_type)
    {
        if (burst_type == "cpu_burst1")
        {
            return cpu_burst1;
        }
        else if (burst_type == "cpu_burst2")
        {
            return cpu_burst2;
        }
        else if (burst_type == "io_burst")
        {
            return io_burst;
        }
        return 0;
    }

    string getBurstType()
    {
        return burst_type;
    }
};

bool readyQueueComparator(PCB p1, PCB p2)
{
    return p1.arrival_time < p2.arrival_time;
}

class RR
{
public:
    vector<PCB> ready_queue;
    queue<PCB> working_queue, running_queue;
    vector<PCB> terminated;
    int current_time;

    vector<string> split(string str, char delimiter)
    {
        int n = str.length();
        vector<string> result;
        string temp = "";
        for (int i = 0; i < n; i++)
        {
            char ch = str[i];
            if (ch != delimiter)
            {
                temp.append(1, ch);
            }
            else
            {
                result.push_back(temp);
                temp = "";
            }
        }
        if (temp != "")
        {
            result.push_back(temp);
        }
        return result;
    }

    void printQueue(vector<PCB> queue)
    {
        for (PCB pcb : queue)
        {
            cout << pcb.PID << "," << pcb.name << "," << pcb.cpu_bound << "," << pcb.priority << "," << pcb.arrival_time << "," << pcb.cpu_burst1 << "," << pcb.io_burst << "," << pcb.cpu_burst2 << endl;
        }
    }

    void populateReadyQueue(vector<string> process_input)
    {
        for (string line : process_input)
        {
            // fin >> line;
            // cout << line << endl;
            vector<string> splits = split(line, ' ');
            int pid = stoi(splits[0]);
            string p_name = splits[1];
            bool cpu_bound = true;
            if (splits[2] == "I")
            {
                cpu_bound = false;
            }
            int priority = stoi(splits[3]);
            int arrival_time = stoi(splits[4]) * 1000;
            int burst_time1 = stoi(splits[5]) * 1000;
            int io_burst_time = stoi(splits[6]) * 1000;
            int burst_time2 = stoi(splits[7]) * 1000;
            PCB new_pcb(pid, p_name, cpu_bound, priority, arrival_time, burst_time1, io_burst_time, burst_time2);
            ready_queue.push_back(new_pcb);
        }
        sort(ready_queue.begin(), ready_queue.end(), readyQueueComparator);
    }

    bool didProcessEnd(PCB current_pcb)
    {
        return current_pcb.getBurstTime("cpu_burst1") == 0 && current_pcb.getBurstTime("io_burst") == 0 && current_pcb.getBurstTime("cpu_burst2") == 0;
    }

    void setAttributes(PCB current_pcb)
    {
        if (current_pcb.getBurstTime("cpu_burst1") != 0)
        {
            return;
        }
        if (current_pcb.getBurstTime("io_burst") != 0)
        {
            int temp = current_time + current_pcb.getBurstTime("io_burst");
            current_pcb.setBurstTime("io_burst", 0);
            current_pcb.setState("BLOCK");
            current_pcb.setArrivalTime(temp);
            current_pcb.setState("RUNNING");
            current_pcb.setCompletionTime(temp);
            current_pcb.setBurstType("cpu_burst2");
            if (current_pcb.getBurstTime("cpu_burst2") != 0)
            {
                current_pcb.setBurstType("cpu_burst2");
                ready_queue.push_back(current_pcb);
                sort(ready_queue.begin(), ready_queue.end(), readyQueueComparator);
            }
        }
        else if (current_pcb.getBurstTime("cpu_burst2") != 0)
        {
            current_pcb.setBurstType("cpu_burst2");
            running_queue.push(current_pcb);
        }
    }

    void addNewProcessIfAny()
    {
        while (!ready_queue.empty() && (ready_queue.front()).arrival_time <= current_time)
        {
            PCB new_pcb = ready_queue.front();
            ready_queue.erase(ready_queue.begin());
            new_pcb.setState("RUNNING");
            running_queue.push(new_pcb);
        }
    }

    void performTasks()
    {
        while (!ready_queue.empty() || !running_queue.empty())
        {
            addNewProcessIfAny();
            if (running_queue.empty())
            {
                current_time++;
                continue;
            }
            PCB running_top = running_queue.front();
            // cout << "Runningtop: " << running_top.name << endl;
            running_queue.pop();
            int working_time;
            if (running_top.burst_type == "cpu_burst1")
            {
                working_time = running_top.getBurstTime("cpu_burst1");
                if (running_top.getStartTime() == 0)
                {
                    running_top.setStartTime(current_time);
                }
                if (working_time <= TIME_QUANTUM)
                {
                    current_time += working_time;
                    running_top.setBurstTime("cpu_burst1", 0);
                    running_top.setBurstType("io_burst");
                    running_top.setCompletionTime(current_time);
                    addNewProcessIfAny();
                }
                else
                {
                    current_time += TIME_QUANTUM;
                    running_top.setBurstTime("cpu_burst1", working_time - TIME_QUANTUM);
                    addNewProcessIfAny();
                    running_queue.push(running_top);
                }
            }
            else if (running_top.burst_type == "cpu_burst2")
            {
                working_time = running_top.getBurstTime("cpu_burst2");
                if (running_top.getStartTime() == 0)
                {
                    running_top.setStartTime(current_time);
                }
                if (working_time <= TIME_QUANTUM)
                {
                    current_time += working_time;
                    running_top.setBurstTime("cpu_burst2", 0);
                    running_top.setCompletionTime(current_time);
                    running_top.setState("TERMINATED");
                    running_top.setResponseTime();
                    running_top.setTurnAroundTime();
                    running_top.setWaitingTime();
                    terminated.push_back(running_top);
                    addNewProcessIfAny();
                }
                else
                {
                    current_time += TIME_QUANTUM;
                    running_top.setBurstTime("cpu_burst2", working_time - TIME_QUANTUM);
                    addNewProcessIfAny();
                    running_queue.push(running_top);
                }
            }
            if (didProcessEnd(running_top))
            {
                running_top.setState("TERMINATED");
                running_top.setResponseTime();
                running_top.setTurnAroundTime();
                running_top.setWaitingTime();
                terminated.push_back(running_top);
            }
            else
            {
                setAttributes(running_top);
            }
        }
    }

    void printTable()
    {
        for (PCB pcb : terminated)
        {
            cout << pcb.name << "," << (pcb.completion_time / 1000) << "," << (pcb.turn_around_time / 1000) << "," << (pcb.waiting_time / 1000) << endl;
            // cout << setw(10) << pcb.PID << setw(10) << pcb.name << setw(10) << pcb.arrival_time / 1000 << setw(10) << pcb.cpu_burst1 / 1000 << setw(10) << pcb.io_burst / 1000 << setw(10) << pcb.cpu_burst2 / 1000 << setw(10) << pcb.start_time / 1000 << setw(10) << pcb.response_time / 1000 << setw(10) << pcb.completion_time / 1000 << setw(10) << pcb.turn_around_time / 1000 << setw(10) << pcb.waiting_time / 1000 << endl;
        }
    }

    void exec(vector<string> process_input)
    {
        populateReadyQueue(process_input);
        // printQueue(ready_queue);
        performTasks();
        printTable();
    }
};

// int main(int argc, char **argv)
// {
//     RR rr;
//     rr.exec(process_input(argv[1]));
// }