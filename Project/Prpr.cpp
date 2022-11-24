#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

int clk = 0;

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
    int cpu_burst1_copy;
    int io_burst;
    int cpu_burst2;
    int cpu_burst2_copy;
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
        cpu_burst1_copy = _burst_time;
        io_burst = _io_burst;
        cpu_burst2 = _burst_time2;
        cpu_burst2_copy = _burst_time2;
    }
};

bool operator<(const Process &p1, const Process &p2)
{
    return p1.priority == p2.priority ? p1.PID > p2.PID : p1.priority > p2.priority;
}

class PriorityCompare
{
public:
    bool operator()(const Process &p1, const Process &p2)
    {
        return p1.priority == p2.priority ? p1.PID > p2.PID : p1.priority < p2.priority;
    }
};

class ArrivalCompare
{
public:
    bool operator()(const Process &p1, const Process &p2)
    {
        return p1.arrival_time_copy == p2.arrival_time_copy ? p1.PID > p2.PID : p1.arrival_time_copy > p2.arrival_time_copy;
    }
};

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
    cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
    while (!pq.empty())
    {
        Process p = pq.top();
        pq.pop();
        cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << endl;
    }
}

void print_ready_queue(priority_queue<Process, vector<Process>, PriorityCompare> pq)
{
    cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
    while (!pq.empty())
    {
        Process p = pq.top();
        pq.pop();
        cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << endl;
    }
}

void print_ready_queue(priority_queue<Process, vector<Process>, ArrivalCompare> pq)
{
    cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
    while (!pq.empty())
    {
        Process p = pq.top();
        pq.pop();
        cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << endl;
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
    p.cpu_burst1_copy = stoi(tokens[5]);
    p.io_burst = stoi(tokens[6]);
    p.cpu_burst2 = stoi(tokens[7]);
    p.cpu_burst2_copy = stoi(tokens[7]);
    return p;
}

void Populate_new_queue(char *file, priority_queue<Process, vector<Process>, ArrivalCompare> &pq)
{
    FILE *fd = fopen(file, "r");
    int size = 1024, pos, c;
    char *buffer = (char *)malloc(size);
    if (fd)
    {
        do
        { // read all lines in file
            pos = 0;
            do
            { // read one line
                c = fgetc(fd);
                if (c != EOF)
                    buffer[pos++] = (char)c;
                if (pos >= size - 1)
                { // increase buffer length - leave room for 0
                    size *= 2;
                    buffer = (char *)realloc(buffer, size);
                }
            } while (c != EOF && c != '\n');
            buffer[pos] = 0;
            // line is now in buffer
            pq.push(create_process(buffer));
        } while (c != EOF);
        fclose(fd);
    }
}

void run_process()
{
    // if (p.name == "idle")
    //     cout << "Clock: " << clk << "     Waiting for a process" << endl;
    // else
    //     cout << "Clock: " << clk << "     Executing process " << p.name << endl;
}
void run_process(Process &p)
{
    p.cpu_burst1_copy--;
    // if (p.name == "idle")
    //     cout << "Clock: " << clk << "     Waiting for a process" << endl;
    // else
    //     cout << "Clock: " << clk << "     Executing process " << p.name << endl;
}

void wakeup(Process &current, priority_queue<Process, vector<Process>, ArrivalCompare> &new_queue)
{
    set_ready(current);
    new_queue.push(current);
}

void yield(Process &current, priority_queue<Process, vector<Process>, ArrivalCompare> &new_queue)
{
    set_waiting(current);
    current.arrival_time_copy = clk + current.io_burst;
    current.is_burst2 = true;
    wakeup(current, new_queue);
}

void terminate(Process &current, queue<Process> &terminate_queue)
{
    set_terminated(current);
    // completion time
    current.completion_time = clk;
    // turnaround time
    current.turn_around_time = current.completion_time - current.arrival_time;
    // record waiting time
    current.waiting_time = current.turn_around_time - current.cpu_burst1;
    terminate_queue.push(current);
}

void track_new_and_ready(priority_queue<Process, vector<Process>, ArrivalCompare> &new_queue, priority_queue<Process, vector<Process>, PriorityCompare> &ready_queue)
{
    if (!new_queue.empty())
    {
        while (clk >= new_queue.top().arrival_time_copy && !new_queue.empty())
        {
            ready_queue.push(new_queue.top());
            new_queue.pop();
        }
    }
    cout << "clock - " << clk << endl;
    cout << " NEW " << endl;
    print_ready_queue(new_queue);
    cout << "\n";
    cout << " READY " << endl;
    print_ready_queue(ready_queue);
}

int main(int argc, char **argv)
{
    priority_queue<Process, vector<Process>, ArrivalCompare> new_queue;
    priority_queue<Process, vector<Process>, PriorityCompare> ready_queue;
    queue<Process> terminate_queue;
    Populate_new_queue(argv[1], new_queue);
    // print_ready_queue(new_queue);
    while (!ready_queue.empty() || !new_queue.empty())
    {
        cout << "**********************************************************************************" << endl;
        // checking new to ready transistion
        track_new_and_ready(new_queue, ready_queue);

        // execute if there is any process in ready queue
        if (!ready_queue.empty())
        {
            Process current = ready_queue.top();
            ready_queue.pop();
            // check arrival time and current time
            cout << " got " << current.PID << " in the ready queue top " << endl;
            while (clk < current.arrival_time)
            {
                // idle
                clk++;
                run_process();
            }
            // record start_time
            cout << " start time for " << current.PID << " is " << clk << endl;
            current.start_time = clk;
            // response time
            current.response_time = current.start_time - current.arrival_time;
            // increment clock to finish the process
            set_running(current);
            int end_time = clk + current.cpu_burst1_copy;
            cout << "REMAINING BURST _ " << current.cpu_burst1_copy << endl;
            cout << " end time for " << current.PID << " is " << end_time << endl;
            while (clk < end_time)
            {
                track_new_and_ready(new_queue, ready_queue);
                Process possible_process = ready_queue.top();
                if (possible_process.priority > current.priority)
                {
                    cout << "found better process, current pr - " << current.priority << " new pr - " << possible_process.priority << endl;
                    ready_queue.push(current);
                    goto label;
                }
                clk++;
                run_process(current);
            }
            // push into termination_queue or ready queue
            terminate(current, terminate_queue);
            cout << "COMPLETED _ " << current.PID << endl;
        }
        else
            clk++;
    label:
        cout << "done" << endl;
        cout << "*****************************************************************************************" << endl;
    }
    cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << setw(10) << "start" << setw(10) << "resp" << setw(10) << "compl" << setw(10) << "turn" << setw(10) << "wait";
    cout << "\n";
    while (!terminate_queue.empty())
    {
        auto p = terminate_queue.front();
        terminate_queue.pop();
        cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << setw(10) << p.start_time << setw(10) << p.response_time << setw(10) << p.completion_time << setw(10) << p.turn_around_time << setw(10) << p.waiting_time << endl;
    }
    return 0;
}