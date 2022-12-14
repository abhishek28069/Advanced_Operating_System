#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
#include "RR_MLQ.cpp"
using namespace std;

int clk_fcfs_mlq = 0;

vector<vector<string>> process_seperation(vector<string> input_from_main)
{
    vector<vector<string>> res(3);
    for (auto line : input_from_main)
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
        else
        {
            res[2].push_back(line);
        }
    }
    return res;
}

// PCB
class Process_FCFS_MLQ
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
    Process_FCFS_MLQ()
    {
        name = "idle";
        is_burst2 = false;
    }
    Process_FCFS_MLQ(int _PID, string _name, bool _cpu_bound, int _priority, int _arrival_time, int _burst_time, int _io_burst, int _burst_time2)
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

bool operator<(const Process_FCFS_MLQ &p1, const Process_FCFS_MLQ &p2)
{
    return p1.arrival_time_copy == p2.arrival_time_copy ? p1.PID > p2.PID : p1.arrival_time_copy > p2.arrival_time_copy;
}
bool IdComp4(const Process_FCFS_MLQ &p1, const Process_FCFS_MLQ &p2)
{
    return p1.PID < p2.PID;
}
void set_waiting(Process_FCFS_MLQ &P)
{
    P.state = "WAITING";
}
void set_running(Process_FCFS_MLQ &P)
{
    P.state = "RUNNING";
}
void set_ready(Process_FCFS_MLQ &P)
{
    P.state = "READY";
}
void set_terminated(Process_FCFS_MLQ &P)
{
    P.state = "TERMINATED";
}

class FCFS_MLQ
{
public:
    priority_queue<Process_FCFS_MLQ> ready_queue;
    queue<Process_FCFS_MLQ> terminate_queue;
    FCFS_MLQ() {}

    void print_ready_queue(priority_queue<Process_FCFS_MLQ> pq)
    {
        while (!pq.empty())
        {
            Process_FCFS_MLQ p = pq.top();
            pq.pop();
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.arrival_time_copy << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << endl;
        }
    }
    Process_FCFS_MLQ create_process(string line)
    {
        vector<string> tokens;
        stringstream check(line);
        string temp;
        while (getline(check, temp, ' '))
        {
            tokens.push_back(temp);
        }
        Process_FCFS_MLQ p;
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
    void Populate_ready_queue(vector<string> lines, priority_queue<Process_FCFS_MLQ> &pq)
    {
        for (auto line : lines)
        {
            pq.push(create_process(line));
        }
    }
    void run_process(Process_FCFS_MLQ p = Process_FCFS_MLQ())
    {
        // if (p.name == "idle")
        //     cout << "Clock: " << clk_fcfs << "     Waiting for a Process_FCFS_MLQ" << endl;
        // else
        //     cout << "Clock: " << clk_fcfs << "     Executing Process_FCFS_MLQ " << p.name << endl;
    }

    void terminate(Process_FCFS_MLQ &current, queue<Process_FCFS_MLQ> &terminate_queue)
    {
        set_terminated(current);
        // completion time
        current.completion_time = clk_fcfs_mlq;
        // turnaround time
        current.turn_around_time = current.completion_time - current.arrival_time;
        // record waiting time
        current.waiting_time = current.turn_around_time - current.cpu_burst1 - current.cpu_burst2;
        terminate_queue.push(current);
    }

    FCFS_MLQ(int current_time, vector<string> lines)
    {
        Populate_ready_queue(lines, ready_queue);
        clk_fcfs_mlq = current_time;
    }
    void schedule_processes()
    {
        while (!ready_queue.empty())
        {
            Process_FCFS_MLQ current = ready_queue.top();
            ready_queue.pop();
            // check arrival time and current time
            while (clk_fcfs_mlq < current.arrival_time_copy)
            {
                // idle
                clk_fcfs_mlq++;
                run_process();
            }
            // record start_time
            current.start_time = clk_fcfs_mlq;
            // response time
            current.response_time = current.start_time - current.arrival_time;

            // increment clock to finish the Process_FCFS_MLQ
            set_running(current);
            int end_time = !current.is_burst2 ? clk_fcfs_mlq + current.cpu_burst1 : clk_fcfs_mlq + current.cpu_burst2;
            while (clk_fcfs_mlq < end_time)
            {
                clk_fcfs_mlq++;
                run_process(current);
            }
            terminate(current, terminate_queue);
        }
    }
    void print_schedule()
    {
        queue<Process_FCFS_MLQ> temp = terminate_queue;
        vector<Process_FCFS_MLQ> temp2;
        cout << setw(10) << "id" << setw(10) << "name" << setw(10) << "priority" << setw(10) << "arrival" << setw(10) << "burst1" << setw(10) << "io" << setw(10) << "burst2" << setw(10) << "start" << setw(10) << "resp" << setw(10) << "compl" << setw(10) << "turn" << setw(10) << "wait";
        cout << "\n";
        while (!temp.empty())
        {
            auto p = temp.front();
            temp.pop();
            temp2.push_back(p);
        }
        sort(temp2.begin(), temp2.end(), IdComp4);
        for (auto p : temp2)
        {
            cout << setw(10) << p.PID << setw(10) << p.name << setw(10) << p.priority << setw(10) << p.arrival_time << setw(10) << p.cpu_burst1 << setw(10) << p.io_burst << setw(10) << p.cpu_burst2 << setw(10) << p.start_time << setw(10) << p.response_time << setw(10) << p.completion_time << setw(10) << p.turn_around_time << setw(10) << p.waiting_time << endl;
        }
    }
    void save_schedule(bool is_append)
    {
        queue<Process_FCFS_MLQ> temp = terminate_queue;
        vector<Process_FCFS_MLQ> temp2;
        ofstream fout;
        if (is_append)
            fout.open("MLQ.csv", ios::in | ios::out | ios::app);
        else
        {
            fout.open("MLQ.csv");
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
        }
        while (!temp.empty())
        {
            auto p = temp.front();
            temp.pop();
            temp2.push_back(p);
        }
        sort(temp2.begin(), temp2.end(), IdComp4);
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
        save_schedule(true);
    }
};

class MLQ
{
public:
    vector<vector<string>> segregated_processes_strings;
    RR rr1, rr2;
    MLQ(vector<string> input_from_main)
    {
        segregated_processes_strings = process_seperation(input_from_main);
    }
    void execute()
    {
        int q1_end_time = rr1.execute(segregated_processes_strings[0], 0, false);
        int q2_end_time = rr2.execute(segregated_processes_strings[1], q1_end_time, true);
        FCFS_MLQ *fcfs3 = new FCFS_MLQ(q2_end_time / 1000, segregated_processes_strings[2]);
        fcfs3->execute();
    }
};

vector<string> process_input_file(char *file)
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

int main(int argc, char **argv)
{
    vector<string> input_from_main = process_input_file(argv[1]);

    MLQ *mlq_scheduler = new MLQ(input_from_main);
    mlq_scheduler->execute();

    return 0;
}