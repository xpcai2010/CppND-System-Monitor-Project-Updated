#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    vector<int> pids = LinuxParser::Pids();
    
    set<int> extant_pids;
    for (Process& process : processes_)
        extant_pids.insert(process.Pid());
    
    for (int pid : pids){
        if (extant_pids.find(pid) == extant_pids.end())
            processes_.emplace_back(Process(pid)); // insert only new pid
    }

    for (Process& process : processes_)
        process.CpuUtilization();

    auto comp = [](Process& a, Process& b){return b < a;}; // using implemented bool operator<(Process const& a) const; 
    std::sort(processes_.begin(), processes_.end(), comp); // sort processes_ by cpu utilization
    return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {
    return LinuxParser::Kernel();
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

// TODO: Return the operating system name
std::string System::OperatingSystem() {
    return LinuxParser::OperatingSystem();
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() {
    return LinuxParser::UpTime();
}