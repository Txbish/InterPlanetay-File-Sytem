#pragma once
#include<string>
#include<time.h>
#include<cstdlib>
#include<iostream>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

class BigInt
{
    int size;
    char* ptr;
    bool neg;
public:
    BigInt(int val=0);
    BigInt(int val, int ab);
    BigInt(const string& text);
    //BigInt(const char* text);
    BigInt(const BigInt& copy);
    BigInt generateRandomBigInt(int bits);
    BigInt binaryToDecimal(const BigInt& binaryNum);
    BigInt HexToDecimal(const string& Hex);
    BigInt Power(string exp);
    BigInt operator +(const BigInt& val) const;
    BigInt operator +(int val) const;
    BigInt operator -(const BigInt& val) const;
    BigInt operator -(int val) const;
    BigInt operator%(const BigInt& val) const;
    BigInt& operator*(const BigInt& val) const;
    BigInt operator+=(const BigInt& rhs);
    BigInt operator-=(const BigInt& rhs);
    BigInt operator*=(const BigInt& rhs);
    bool operator==(const BigInt& val) const;
    bool operator==(const char* val) const;
    bool operator!=(const BigInt& val) const;
    bool operator<(const BigInt& val) const;
    bool operator<=(const BigInt& val) const;
    bool operator>(const BigInt& val) const;
    bool operator>=(const BigInt& val) const;
    BigInt& operator++();
    BigInt operator++(int);
    BigInt& operator--();
    BigInt operator--(int);
    operator string();
    operator int();
    friend ostream& operator<<(ostream& output, const BigInt& val);
    friend istream& operator>>(istream& input, BigInt& val);
    string toString(const BigInt val);
    ~BigInt();
};
