/* Encapsule system API to get current time.*/

#ifndef MY_TIME_H
#define MY_TIME_H

#include <iostream>
#include <time.h>

using namespace std;

class my_time
{
public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    
    my_time()
    {
        getTime();
    };

    void getTime()
    {
        time_t t = time(NULL);
        tm *local_tm = localtime(&t);
        this -> year = local_tm -> tm_year + 1900;
        this -> month = local_tm -> tm_mon + 1;
        this -> day = local_tm -> tm_mday;
        this -> hour = local_tm -> tm_hour;
        this -> minute = local_tm -> tm_min;
        this -> second = local_tm -> tm_sec;
    }

    string str()
    {
        getTime();
        string res;
        res += to_string(this -> year);
        res += "-";

        if(this -> month < 10) res += "0";
        res += to_string(this -> month);
        res += "-";

        if(this -> day < 10) res += "0";
        res += to_string(this -> day);
        res += "-";
        
        if(this -> hour < 10) res += "0";
        res += to_string(this -> hour);
        res += ":";

        if(this -> minute < 10) res += "0";
        res += to_string(this -> minute);
        res +=":";

        if(this -> second < 10) res += "0";
        res += to_string(this -> second);

        return res;
    }
};



#endif