#include <string>

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
    time = std::to_string(HH) + ":" + std::to_string(MM) + ":" + std::to_string(SS); // format as HH:MM:SS
    return time;
}