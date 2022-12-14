#include <iostream>
#include <bits/stdc++.h>
#include "FCFS.cpp"
#include "Priority_Preempt.cpp"
#include "Priority_non_preempt.cpp"
#include "MLQ_new.cpp"

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

    FCFS *fcfs_scheduler = new FCFS(input_from_main);
    fcfs_scheduler->execute();

    Priority_preemptive *pr_preemptive_scheduler = new Priority_preemptive(input_from_main);
    pr_preemptive_scheduler->execute();

    Priority_non_preemptive *pr_non_preempt_scheduler = new Priority_non_preemptive(input_from_main);
    pr_non_preempt_scheduler->execute();

    MLQ *mlq_scheduler = new MLQ(input_from_main);
    mlq_scheduler->execute();

    return 0;
}