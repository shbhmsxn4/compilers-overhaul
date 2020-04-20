/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include "gen_utils.h"

void assert(bool condition, char *error_string)
{
    if (!condition)
    {
        printf("ERROR: Assertion failed - %s\n", error_string);
        exit(1);
    }
}

bool is_whitespace(char a)
{
    if (a == '\n')
        return true;
    if (a == '\r')
        return true;
    if (a == '\t')
        return true;
    if (a == ' ')
        return true;
    return false;
}

char *itoa(int value, char *str, int base)
{
    sprintf(str, "%d", value);
    return str;
}

float parse_float(char *float_str)
{
    float mantissa;
    int exponent;
    float f;

    // Check for existence of exponent
    int exponent_idx = -1;
    for (int i = 0; float_str[i] != '\0'; i++)
    {
        if (float_str[i] == 'e' || float_str[i] == 'E')
        {
            exponent_idx = i;
            break;
        }
    }
    if (exponent_idx != -1)
    {
        // Parse a float which has exponent

        // Parse exponent
        if (float_str[exponent_idx + 1] == '+')
        {
            exponent = atoi(float_str + exponent_idx + 2);
        }
        else if (float_str[exponent_idx + 1] == '-')
        {
            exponent = (-1) * atoi(float_str + exponent_idx + 2);
        }
        else
        {
            exponent = atoi(float_str + exponent_idx);
        }

        // Temporarily set the exponent to '\0'
        char exponent_char = float_str[exponent_idx];
        float_str[exponent_idx] = '\0';

        // Parse mantissa
        mantissa = strtof(float_str, NULL);

        // Reset exponent to 'e' or 'E'
        float_str[exponent_idx] = exponent_char;

        // Combine mantissa and exponent into float
        f = mantissa * pow(10, exponent);
    }
    else
    {
        // Parse a float which does not have exponent
        f = strtof(float_str, NULL);
    }
    return f;
}