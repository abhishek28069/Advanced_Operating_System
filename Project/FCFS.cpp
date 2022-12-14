#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

int clk_fcfs = 0;

// PCB
class Process_FCFS
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
    Process_FCFS()
    {
        name = "idle";
        is_burst2 = false;
    }
    Process_FCFS(int _PID, string _name, bool _cpu_bound, int _priority, int _arrival_time, int _burst_time, int _io_burst, int _burst_time2)
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

bool operator<(const Process_FCFS &p1, const Process_FCFS &p2)
{
    return p1.arrival_time_copy == p2.arrival_time_copy ? p1.PID > p2.PID : p1.arrival_time_copy > p2.arrival_time_copy;
}
bool IdComp3(const Process_FCFS &p1, const Process_FCFS &p2)
{
    return p1.PID < p2.PID;
}
void set_waiting(Process_FCFS &P)
{
    P.state = "WAITING";
}
void set_running(Process_FCFS &P)
{
    P.state = "RUNNING";
}
void set_ready(Process_FCFS &P)
{
    P.state = "READY";
}
void set_terminated(Process_FCFS &P)
{
    P.state = "TERMINATED";
}

class FCFS
{
public:
    priority_queue<Process_FCFS> ready_queue;
    queue<Process_FCFS> terminate_queue;
    FCFS() {}

    void print_ready_queue(priority_queue<Process_FCFS> pq)
    {
        while (!pq.empty())
        {
            Process_FCFS p = pq.top();
            pq.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << endl;
        }
    }
    Process_FCFS create_process(string line)
    {
        vector<string> tokens;
        stringstream check(line);
        string temp;
        while (getline(check, temp, ' '))
        {
            tokens.push_back(temp);
        }
        Process_FCFS p;
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
    void Populate_ready_queue(vector<string> lines, priority_queue<Process_FCFS> &pq)
    {
        for (auto line : lines)
        {
            pq.push(create_process(line));
        }
    }
    void run_process(Process_FCFS p = Process_FCFS())
    {
        if (p.name == "idle")
            cout << "Clock: " << clk_fcfs << "     Waiting for a Process_FCFS" << endl;
        else
            cout << "Clock: " << clk_fcfs << "     Executing Process_FCFS " << p.name << endl;
    }
    void wakeup(Process_FCFS &current, priority_queue<Process_FCFS> &ready_queue)
    {
        set_ready(current);
        ready_queue.push(current);
    }
    void yield(Process_FCFS &current, priority_queue<Process_FCFS> &ready_queue)
    {
        set_waiting(current);
        current.arrival_time_copy = clk_fcfs + current.io_burst;
        current.is_burst2 = true;
        cout << "*IO* for " << current.PID << " for " << current.arrival_time_copy << endl;
        wakeup(current, ready_queue);
    }
    void terminate(Process_FCFS &current, queue<Process_FCFS> &terminate_queue)
    {
        set_terminated(current);
        // completion time
        current.completion_time = clk_fcfs;
        // turnaround time
        current.turn_around_time = current.completion_time - current.arrival_time;
        // record waiting time
        current.waiting_time = current.turn_around_time - current.cpu_burst1 - current.cpu_burst2;
        terminate_queue.push(current);
    }

    FCFS(vector<string> lines)
    {
        Populate_ready_queue(lines, ready_queue);
    }
    void schedule_processes()
    {
        while (!ready_queue.empty())
        {
            Process_FCFS current = ready_queue.top();
            ready_queue.pop();
            // check arrival time and current time
            while (clk_fcfs < current.arrival_time_copy)
            {
                // idle
                clk_fcfs++;
                run_process();
            }
            if (!current.is_burst2)
            {
                // record start_time
                current.start_time = clk_fcfs;
                // response time
                current.response_time = current.start_time - current.arrival_time;
            }

            // increment clock to finish the Process_FCFS
            set_running(current);
            int end_time = !current.is_burst2 ? clk_fcfs + current.cpu_burst1 : clk_fcfs + current.cpu_burst2;
            while (clk_fcfs < end_time)
            {
                clk_fcfs++;
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
        queue<Process_FCFS> temp = terminate_queue;
        vector<Process_FCFS> temp2;
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << setw(10) << "start" << setw(10) << "resp" << setw(10) << "compl" << setw(10) << "turn" << setw(10) << "wait";
        cout << "\n";
        while (!temp.empty())
        {
            auto p = temp.front();
            temp.pop();
            temp2.push_back(p);
        }
        sort(temp2.begin(), temp2.end(), IdComp3);
        for (auto p : temp2)
        {
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << setw(10) << p.start_time << setw(10) << p.response_time << setw(10) << p.completion_time << setw(10) << p.turn_around_time << setw(10) << p.waiting_time << endl;
        }
    }
    void save_schedule()
    {
        queue<Process_FCFS> temp = terminate_queue;
        vector<Process_FCFS> temp2;
        ofstream fout;
        fout.open("FCFS_Schedule.csv");
        fout << "id"
             << ","
             << "name"
             << ","
             << "priority"
             << ","
             << "arrival"
             << ","
             << "burst1"
             << ","
             << "io"
             << ","
             << "burst2"
             << ","
             << "start"
             << ","
             << "resp"
             << ","
             << "compl"
             << ","
             << "turn"
             << ","
             << "wait";
        fout << "\n";
        while (!temp.empty())
        {
            auto p = temp.front();
            temp.pop();
            temp2.push_back(p);
        }
        sort(temp2.begin(), temp2.end(), IdComp3);
        for (auto p : temp2)
        {
            fout << p.PID << "," << p.name << "," << p.priority << "," << p.arrival_time << "," << p.cpu_burst1 << "," << p.io_burst << "," << p.cpu_burst2 << "," << p.start_time << "," << p.response_time << "," << p.completion_time << "," << p.turn_around_time << "," << p.waiting_time << endl;
        }
        fout.close();
    }
    void execute()
    {
        schedule_processes();
        print_schedule();
        save_schedule();
        system("python3 .\\plot.py fcfs");
    }
};
