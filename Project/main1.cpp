#include <iostream>
#include <bits/stdc++.h>

// #include "FCFS.cpp"
#include "RR.cpp"

using namespace std;

// class PCB
// {
//     // populating from input
//     int PID;
//     string name;
//     bool cpu_bound;
//     int priority;
//     int arrival_time;
//     vector<pair<string, int>> burst_times; // example [{"c",2},{"i",3},{"c",4}....]
//     // derived variables
//     string state;
//     int start_time;                            // timestamp of first-time cpu allocation
//     int completion_time;                       // timestamp of termination
//     int waiting_time;                          // turnaround time - cpu_time
//     int turn_around_time;                      // completion time - arrival time
//     int response_time;                         // start time - arrival time
//     vector<pair<string, int>> remaining_times; // tracking the remainging burst_times
//     // constructor
//     Process(int _PID, string _name, bool _cpu_bound, int _priority, int _arrival_time, vector<pair<string, int>> _burst_times)
//     {
//         PID = _PID;
//         name = _name;
//         cpu_bound = _cpu_bound;
//         priority = _priority;
//         arrival_time = _arrival_time;
//         burst_times = _burst_times;
//         remaining_times = _burst_times;
//     }
//     void set_waiting()
//     {
//         P.state = "WAITING";
//     }
//     void set_running()
//     {
//         P.state = "RUNNING";
//     }
//     void set_ready()
//     {
//         P.state = "READY";
//     }
//     void set_terminated()
//     {
//         P.state = "TERMINATED";
//     }

// };

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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        return 0;
    }
    vector<string> processes_list = process_input(argv[1]);
    // cout << processes_list.size() << endl;
    // for(string process : processes_list){
    //     cout << process << endl;
    // }
    RR rr;
    rr.exec(processes_list);
    return 0;
}

// Process Id	Arrival time	Burst time
// P1	0	4
// P2	1	5
// P3	2	2
// P4	3	1
// P5	4	6
// P6	6	3

// 0,A,I,8,0,12,5,5
// 1,B,7,1,7,5,2,5
// 2,C,7,2,2,7,3,4
// 3,D,5,3,9,4,6,2
// 4,E,1,4,1,2,1,1
// 5,F,2,5,3,2,6,1
// 6,G,3,6,6,4,1,1
// 7,H,4,7,5,1,3,1