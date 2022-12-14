import matplotlib.pyplot as plt
import csv
import sys

if sys.argv[1] == "fcfs":
    csv_filename = "./FCFS_Schedule.csv"
elif sys.argv[1] == "priority_preempt":
    csv_filename = "./Priority_Preemptive_Schedule.csv"
elif sys.argv[1] == "priority_non_preempt":
    csv_filename = "./Priority_Non_Preemptive_Schedule.csv"
dataList = []

with open(csv_filename) as f:
    reader = csv.DictReader(f)
    for row in reader:
        dataList.append(row)

completionTime = [float(i["compl"]) for i in dataList]

TAT = [float(i["turn"]) for i in dataList]

waitingTime = [float(i["wait"]) for i in dataList]

responseTime = [float(i["resp"]) for i in dataList]

process = [float(i["id"]) for i in dataList]

plt.scatter(process, completionTime, label="Completion Time")
plt.scatter(process, TAT, label="Turn Around Time")
plt.scatter(process, waitingTime, label="Waiting Time")
plt.scatter(process, responseTime, label="Response Time")

plt.plot(process, completionTime, '--')
plt.plot(process, TAT, '--')
plt.plot(process, waitingTime, '--')
plt.plot(process, responseTime, '--')

plt.title(sys.argv[1])
plt.xlabel('Process')
plt.ylabel('Time Taken')
plt.legend(['CT', 'TAT', 'WT', 'RT'])
plt.grid(True)
plt.savefig("graphs/"+sys.argv[1], dpi=1000)
