#include "processor.h"
#include "linux_parser.h"



// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    float utilization = 0;

    long active_tickes = LinuxParser::ActiveJiffies();
    long idle_ticks = LinuxParser::IdleJiffies();
    long diff_active_tickes = active_tickes - prev_active_tickes_;
    long diff_idle_ticks = idle_ticks - prev_idle_ticks_;

    utilization = static_cast<float>(diff_active_tickes) / (diff_active_tickes + diff_idle_ticks);

    prev_active_tickes_ = active_tickes;
    prev_idle_ticks_ = idle_ticks;

    return utilization;
}