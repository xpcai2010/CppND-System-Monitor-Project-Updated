#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath); // parsing "/etc/os-release"
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename); // parsing "/proc/version"
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key;
  double value, mem_total, mem_free;
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename); // parsing "/proc/meminfo"
  if(filestream.is_open()){
    while (std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "MemTotal") mem_total = value;              //total memory
        else if (key == "MemFree") mem_free = value;           //free memory
      }
    }
  }
  return (mem_total - mem_free)/mem_total; // utilization
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long active_time = 0, idle_time = 0, useless_1 = 0, useless_2 = 0;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename); // parsing "/proc/uptime"
  if (stream.is_open()){
    std::getline(stream, line);
    std::replace(line.begin(), line.end(), '.', ' ');
    std::istringstream linestream(line);
    linestream >> active_time >> useless_1>> idle_time >> useless_2;
  }
  return (active_time + idle_time)/10;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long num_jiffies = 0;
  vector<string> cpu_time = LinuxParser::CpuUtilization();
  for (int i = 0; i < 8; i++)
    num_jiffies += std::stol(cpu_time[i]);
  return num_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, cpu;
  std::ifstream stream(kProcDirectory +to_string(pid) + kStatFilename); // parsing "/proc/[PID]/stat"
  long utime = 0, stime = 0, cutime = 0, cstime = 0; // definition  see below 
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; linestream >> cpu; i++){
      /* https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
         /proc/[PID]/stat
         #14 utime - CPU time spent in user code, measured in clock ticks
         #15 stime - CPU time spent in kernel code, measured in clock ticks
         #16 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
         #17 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
       */
      if (i == 14){
        utime = std::stol(cpu);
        continue;
      }
      if (i == 15){
        stime = std::stol(cpu);
        continue;
      }
      if (i == 16){
        cutime = std::stol(cpu);
        continue;
      }
      if (i == 17){
        cstime = std::stol(cpu);
        continue;
      }      
    }
  }
  return utime + stime + cutime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_time = LinuxParser::CpuUtilization();
  long active_jiffies = stol(cpu_time[CPUStates::kUser_]) + stol(cpu_time[CPUStates::kNice_]) + 
                        stol(cpu_time[CPUStates::kSystem_]) + stol(cpu_time[CPUStates::kIRQ_]) + 
                        stol(cpu_time[CPUStates::kSoftIRQ_]) + stol(cpu_time[CPUStates::kSteal_]) ; 
  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_time = LinuxParser::CpuUtilization();
  long idle_jiffies = stol(cpu_time[CPUStates::kIdle_]) + stol(cpu_time[CPUStates::kIOwait_]); 
  return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_utilizations;
  string line, cpu, cpu_time;
  std::ifstream filestream(kProcDirectory + kStatFilename); // parsing "/proc/stat"
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line); // get the first line of "/proc/stat"
    while (linestream >> cpu){ // parsing the first line of "/proc/stat"
      if (cpu != "cpu"){
        cpu_utilizations.emplace_back(cpu);
      }
    }
  }

  return cpu_utilizations;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, value, line;
  std::ifstream filestream(kProcDirectory + kStatFilename); //parsing "/proc/stat"
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "processes")
          return std::stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, value, line;
  std::ifstream filestream(kProcDirectory + kStatFilename); //parsing "/proc/stat"
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "procs_running")
          return std::stoi(value);
      }
    }
  }
  return 0;  
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
   string line;
   std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename); //parsing "/proc/pid/cmdline"
   if (filestream.is_open()){
     std::getline(filestream, line);
   }
   return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string key, line;
  int value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename); //parsing "/proc/pid/status"
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "VmSize:")
          return to_string(value/1000); // from KB to MB
      }
    }
  }
  return string("0");
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key, line, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename); //parsing "/proc/pid/status"
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "Uid:")
          return value; 
      }
    }
  }
  return string("0");  
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string user, x, uid, line;
  std::ifstream filestream(kPasswordPath); //parsing "/etc/passwd"
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid){
        if (uid == LinuxParser::Uid(pid)){
          return user;
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string value, line;
  long starttime = 0, uptime = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename); //parsing "/proc/pid/stat"
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; linestream >> value; i++){
      /* https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
         /proc/[PID]/stat
         #22 starttime - Time when the process started, measured in clock ticks
       */
      if (i == 22){
        starttime = std::stol(value);
        break;
      }
    }
  }
  uptime = LinuxParser::UpTime() - starttime/sysconf(_SC_CLK_TCK); //uptime - (starttime / Hertz) 
  return uptime;
}