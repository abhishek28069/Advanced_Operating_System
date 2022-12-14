#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

int clk = 0;

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
    int age;
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
        age = 0;
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
        age = 0;
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

// PCB
class Process
{
public:
    // populating from input
    int PID;
    string name;
    bool cpu_bound;
    bool is_burst2; // for deciding whether the first cpu burst is completed or not
    int priority;
    int arrival_time;
    int arrival_time_copy;
    int cpu_burst1;
    int io_burst;
    int cpu_burst2;
    // derived variables
    string state;
    int start_time;       // timestamp of first-time cpu allocation
    int completion_time;  // timestamp of termination
    int waiting_time;     // turnaround time - cpu_time
    int turn_around_time; // completion time - arrival time
    int response_time;    // start time - arrival time
    // constructor
    Process()
    {
        name = "idle";
        is_burst2 = false;
    }
    Process(int _PID, string _name, bool _cpu_bound, int _priority, int _arrival_time, int _burst_time, int _io_burst, int _burst_time2)
    {
        PID = _PID;
        name = _name;
        cpu_bound = _cpu_bound;
        state = "READY";
        is_burst2 = false;
        priority = _priority;
        arrival_time = _arrival_time;
        arrival_time_copy = arrival_time;
        cpu_burst1 = _burst_time;
        io_burst = _io_burst;
        cpu_burst2 = _burst_time2;
    }
};

bool operator<(const Process &p1, const Process &p2)
{
    return p1.arrival_time_copy == p2.arrival_time_copy ? p1.PID > p2.PID : p1.arrival_time_copy > p2.arrival_time_copy;
}

void set_waiting(Process &P)
{
    P.state = "WAITING";
}
void set_running(Process &P)
{
    P.state = "RUNNING";
}
void set_ready(Process &P)
{
    P.state = "READY";
}
void set_terminated(Process &P)
{
    P.state = "TERMINATED";
}

void print_ready_queue(priority_queue<Process> pq)
{
    while (!pq.empty())
    {
        Process p = pq.top();
        pq.pop();
        cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << endl;
    }
}

Process create_process(string line)
{
    vector<string> tokens;
    stringstream check(line);
    string temp;
    while (getline(check, temp, ' '))
    {
        tokens.push_back(temp);
    }
    Process p;
    p.PID = stoi(tokens[0]);
    p.name = tokens[1];
    p.cpu_bound = true;
    p.state = "READY";
    p.is_burst2 = false;
    p.priority = stoi(tokens[3]);
    p.arrival_time = stoi(tokens[4]);
    p.arrival_time_copy = stoi(tokens[4]);
    p.cpu_burst1 = stoi(tokens[5]);
    p.io_burst = stoi(tokens[6]);
    p.cpu_burst2 = stoi(tokens[7]);
    return p;
}

void Populate_ready_queue(vector<string> lines, priority_queue<Process> &pq)
{
    for (auto line : lines)
    {
        pq.push(create_process(line));
    }
}

void run_process(Process p = Process())
{
    if (p.name == "idle")
        cout << "Clock: " << clk << "     Waiting for a process" << endl;
    else
        cout << "Clock: " << clk << "     Executing process " << p.name << endl;
}

void wakeup(Process &current, priority_queue<Process> &ready_queue)
{
    set_ready(current);
    ready_queue.push(current);
}

void yield(Process &current, priority_queue<Process> &ready_queue)
{
    set_waiting(current);
    current.arrival_time_copy = clk + current.io_burst;
    current.is_burst2 = true;
    cout << "*IO* for " << current.PID << " for " << current.arrival_time_copy << endl;
    wakeup(current, ready_queue);
}

void terminate(Process &current, queue<Process> &terminate_queue)
{
    set_terminated(current);
    // completion time
    current.completion_time = clk;
    // turnaround time
    current.turn_around_time = current.completion_time - current.arrival_time;
    // record waiting time
    current.waiting_time = current.turn_around_time - current.cpu_burst1 - current.cpu_burst2;
    terminate_queue.push(current);
}

class FCFS
{
public:
    priority_queue<Process> ready_queue;
    queue<Process> terminate_queue;
    FCFS() {}
    FCFS(vector<string> input_lines)
    {
        Populate_ready_queue(input_lines, ready_queue);
    }
    void add_additional_processes(vector<PCB> new_procs)
    {
        for (auto new_proc : new_procs)
        {
            Process converted_proc;
            converted_proc.PID = new_proc.PID;
            converted_proc.name = new_proc.name;
            converted_proc.cpu_bound = true;
            converted_proc.state = "READY";
            converted_proc.is_burst2 = false;
            converted_proc.priority = new_proc.priority;
            converted_proc.arrival_time = new_proc.arrival_time;
            converted_proc.arrival_time_copy = new_proc.arrival_time;
            converted_proc.cpu_burst1 = new_proc.cpu_burst1;
            converted_proc.io_burst = new_proc.io_burst;
            converted_proc.cpu_burst2 = new_proc.cpu_burst2;
            ready_queue.push(converted_proc);
        }
    }
    void schedule_processes(int current_time, vector<PCB> new_procs)
    {
        clk = current_time;
        add_additional_processes(new_procs);
        while (!ready_queue.empty())
        {
            Process current = ready_queue.top();
            ready_queue.pop();
            // check arrival time and current time
            while (clk < current.arrival_time_copy)
            {
                // idle
                clk++;
                run_process();
            }
            if (!current.is_burst2)
            {
                // record start_time
                current.start_time = clk;
                // response time
                current.response_time = current.start_time - current.arrival_time;
            }

            // increment clock to finish the process
            set_running(current);
            int end_time = !current.is_burst2 ? clk + current.cpu_burst1 : clk + current.cpu_burst2;
            while (clk < end_time)
            {
                clk++;
                run_process(current);
            }

            // push into termination_queue or ready queue
            if (current.is_burst2 || current.io_burst == 0 || current.cpu_burst2 == 0)
            {
                terminate(current, terminate_queue);
            }
            else
            {
                yield(current, ready_queue);
            }
        }
    }
    void print_schedule()
    {
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << setw(10) << "start" << setw(10) << "resp" << setw(10) << "compl" << setw(10) << "turn" << setw(10) << "wait";
        cout << "\n";
        while (!terminate_queue.empty())
        {
            auto p = terminate_queue.front();
            terminate_queue.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.arrival_time << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << setw(10) << p.start_time << setw(10) << p.response_time << setw(10) << p.completion_time << setw(10) << p.turn_around_time << setw(10) << p.waiting_time << endl;
        }
    }
};

// int main(int argc, char **argv)
// {

//     FCFS *fcfs_scheduler = new FCFS(input_lines);
//     fcfs_scheduler->schedule_processes(current_time,demoted_procs);
//     fcfs_scheduler->print_schedule();

//     return 0;
// }