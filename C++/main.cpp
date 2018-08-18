/* 
 * File:   main.cpp
 * Author: cuidx
 *
 * Created on 2018年1月27日, 上午9:19
 * 
 * g++    -c -g -MMD -MP -MF "build/Debug/GNU-Linux-x86/main.o.d" -o build/Debug/GNU-Linux-x86/main.o main.cpp
 * g++     -o dist/Debug/GNU-Linux-x86/reversestring build/Debug/GNU-Linux-x86/main.o 
 */

#include <cstdlib>
#include <string.h>
#include <stdio.h>

using namespace std;

// judge whether a string is reversable
bool isReversable(char *str) {
    if (NULL == str || strlen(str) == 0) {
        return false;
    }
    
    int pf = 0;
    int pe = strlen(str) - 1;
    
    while (pf < pe) {
        if (str[pf] != str[pe]) {
            return false;
        }
        pf++;
        pe--;
    }
    
    return true;
}

//reverse a string
char* reverse(char* str) {
    if (NULL == str || strlen(str) == 0) {
        return str;
    }
    int pf = 0;
    int pe = strlen(str) - 1;
    while (pf < pe) {
        int tmp = str[pf];
        str[pf] = str[pe];
        str[pe] = tmp;
        pf++;
        pe--;
    }
    return str;
}

//reverse word seperated by space
char* reverse_word(char* str) {
    //1. reverse the entire string
    reverse(str);
    
    //2.find word ended by space, reverse that word
    char* pword = str;
    int pos = 0;
    while (str[pos] != '\0') {
        if (str[pos] == ' ') {
            str[pos] = '\0';
            reverse(pword);
            str[pos] = ' ';
            pos++;
            pword = str + pos;
            continue;
        }
        pos++;
    }
    //reverse the last word
    reverse(pword);
    
    return str;
}

/*
 * 
 */
int main(int argc, char** argv) {

    char* pstr[] = { "abc", "aba", "ab", "aa",
                        "abcdcba","abcddcba"};
    
    int count = sizeof(pstr) / sizeof(pstr[0]);
    
    for (int pos = 0; pos < count; pos++) {
        printf("string %s is %s reversable\n", pstr[pos], isReversable(pstr[pos]) ? "" : "not");
    }
    
    char test[] = "this is a test string!    ";
    printf("%s\n",test);
    printf("%s\n",reverse_word(test));
    printf("%s\n",reverse(test));
   
    return 0;
}
