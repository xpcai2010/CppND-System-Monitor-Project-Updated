#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"


using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
    CpuUtilization();
    Command();
    Ram();
    User();
    UpTime();
}// constructor

// TODO: Return this process's ID
int Process::Pid() {
     return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    /* https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    */
//    long active_time_pid = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
//    long up_time_pid = LinuxParser::UpTime(pid_);
//    long diff_active_time_pid = active_time_pid - prev_active_time_pid_;
//    long diff_up_time_pid = up_time_pid - prev_up_time_pid_;

//    cpu_usage_ = static_cast<float>(diff_active_time_pid) / static_cast<float>(diff_up_time_pid);
//    prev_active_time_pid_ = active_time_pid;
//    prev_up_time_pid_ = up_time_pid;
//    return cpu_usage_; 

    cpu_usage_ = static_cast<float>(LinuxParser::ActiveJiffies(pid_)/ sysconf(_SC_CLK_TCK)) / LinuxParser::UpTime(pid_);    
    return cpu_usage_; 
}

// TODO: Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(pid_);
}

// TODO: Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(pid_);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return cpu_usage_ < a.cpu_usage_;
}