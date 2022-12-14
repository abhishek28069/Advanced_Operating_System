#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

int clk_pr_preempt = 0;

// PCB
class Process_Pr_preempt
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
    Process_Pr_preempt()
    {
        name = "idle";
        is_burst2 = false;
    }
    Process_Pr_preempt(int _PID, string _name, bool _cpu_bound, int _priority, int _arrival_time, int _burst_time, int _io_burst, int _burst_time2)
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

bool operator<(const Process_Pr_preempt &p1, const Process_Pr_preempt &p2)
{
    return p1.priority == p2.priority ? p1.PID > p2.PID : p1.priority > p2.priority;
}

class PriorityCompare
{
public:
    bool operator()(const Process_Pr_preempt &p1, const Process_Pr_preempt &p2)
    {
        return p1.priority == p2.priority ? p1.PID > p2.PID : p1.priority < p2.priority;
    }
};

class ArrivalCompare
{
public:
    bool operator()(const Process_Pr_preempt &p1, const Process_Pr_preempt &p2)
    {
        return p1.arrival_time_copy == p2.arrival_time_copy ? p1.PID > p2.PID : p1.arrival_time_copy > p2.arrival_time_copy;
    }
};

bool IdComp(const Process_Pr_preempt &p1, const Process_Pr_preempt &p2)
{
    return p1.PID < p2.PID;
}

void set_waiting(Process_Pr_preempt &P)
{
    P.state = "WAITING";
}
void set_running(Process_Pr_preempt &P)
{
    P.state = "RUNNING";
}
void set_ready(Process_Pr_preempt &P)
{
    P.state = "READY";
}
void set_terminated(Process_Pr_preempt &P)
{
    P.state = "TERMINATED";
}

class Priority_preemptive
{
public:
    priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, ArrivalCompare> new_queue;
    priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, PriorityCompare> ready_queue;
    queue<Process_Pr_preempt> terminate_queue;
    set<Process_Pr_preempt> started;
    Priority_preemptive() {}

    void print_ready_queue(priority_queue<Process_Pr_preempt> pq)
    {
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
        while (!pq.empty())
        {
            Process_Pr_preempt p = pq.top();
            pq.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << endl;
        }
    }
    void print_ready_queue(priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, PriorityCompare> pq)
    {
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
        while (!pq.empty())
        {
            Process_Pr_preempt p = pq.top();
            pq.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << endl;
        }
    }
    void print_ready_queue(priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, ArrivalCompare> pq)
    {
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
        while (!pq.empty())
        {
            Process_Pr_preempt p = pq.top();
            pq.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << endl;
        }
    }
    Process_Pr_preempt create_process(string line)
    {
        vector<string> tokens;
        stringstream check(line);
        string temp;
        while (getline(check, temp, ' '))
        {
            tokens.push_back(temp);
        }
        Process_Pr_preempt p;
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
    void Populate_new_queue(vector<string> lines, priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, ArrivalCompare> &pq)
    {
        for (auto line : lines)
        {
            pq.push(create_process(line));
        }
    }
    void run_process()
    {
        // if (p.name == "idle")
        //     cout << "Clock: " << clk_pr_preempt << "     Waiting for a Process_Pr_preempt" << endl;
        // else
        //     cout << "Clock: " << clk_pr_preempt << "     Executing Process_Pr_preempt " << p.name << endl;
    }
    void run_process(Process_Pr_preempt &p)
    {
        if (p.is_burst2)
        {
            p.cpu_burst2_copy--;
        }
        else
        {
            p.cpu_burst1_copy--;
        }
        // if (p.name == "idle")
        //     cout << "Clock: " << clk_pr_preempt << "     Waiting for a Process_Pr_preempt" << endl;
        // else
        //     cout << "Clock: " << clk_pr_preempt << "     Executing Process_Pr_preempt " << p.name << endl;
    }
    void wakeup(Process_Pr_preempt &current, priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, ArrivalCompare> &new_queue)
    {
        set_ready(current);
        new_queue.push(current);
    }
    void yield(Process_Pr_preempt &current, priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, ArrivalCompare> &new_queue)
    {
        set_waiting(current);
        current.arrival_time_copy = clk_pr_preempt + current.io_burst;
        current.is_burst2 = true;
        wakeup(current, new_queue);
    }
    void terminate(Process_Pr_preempt &current, queue<Process_Pr_preempt> &terminate_queue)
    {
        set_terminated(current);
        // completion time
        current.completion_time = clk_pr_preempt;
        // turnaround time
        current.turn_around_time = current.completion_time - current.arrival_time;
        // record waiting time
        current.waiting_time = current.turn_around_time - current.cpu_burst1 - current.cpu_burst2;
        terminate_queue.push(current);
    }
    void track_new_and_ready(priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, ArrivalCompare> &new_queue, priority_queue<Process_Pr_preempt, vector<Process_Pr_preempt>, PriorityCompare> &ready_queue)
    {
        if (!new_queue.empty())
        {
            while (clk_pr_preempt >= new_queue.top().arrival_time_copy && !new_queue.empty())
            {
                ready_queue.push(new_queue.top());
                new_queue.pop();
            }
        }
        cout << "clock - " << clk_pr_preempt << endl;
        cout << " NEW " << endl;
        print_ready_queue(new_queue);
        cout << "\n";
        cout << " READY " << endl;
        print_ready_queue(ready_queue);
    }

    Priority_preemptive(vector<string> lines)
    {
        Populate_new_queue(lines, new_queue);
    }
    void schedule_processes()
    {
        while (!ready_queue.empty() || !new_queue.empty())
        {
            cout << "**********************************************************************************" << endl;
            // checking new to ready transistion
            track_new_and_ready(new_queue, ready_queue);

            // execute if there is any Process_Pr_preempt in ready queue
            if (!ready_queue.empty())
            {
                Process_Pr_preempt current = ready_queue.top();
                ready_queue.pop();
                // check arrival time and current time
                cout << " got " << current.PID << " in the ready queue top " << endl;
                if (current.is_burst2)
                {
                    cout << "second time" << endl;
                }
                else
                {
                    cout << "first time" << endl;
                }
                while (clk_pr_preempt < current.arrival_time_copy)
                {
                    // idle
                    clk_pr_preempt++;
                    run_process();
                }
                if (started.find(current) == started.end())
                {
                    // record start_time
                    current.start_time = clk_pr_preempt;
                    // response time
                    current.response_time = current.start_time - current.arrival_time;
                    // add into set
                    started.insert(current);
                }
                // increment clock to finish the Process_Pr_preempt
                set_running(current);
                int end_time = !current.is_burst2 ? clk_pr_preempt + current.cpu_burst1_copy : clk_pr_preempt + current.cpu_burst2_copy;
                cout << "REMAINING BURST _ " << current.cpu_burst1_copy << endl;
                cout << " end time for " << current.PID << " is " << end_time << endl;
                while (clk_pr_preempt < end_time)
                {
                    track_new_and_ready(new_queue, ready_queue);
                    Process_Pr_preempt possible_process = ready_queue.top();
                    if (possible_process.priority > current.priority)
                    {
                        cout << "found better Process_Pr_preempt, current pr - " << current.priority << " new pr - " << possible_process.priority << endl;
                        ready_queue.push(current);
                        goto label;
                    }
                    clk_pr_preempt++;
                    run_process(current);
                }
                // push into termination_queue or ready queue
                if (current.is_burst2 || current.io_burst == 0 || current.cpu_burst2 == 0)
                {
                    terminate(current, terminate_queue);
                    cout << "COMPLETED _ " << current.PID << endl;
                }
                else
                {
                    yield(current, new_queue);
                }
            }
            else
                clk_pr_preempt++;
        label:
            cout << "done" << endl;
            cout << "*****************************************************************************************" << endl;
        }
    }
    void print_schedule()
    {
        queue<Process_Pr_preempt> temp = terminate_queue;
        vector<Process_Pr_preempt> temp2;
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << setw(10) << "start" << setw(10) << "resp" << setw(10) << "compl" << setw(10) << "turn" << setw(10) << "wait";
        cout << "\n";
        while (!temp.empty())
        {
            auto p = temp.front();
            temp.pop();
            temp2.push_back(p);
        }
        sort(temp2.begin(), temp2.end(), IdComp);
        for (auto p : temp2)
        {
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << setw(10) << p.start_time << setw(10) << p.response_time << setw(10) << p.completion_time << setw(10) << p.turn_around_time << setw(10) << p.waiting_time << endl;
        }
    }
    void save_schedule()
    {
        queue<Process_Pr_preempt> temp = terminate_queue;
        vector<Process_Pr_preempt> temp2;
        ofstream fout;
        fout.open("Priority_Preemptive_Schedule.csv");
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
        sort(temp2.begin(), temp2.end(), IdComp);
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
        system("python3 .\\plot.py priority_preempt");
    }
};

// int main(int argc, char **argv)
// {
//     vector<string> input_from_main = process_input(argv[1]);
//     Priority_preemptive *pr_preemptive_scheduler = new Priority_preemptive(input_from_main);
//     pr_preemptive_scheduler->schedule_processes();
//     pr_preemptive_scheduler->print_schedule();
//     pr_preemptive_scheduler->save_schedule();
// }