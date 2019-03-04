#include "tables.h"
#include <string>
using namespace std;


string pitchNames[12] = {"C", "C#","D","Eb","E", "F", "F#","G","G#","A","Bb","B"};


string OutNames[12] = {"c", "cis", "d", "ees", "e","f","fis","g","gis","a","bes","b"};

string ratios[SIZE] = {"1/6","1/5","2/5","3/5","4/5","1/3","2/3","5/6"};

string types[SIZE] = {"16","16", "8","8.","4","8","4","!4 16"};

string tuplet_names[SIZE] = {"6/5","5/4","5/4","5/4","5/4","3/2","3/2", "6/5"};

int valid_time[13] = {0, 10, 12, 15, 20, 24, 30, 36, 40, 45, 48, 50, 60};

string rest_signs[SIZE2] = {"r16","r16", "r8","r8 r16","r4","r8","r4", "r4 r16"};

string rest_ratios[SIZE2] = {"1/6","1/5","2/5","3/5","4/5","1/3","2/3", "5/6"};

string pow_of_2[10] = {"4", "2","1", "1/2","1/4","1/8","1/16","1/32", "1/64", "1/128"};

string modifiers[40] = {"accent", "espressivo", "marcato", "portato", "staccatissimo", "staccato", "tenuto",
"prall", "prallup", "pralldown", "upprall", "downprall", "prallprall", "lineprall", "prallmordent", "mordent", "upmordent", "downmordent", "trill", "turn", "reverseturn",
"shortfermata", "fermata", "longfermata", "verylongfermata",
"upbow", "downbow", "flageolet", "open", "halfopen", "lheel", "rheel", "ltoe", "rtoe", "snappizzicato", "stopped", "segno", "coda", "varcoda" };


