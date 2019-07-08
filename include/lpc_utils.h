#ifndef LPC_UTILS_H
#define LPC_UTILS_H
#endif

#include <stdio.h>
#include <string>
#include <sstream>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>

int get_length(char str[]);
int get_input_length(std::string input);
float hex2float(std::string str_hex);
char str2char(std::string input, char &output);
float hex2float_(char hex[]);
int intMax(int x, int y);
int intMin(int x, int y);
std::string toHex(const std::string &s, bool upper_case);