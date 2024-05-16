#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <sstream>

using namespace std;

struct Process
{
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int start_time;
    int finish_time;
    int waiting_time;
    int turnaround_time;
};

// Function to read processes from file
vector<Process> readProcesses(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    vector<Process> processes;
    int id, arrival, burst;
    while (file >> id >> arrival >> burst)
    {
        processes.push_back({id, arrival, burst, burst, -1, -1, 0, 0});
    }
    file.close();
    return processes;
}
// FCFS scheduling algorithm
string FCFS(vector<Process> &processes, int context_switch_time)
{
    int current_time = 0;
    int sum_tat = 0, sum_wt = 0;
    float cpu_utilization;
    int total_idle_time = 0;
    int prev = 0;
    string output;

    for (auto &process : processes)
    {
        if (process.arrival_time > current_time)
            current_time = process.arrival_time;

        process.start_time = current_time;
        process.finish_time = current_time + process.burst_time;
        process.waiting_time = process.start_time - process.arrival_time;
        process.turnaround_time = process.waiting_time + process.burst_time;
        sum_tat += process.turnaround_time;
        sum_wt += process.waiting_time;
        current_time = process.finish_time + context_switch_time;
        total_idle_time += (prev == 0) ? 0 : (process.start_time - prev);
        prev = current_time;
    }

    float length_cycle = current_time;
    cpu_utilization = (length_cycle - total_idle_time) / length_cycle;

    output += "\nFCFS:";
    output += "\n\nProcess No.\tAT\tCPU Burst Time\tFinish Time\tTAT\tWT\n";
    for (const auto &process : processes)
        output += to_string(process.id) + "\t\t" + to_string(process.arrival_time) + "\t   " + to_string(process.burst_time) + "\t\t" + to_string(process.finish_time) + "\t\t" + to_string(process.turnaround_time) + "\t" + to_string(process.waiting_time) + "\n";
    output += "\nAverage Turn Around time= " + to_string((float)sum_tat / processes.size());
    output += "\nAverage Waiting Time= " + to_string((float)sum_wt / processes.size());
    output += "\nThroughput= " + to_string(processes.size() / length_cycle);
    output += "\nCPU Utilization(Percentage)= " + to_string(cpu_utilization * 100) + "\n";

    return output;
}

// SRT scheduling algorithm
string SRT(vector<Process> &processes)
{
    int current_time = 0;
    int completed = 0;
    int sum_tat = 0, sum_wt = 0, total_idle_time = 0, prev = 0, length_cycle;
    float cpu_utilization;
    string output;

    while (completed != processes.size())
    {
        int min_index = -1;
        int shortest_remaining_time = INT_MAX;
        for (int i = 0; i < processes.size(); i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
            {
                if (processes[i].remaining_time < shortest_remaining_time)
                {
                    shortest_remaining_time = processes[i].remaining_time;
                    min_index = i;
                }
            }
        }

        if (min_index == -1)
        {
            current_time++;
        }
        else
        {
            if (processes[min_index].start_time == -1)
            {
                processes[min_index].start_time = current_time;
            }
            processes[min_index].remaining_time--;

            if (processes[min_index].remaining_time == 0)
            {
                completed++;
                processes[min_index].finish_time = current_time + 1;
                processes[min_index].turnaround_time = processes[min_index].finish_time - processes[min_index].arrival_time;
                processes[min_index].waiting_time = processes[min_index].turnaround_time - processes[min_index].burst_time;
                sum_tat += processes[min_index].turnaround_time;
                sum_wt += processes[min_index].waiting_time;
                total_idle_time += (prev == 0) ? 0 : (current_time - prev);
                prev = current_time + 1;
            }

            current_time++;
        }
    }

    length_cycle = current_time;
    cpu_utilization = (float)(length_cycle - total_idle_time) / length_cycle;

    output += "\nSRT:";
    output += "\n\nProcess No.\tAT\tCPU Burst Time\tFinish Time\tTAT\tWT\n";
    for (const auto &process : processes)
        output += to_string(process.id) + "\t\t" + to_string(process.arrival_time) + "\t   " + to_string(process.burst_time) + "\t\t" + to_string(process.finish_time) + "\t\t" + to_string(process.turnaround_time) + "\t" + to_string(process.waiting_time) + "\n";
    output += "\nAverage Turn Around time= " + to_string((float)sum_tat / processes.size());
    output += "\nAverage Waiting Time= " + to_string((float)sum_wt / processes.size());
    output += "\nThroughput= " + to_string(processes.size() / (float)length_cycle);
    output += "\nCPU Utilization(Percentage)= " + to_string(cpu_utilization * 100) + "\n";

    return output;
}

// RR scheduling algorithm
string RR(vector<Process> &processes, int quantum)
{
    int current_time = 0;
    int completed = 0;
    int sum_tat = 0, sum_wt = 0, total_idle_time = 0, prev = 0, length_cycle;
    float cpu_utilization;
    string output;

    while (completed != processes.size())
    {
        for (int i = 0; i < processes.size(); i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
            {
                if (processes[i].remaining_time <= quantum)
                {
                    current_time += processes[i].remaining_time;
                    processes[i].remaining_time = 0;
                    completed++;

                    processes[i].finish_time = current_time;
                    processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;

                    sum_tat += processes[i].turnaround_time;
                    sum_wt += processes[i].waiting_time;
                    total_idle_time += (prev == 0) ? 0 : (current_time - prev);

                    prev = current_time;
                }
                else
                {
                    current_time += quantum;
                    processes[i].remaining_time -= quantum;
                }
            }
        }
    }

    length_cycle = current_time;
    cpu_utilization = (float)(length_cycle - total_idle_time) / length_cycle;

    output += "\nRR:";
    output += "\n\nProcess No.\tAT\tCPU Burst Time\tFinish Time\tTAT\tWT\n";
    for (const auto &process : processes)
        output += to_string(process.id) + "\t\t" + to_string(process.arrival_time) + "\t   " + to_string(process.burst_time) + "\t\t" + to_string(process.finish_time) + "\t\t" + to_string(process.turnaround_time) + "\t" + to_string(process.waiting_time) + "\n";
    output += "\nAverage Turn Around time= " + to_string((float)sum_tat / processes.size());
    output += "\nAverage Waiting Time= " + to_string((float)sum_wt / processes.size());
    output += "\nThroughput= " + to_string(processes.size() / (float)length_cycle);
    output += "\nCPU Utilization(Percentage)= " + to_string(cpu_utilization * 100) + "\n";

    return output;
}

void printGanttChart(const vector<Process> &processes)
{
    cout << "Gantt Chart:" << endl;
    for (const auto &process : processes)
    {
        cout << "P" << process.id << "\t";
    }
    cout << endl;
    for (const auto &process : processes)
    {
        cout << process.start_time << "\t";
    }
    cout << processes.back().finish_time << endl;
}

double calculateCPUUtilization(const vector<Process> &processes, int total_time)
{
    int total_burst_time = 0;
    for (const auto &process : processes)
    {
        total_burst_time += process.burst_time;
    }
    return (double)total_burst_time / total_time;
}

void displayResults(const vector<Process> &processes, int total_time)
{
    cout << "Process\tFinish Time\tWaiting Time\tTurnaround Time" << endl;
    for (const auto &process : processes)
    {
        cout << "P" << process.id << "\t" << process.finish_time << "\t\t" << process.waiting_time << "\t\t" << process.turnaround_time << endl;
    }
    cout << "CPU Utilization: " << calculateCPUUtilization(processes, total_time) << endl;
}

int main()
{
    string filename = "C:/Users/raya co/Desktop/processes.txt";
    int context_switch_time = 2;
    int quantum = 3;

    vector<Process> processes = readProcesses(filename);
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrival_time < b.arrival_time; });

    // FCFS
    vector<Process> fcfs_processes = processes;
    string fcfs_output = FCFS(fcfs_processes, context_switch_time);

    fcfs_output = fcfs_output.substr(0, fcfs_output.find("Gantt Chart:"));
    cout << fcfs_output << endl;
    printGanttChart(fcfs_processes);

    // SRT
    vector<Process> srt_processes = processes;
    string srt_output = SRT(srt_processes);

    srt_output = srt_output.substr(0, srt_output.find("Gantt Chart:"));
    cout << srt_output << endl;
    printGanttChart(srt_processes);

    // RR
    vector<Process> rr_processes = processes;
    string rr_output = RR(rr_processes, quantum);

    rr_output = rr_output.substr(0, rr_output.find("Gantt Chart:"));
    cout << rr_output << endl;

    return 0;
}
