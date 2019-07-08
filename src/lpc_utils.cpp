#include "lpc_utils.h"

int get_length(char str[])
{
    int count = 0;
    char *ptr = str;
    while ((ptr = strchr(ptr, '|')) != NULL)
    {
        count++;
        ptr++;
    }
    // printf("Length %d\n", count);
    return count;
}

int get_input_length(std::string input)
{
    return input.length();
}

float hex2float(std::string str_hex)
{
    float x;
    std::stringstream ss(str_hex);
    ss >> std::hex >> x;
    return x;
}

char str2char(std::string input, char &output)
{
    strcpy(&output, input.c_str()); // or pass &s[0]
}

float hex2float_(char hex[])
{
    int num;
    float f;
    sscanf(hex, "%x", &num); // assuming you checked input
    f = *((float *)&num);
    // printf("the hexadecimal 0x%08x becomes %.3f as a float\n", num, f);
    return f;
}

int intMax(int x, int y)
{
    return x > y ? x : y;
}

int intMin(int x, int y)
{
    return x < y ? x : y;
}

std::string toHex(const std::string &s, bool upper_case)
{
    std::ostringstream ret;

    for (std::string::size_type i = 0; i < s.length(); ++i)
    {
        int z = s[i] & 0xff;
        ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << z;
    }

    return ret.str();
}