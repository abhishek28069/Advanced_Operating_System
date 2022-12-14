#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
#include "RR_MLQ.cpp"
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

vector<string> process_input(char *file)
{
    vector<string> process_as_strings;
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
            process_as_strings.push_back(buffer);
        } while (c != EOF);
        fclose(fd);
    }
    return process_as_strings;
}

vector<vector<string>> processes_for_RR(char *file)
{
    vector<vector<string>> res(2);
    vector<string> all_processes = process_input(file);
    for (auto line : all_processes)
    {
        vector<string> tokens;
        stringstream check(line);
        string temp;
        while (getline(check, temp, ' '))
        {
            tokens.push_back(temp);
        }
        if (stoi(tokens[3]) >= 4 && stoi(tokens[3]) <= 7)
        {
            res[1].push_back(line);
        }
        else if (stoi(tokens[3]) >= 8 && stoi(tokens[3]) <= 10)
        {
            res[0].push_back(line);
        }
    }
    return res;
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

Process push_process(string line, vector<priority_queue<Process>> &pq)
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
    if (p.priority >= 0 && p.priority <= 3)
    {
        pq[2].push(p);
    }
    else if (p.priority >= 4 && p.priority <= 7)
    {
        pq[1].push(p);
    }
    else
    {
        pq[0].push(p);
    }
    return p;
}

void Populate_ready_queue(char *file, vector<priority_queue<Process>> &pq)
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
            push_process(buffer, pq);
        } while (c != EOF);
        fclose(fd);
    }
}

void run_process(Process p = Process())
{
    // if (p.name == "idle")
    //     cout << "Clock: " << clk << "     Waiting for a process" << endl;
    // else
    //     cout << "Clock: " << clk << "     Executing process " << p.name << endl;
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

void fcfs_execute(priority_queue<Process> ready_queue, queue<Process> terminate_queue)
{
    while (!ready_queue.empty())
    {
        Process current = ready_queue.top();
        ready_queue.pop();
        cout << current.arrival_time << endl;
        // check arrival time and current time
        while (clk < current.arrival_time_copy)
        {
            // idle
            clk++;
            run_process();
        }

        // record start_time
        current.start_time = clk;
        // response time
        current.response_time = current.start_time - current.arrival_time;

        // increment clock to finish the process
        set_running(current);
        int end_time = clk + current.cpu_burst1;
        while (clk < end_time)
        {
            clk++;
            run_process(current);
        }
        // push into termination_queue or ready queue
        terminate(current, terminate_queue);
    }
}

void print_schedule(queue<Process> terminate_queue)
{
    queue<Process> temp = terminate_queue;
    vector<Process> temp2;
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

int main(int argc, char **argv)
{
    vector<priority_queue<Process>> ready_queues(3); // 0(RR) for priority 10-8, 1(RR) for priority 7-4, 2(FCFS) for priority 3-0
    queue<Process> terminate_queue;
    Populate_ready_queue(argv[1], ready_queues);
    vector<vector<string>> RR_Processes = processes_for_RR(argv[1]);
    // print_ready_queue(ready_queues[2]);

    RR rr1, rr2;

    int q1_end_time = rr1.execute(RR_Processes[0], 0, false);

    cout << "\n\n\n\n";

    int q2_end_time = rr2.execute(RR_Processes[1], q1_end_time, true);
    clk = q2_end_time / 1000;

    fcfs_execute(ready_queues[2], terminate_queue);

    return 0;
}