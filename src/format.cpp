#include <string>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    string time;
    long HH, MM, SS;
    HH = seconds / 3600;                     // get hours
    MM = (seconds - HH * 3600) / 60;         // get minutes
    SS = seconds - MM * 60 - HH * 3600;      // get seconds;

    std::ostringstream elapsedTime;
    elapsedTime << std::setw(2) << std::setfill('0') << HH
                       << ":" << std::setw(2) << std::setfill('0') << MM
                       << ":" << std::setw(2) << std::setfill('0') << SS;
    return elapsedTime.str();
}