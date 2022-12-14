#include <iostream>
#include <fstream>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

int clk_pr_non_preempt = 0;

// PCB
class Process_Pr
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
    Process_Pr()
    {
        name = "idle";
        is_burst2 = false;
    }
    Process_Pr(int _PID, string _name, bool _cpu_bound, int _priority, int _arrival_time, int _burst_time, int _io_burst, int _burst_time2)
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

bool operator<(const Process_Pr &p1, const Process_Pr &p2)
{
    return p1.priority == p2.priority ? p1.PID > p2.PID : p1.priority > p2.priority;
}

class PriorityCompare2
{
public:
    bool operator()(const Process_Pr &p1, const Process_Pr &p2)
    {
        return p1.priority == p2.priority ? p1.PID > p2.PID : p1.priority < p2.priority;
    }
};

class ArrivalCompare2
{
public:
    bool operator()(const Process_Pr &p1, const Process_Pr &p2)
    {
        return p1.arrival_time_copy == p2.arrival_time_copy ? p1.PID > p2.PID : p1.arrival_time_copy > p2.arrival_time_copy;
    }
};

bool IdComp2(const Process_Pr &p1, const Process_Pr &p2)
{
    return p1.PID < p2.PID;
}

void set_waiting(Process_Pr &P)
{
    P.state = "WAITING";
}
void set_running(Process_Pr &P)
{
    P.state = "RUNNING";
}
void set_ready(Process_Pr &P)
{
    P.state = "READY";
}
void set_terminated(Process_Pr &P)
{
    P.state = "TERMINATED";
}

class Priority_non_preemptive
{

public:
    priority_queue<Process_Pr, vector<Process_Pr>, ArrivalCompare2> new_queue;
    priority_queue<Process_Pr, vector<Process_Pr>, PriorityCompare2> ready_queue;
    queue<Process_Pr> terminate_queue;
    Priority_non_preemptive() {}

    void print_ready_queue(priority_queue<Process_Pr> pq)
    {
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
        while (!pq.empty())
        {
            Process_Pr p = pq.top();
            pq.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << endl;
        }
    }
    void print_ready_queue(priority_queue<Process_Pr, vector<Process_Pr>, PriorityCompare2> pq)
    {
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
        while (!pq.empty())
        {
            Process_Pr p = pq.top();
            pq.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << endl;
        }
    }
    void print_ready_queue(priority_queue<Process_Pr, vector<Process_Pr>, ArrivalCompare2> pq)
    {
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << endl;
        while (!pq.empty())
        {
            Process_Pr p = pq.top();
            pq.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << endl;
        }
    }
    Process_Pr create_process(string line)
    {
        vector<string> tokens;
        stringstream check(line);
        string temp;
        while (getline(check, temp, ' '))
        {
            tokens.push_back(temp);
        }
        Process_Pr p;
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
    void Populate_new_queue(vector<string> lines, priority_queue<Process_Pr, vector<Process_Pr>, ArrivalCompare2> &pq)
    {
        for (auto line : lines)
        {
            pq.push(create_process(line));
        }
    }
    void run_process(Process_Pr p = Process_Pr())
    {
        if (p.name == "idle")
            cout << "Clock: " << clk_pr_non_preempt << "     Waiting for a Process_Pr" << endl;
        else
            cout << "Clock: " << clk_pr_non_preempt << "     Executing Process_Pr " << p.name << endl;
    }
    void wakeup(Process_Pr &current, priority_queue<Process_Pr, vector<Process_Pr>, ArrivalCompare2> &new_queue)
    {
        set_ready(current);
        new_queue.push(current);
    }
    void yield(Process_Pr &current, priority_queue<Process_Pr, vector<Process_Pr>, ArrivalCompare2> &new_queue)
    {
        set_waiting(current);
        current.arrival_time_copy = clk_pr_non_preempt + current.io_burst;
        current.is_burst2 = true;
        wakeup(current, new_queue);
    }
    void terminate(Process_Pr &current, queue<Process_Pr> &terminate_queue)
    {
        set_terminated(current);
        // completion time
        current.completion_time = clk_pr_non_preempt;
        // turnaround time
        current.turn_around_time = current.completion_time - current.arrival_time;
        // record waiting time
        current.waiting_time = current.turn_around_time - current.cpu_burst1 - current.cpu_burst2;
        terminate_queue.push(current);
    }

    Priority_non_preemptive(vector<string> lines)
    {
        Populate_new_queue(lines, new_queue);
        print_ready_queue(new_queue);
    }
    void schedule_processes()
    {
        while (!ready_queue.empty() || !new_queue.empty())
        {
            // checking new to ready transistion
            if (!new_queue.empty())
            {
                cout << "new queue is not empty" << endl;
                while (clk_pr_non_preempt >= new_queue.top().arrival_time_copy && !new_queue.empty())
                {
                    ready_queue.push(new_queue.top());
                    new_queue.pop();
                }
            }
            else
            {
                cout << "new queue is empty" << endl;
            }
            cout << "clock - " << clk_pr_non_preempt << endl;
            cout << " NEW " << endl;
            print_ready_queue(new_queue);
            cout << "\n";
            cout << " READY " << endl;
            print_ready_queue(ready_queue);
            // execute if there is any Process_Pr in ready queue
            if (!ready_queue.empty())
            {
                Process_Pr current = ready_queue.top();
                ready_queue.pop();
                // check arrival time and current time
                cout << " got " << current.PID << " in the ready queue top " << endl;
                while (clk_pr_non_preempt < current.arrival_time_copy)
                {
                    // idle
                    clk_pr_non_preempt++;
                    run_process();
                }
                if (!current.is_burst2)
                {
                    // record start_time
                    current.start_time = clk_pr_non_preempt;
                    // response time
                    current.response_time = current.start_time - current.arrival_time;
                }

                // increment clock to finish the Process_Pr
                set_running(current);
                int end_time = !current.is_burst2 ? clk_pr_non_preempt + current.cpu_burst1 : clk_pr_non_preempt + current.cpu_burst2;
                while (clk_pr_non_preempt < end_time)
                {
                    clk_pr_non_preempt++;
                    run_process(current);
                }
                // push into termination_queue or ready queue
                if (current.is_burst2 || current.io_burst == 0 || current.cpu_burst2 == 0)
                {
                    terminate(current, terminate_queue);
                }
                else
                {
                    yield(current, new_queue);
                }
            }
            else
                clk_pr_non_preempt++;
        }
    }
    void print_schedule()
    {
        queue<Process_Pr> temp = terminate_queue;
        vector<Process_Pr> temp2;
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << setw(10) << "start" << setw(10) << "resp" << setw(10) << "compl" << setw(10) << "turn" << setw(10) << "wait";
        cout << "\n";
        while (!temp.empty())
        {
            auto p = temp.front();
            temp.pop();
            temp2.push_back(p);
        }
        sort(temp2.begin(), temp2.end(), IdComp2);
        for (auto p : temp2)
        {
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << setw(10) << p.start_time << setw(10) << p.response_time << setw(10) << p.completion_time << setw(10) << p.turn_around_time << setw(10) << p.waiting_time << endl;
        }
    }
    void save_schedule()
    {
        queue<Process_Pr> temp = terminate_queue;
        vector<Process_Pr> temp2;
        ofstream fout;
        fout.open("Priority_Non_Preemptive_Schedule.csv");
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
        sort(temp2.begin(), temp2.end(), IdComp2);
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
        system("python3 .\\plot.py priority_non_preempt");
    }
};

// int main(int argc, char **argv)
// {
//     vector<string> input_from_main = process_input(argv[1]);
//     Priority_non_preemptive *pr_scheduler = new Priority_non_preemptive(input_from_main);
//     pr_scheduler->schedule_processes();
//     pr_scheduler->print_schedule();
//     pr_scheduler->save_schedule();
// }