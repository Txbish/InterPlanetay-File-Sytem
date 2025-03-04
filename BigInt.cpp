#pragma once
#include <iostream>
using namespace std;
#include "BigInt.h"
#include<sstream>

BigInt::BigInt(int val1)
{
    size = 1;
    int count = 0;
    int n = val1;
    bool f = 0;
    while (n != 0)
    {
        n /= 10;
        ++count;
        f = 1;
    }
    if (f == 0)
    {
        neg = false;
        ptr = NULL;
        ptr = new char[1];
        ptr[0] = '0';
        size = 1;
        ptr[1] = '\0';
        return;
    }
    if (val1 > 0)
    {
        ptr = NULL;
        ptr = new char[count];
        for (int i = 0; i < count; ++i)
        {
            ptr[i] = '\0';
        }
        size = count;
        int n1 = val1;
        count = count - 1;
        while (n1 != 0)
        {
            ptr[count] = '0' + n1 % 10;
            n1 /= 10;
            count--;
        }
        neg = false;
        ptr[size] = '\0';
    }
    if (val1 < 0)
    {
        int negi = val1;
        negi = -negi;
        ptr = NULL;
        ptr = new char[count + 1];
        for (int i = 0; i < count + 1; ++i)
        {
            ptr[i] = '\0';
        }
        size = count + 1;
        int n1 = negi;
        count = count;
        while (n1 != 0)
        {
            ptr[count] = '0' + n1 % 10;
            n1 /= 10;
            count--;
        }
        neg = true;
        ptr[0] = '-';
        ptr[size] = '\0';
    }
}

BigInt::BigInt(const string& text)
{
    int stext = 0;
    while (text[stext] != '\0')
    {
        stext++;
    }
    if (text[0] == '0'&&text.size() == 1)
    {
        neg = false;
        ptr = NULL;
        ptr = new char[1];
        ptr[0] = '0';
        size = 1;
        ptr[1] = '\0';
        return;
    }
    ptr = new char[stext+1];
    for (int i = 0; i < stext; i++)
    {
        ptr[i] = text[i];
    }
    ptr[stext] = '\0';
    if (ptr[0] == '-')
    {
        neg = true;
    }
    else
        neg = false;

    size = stext;
}

BigInt::BigInt(const BigInt& copy)
{
    ptr = new char[copy.size+1];

    for (int i = 0; i < copy.size; i++)
        ptr[i] = copy.ptr[i];
    ptr[copy.size] = '\0';
    size = copy.size;
    neg = copy.neg;
}
BigInt BigInt::operator+(const BigInt& val) const
{
    string negative;
    string negative1;
    BigInt* result = NULL;
    if (val.neg == true)
    {
        for (int i = 0; i < val.size; i++)
        {
            if (val.ptr[i] != '-')
            {
                negative = negative + val.ptr[i];
            }
        }
    }
    if (neg == true)
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != '-')
            {
                negative1 = negative1 + ptr[i];
            }
        }
    }
    if ((val.neg == true) && (neg == false))
    {
        result = new BigInt(negative);
        if (*result > *this)
        {
            BigInt reto = result - this;
            *result = result - this;
            BigInt ret(*result);
            delete[] result->ptr;
            result->ptr = NULL;
            result->size = result->size + 1;
            result->ptr = new char[result->size + 1];
            result->ptr[0] = '-';
            result->neg = true;
            for (int i = 1; i < result->size + 1; i++)
            {
                result->ptr[i] = ret.ptr[i - 1];
            }
            return *result;
        }
        if (*result < *this)
        {
            BigInt reto;
            reto = *this - *result;
            return reto;
        }
        if (*result == *this)
        {
            BigInt reto;
            return reto;
        }
    }
    if ((val.neg == false) && (neg == true))
    {
        result = new BigInt(negative1);
        if (val > *result)
        {
            *result = val - *result;
            BigInt ret(*result);
            delete[] result->ptr;
            result->ptr = NULL;
            result->size = result->size + 1;
            result->ptr = new char[result->size + 1];
            result->ptr[0] = '-';
            result->neg = true;
            for (int i = 1; i < result->size + 1; i++)
            {
                result->ptr[i] = ret.ptr[i - 1];
            }
            return *result;
        }
        if (val < *result)
        {
            BigInt reto;
            reto = *result - val;
            return reto;
        }
        if (*result == val)
        {
            BigInt reto;
            return reto;
        }
    }
    if ((val.neg == true) && (neg == true))
    {
        result = new BigInt(negative1);
        BigInt* result1 = new BigInt(negative);
        *result = *result + *result1;
        BigInt Reto(*result);
        delete[] result->ptr;
        result->ptr = NULL;
        result->ptr = new char[++result->size];
        result->ptr[0] = '-';
        for (int i = 1; i < result->size; i++)
        {
            result->ptr[i] = Reto.ptr[i - 1];
        }
        result->neg = true;
        delete[] result1->ptr;
        delete result1;
        return *result;
    }
    int size1 = 0;
    size1 = (size >= val.size) ? size : val.size;
    char* sum = new char[size1 + 2];
    sum[size1 + 1] = '\0';
    int carry = 0;
    for (int i = size - 1, j = val.size - 1, k = size1; k >= 0; k--, j--, i--)
    {
        int x, y, z = 0;
        if (i >= 0)
            x = ptr[i] - '0';
        else
            x = 0;
        if (j >= 0)
            y = val.ptr[j] - '0';
        else
            y = 0;
        z = x + y + carry;
        sum[k] = (z % 10) + '0';
        carry = z / 10;
    }
    if (carry > 0)
        sum[0] = carry + '0';

    if (sum[0] == '0')
    {
        for (int i = 0; i < size1 + 1; i++)
        {
            sum[i] = sum[i + 1];
        }
    }
    BigInt ret(sum);
    delete[] sum;
    sum = nullptr;
    return ret;
}
BigInt BigInt::operator+(int val) const
{
    BigInt* val1 = new BigInt(val);
    *val1 = *this + *val1;
    return *val1;

}
BigInt::operator string()
{
    ptr[size] = '\0';
    return ptr;
}
BigInt::BigInt(int val, int a)
{
    ptr = new char[val];
    size = val;
}
BigInt BigInt::operator-(const BigInt& val) const
{   
    if (*this == val)
    {
        BigInt ret(0);
        return ret;
    }
    string negative;
    string negative1;
    BigInt* result = NULL;
    if (val.neg == true)
    {
        for (int i = 0; i < val.size; i++)
        {
            if (val.ptr[i] != '-')
            {
                negative = negative + val.ptr[i];
            }
        }
    }
    if (neg == true)
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != '-')
            {
                negative1 = negative1 + ptr[i];
            }
        }
    }
    if ((val.neg == true) && (neg == false))
    {
        result = new BigInt(negative);
        *result = *result + *this;
        return *result;
    }
    if ((val.neg == false) && (neg == true))
    {
        result = new BigInt(negative1);
        *result = *result + val;

        BigInt ret(*result);
        delete[] result->ptr;
        result->ptr = NULL;
        result->size = result->size + 1;
        result->ptr = new char[result->size + 1];
        result->ptr[0] = '-';
        result->neg = true;
        for (int i = 1; i < result->size + 1; i++)
        {
            result->ptr[i] = ret.ptr[i - 1];
        }
        return *result;
    }

    if ((val.neg == true) && (neg == true))
    {
        if (val == *this)
        {
            BigInt aa;
            return aa;
        }
        result = new BigInt(negative1);
        BigInt* result1 = new BigInt(negative);
        *result = *result - *result1;
        BigInt ret(*result);
        delete[] result->ptr;
        result->ptr = NULL;
        result->size = result->size + 1;
        result->ptr = new char[result->size + 1];
        result->ptr[0] = '-';
        result->neg = true;
        for (int i = 1; i < result->size + 1; i++)
        {
            result->ptr[i] = ret.ptr[i - 1];
        }
        delete[] result1->ptr;
        result1->ptr = NULL;
        delete result1;
        return *result;
    }
    if (val > *this)
    {
        BigInt* result1 = new BigInt(val);
        BigInt sub(*this);
        sub = *result1 - sub;
        BigInt r(sub);
        delete[] sub.ptr;
        sub.ptr = NULL;
        sub.ptr = new char[++sub.size];
        sub.neg = true;
        for (int i = 1; i < sub.size + 1; i++)
        {
            sub.ptr[i] = r.ptr[i - 1];
        }
        sub.ptr[0] = '-';
        delete[] result1->ptr;
        result1->ptr = NULL;
        delete result1;
        result1 = NULL;
        return sub;
    }
    int size1 = (size >= val.size) ? size : val.size;
    char* diff = new char[size1 + 1];
    for (int i = 0; i < size1; i++)
    {
        diff[i] = '0';
    }
    diff[size] = '\0';
    int carry = 0;
    for (int i = size - 1, j = val.size - 1, k = size1 - 1; k >= 0; i--, j--, k--)
    {
        int x, y;
        if (i >= 0)
            x = ptr[i] - '0';
        else
            x = 0;
        if (j >= 0)
            y = val.ptr[j] - '0';
        else
            y = 0;
        int z = x - y - carry;
        if (z < 0)
        {
            z = z + 10;
            carry = 1;
        }
        else
        {
            carry = 0;
        }
        diff[k] = z + '0';
    }
    int pos = 0;
    if(size1!=1)
    while (pos < size1 && diff[pos] == '0')
    {
        pos++;
    }
    char *ptt=new char[size1 - pos];
    for (int i = pos; i < size1; i++)
    {
        ptt[i - pos] = diff[i];
    }
    ptt[size1 - pos] = '\0';

    BigInt ret(ptt);
    ret.ptr[size] = '\0';
    //delete[] diff;
    //delete[] ptt;
    //diff = NULL;
    return ret;
}
BigInt BigInt::operator-(int val1) const
{
    BigInt* val = new  BigInt(val1);
    *val = *this - *val;
    return *val;

}
BigInt BigInt::operator%(const BigInt& divisor) const {
    BigInt temp(*this);
    BigInt absDivisor = divisor;
    absDivisor.neg = false; // Ensure divisor is positive

    while (temp >= absDivisor) {
        BigInt multiple = absDivisor;
        BigInt count(1);

        while ((multiple + multiple) <= temp) {
            multiple = multiple + multiple;
            count = count + count;
        }

        temp = temp - multiple;
    }

    return temp;
}


BigInt BigInt::binaryToDecimal(const BigInt& binaryNum) {
    int p = 0;
    BigInt decimalNum("0");
    BigInt two(2);
    for (int i = binaryNum.size - 1; i >= 0; --i) {
        int a = binaryNum.ptr[i] - '0';

        BigInt digit(a);

        BigInt digitValue("1");
        for (int j = 0; j < p; j++)
        {
            digitValue *= two;
        }
        if (a == 0)
            digitValue = BigInt(0);
        else
            digitValue *= digit;
        decimalNum = decimalNum + digitValue;
        p++;
    }
    return decimalNum;

}
BigInt BigInt::HexToDecimal(const std::string& hexNum) {
    BigInt decimalNum("0");
    BigInt sixteen(16);

    for (int i = hexNum.size() - 1; i >= 0; --i) {
        int digitValue = 0;

        if (hexNum[i] >= '0' && hexNum[i] <= '9') {
            digitValue = hexNum[i] - '0'; 
        }
        else if (hexNum[i] >= 'A' && hexNum[i] <= 'F') {
            digitValue = hexNum[i] - 'A' + 10; 
        }
        else if (hexNum[i] >= 'a' && hexNum[i] <= 'f') {
            digitValue = hexNum[i] - 'a' + 10; 
        }

        BigInt digit(digitValue);

        BigInt digitWeight("1");
        for (int j = hexNum.size() - 1; j > i; --j) {
            digitWeight *= sixteen;
        }

        if (digitValue == 0)
            digitWeight = BigInt(0);
        else
            digitWeight *= digit;

        decimalNum = decimalNum + digitWeight;
    }
    return decimalNum;
}

BigInt BigInt::generateRandomBigInt(int bits) {
    std::string randomNumber;
    for (int i = 0; i < bits; ++i) {
        randomNumber += to_string(rand() % 2);
    }
    BigInt randomBigInt(randomNumber);
    return this->binaryToDecimal(randomBigInt);
}
BigInt::~BigInt()
{
    /*
    if(ptr!=nullptr)
    delete[] ptr;
    ptr = NULL; */
}
BigInt BigInt::operator+=(const BigInt& rhs)
{

    BigInt* aa = new BigInt(*this);
    *aa = *aa + rhs;
    delete[] this->ptr;
    this->ptr = NULL;
    this->ptr = new char[aa->size];
    for (int i = 0; i < aa->size; i++)
    {
        this->ptr[i] = aa->ptr[i];
    }
    this->ptr[aa->size] = '\0';
    this->size = aa->size;
    delete[] aa->ptr;
    delete aa;
    return *this;
}
BigInt BigInt::operator-=(const BigInt& rhs)
{
    BigInt* aa = new BigInt(*this);
    *aa = *aa - rhs;
    delete[] this->ptr;
    this->ptr = NULL;
    this->ptr = new char[aa->size];
    for (int i = 0; i < aa->size; i++)
    {
        this->ptr[i] = aa->ptr[i];
    }
    this->ptr[aa->size] = '\0';
    this->size = aa->size;
    delete[] aa->ptr;
    delete aa;
    return *this;
}
BigInt& BigInt::operator*(const BigInt& val) const
{
    bool t = 0;
    bool y = 0;
    bool u = 0;
    bool p = 0;
    int ab = val.ptr[0] - '0';
    int aa = ptr[0] - '0';
    t = (ab == 0) ? 1 : 0;
    y = (aa == 0) ? 1 : 0;
    u = (size == 1 && y == 1) ? 1 : 0;
    p = (val.size == 1 && t == 1) ? 1 : 0;
    if (u || p)
    {
        BigInt a;
        return a;
    }
    
    int mulsize = val.size + size;
    char* mul = new char[mulsize + 1];
    for (int i = 0; i < mulsize; i++)
    {
        mul[i] = '0';
    }
    mul[mulsize] = '\0';
    for (int i = size - 1; i >= 0; i--)
    {
        int carry = 0;
        for (int j = val.size - 1; j >= 0; j--)
        {
            int cross = (ptr[i] - '0') * (val.ptr[j] - '0') + carry + (mul[i + j + 1] - '0');
            carry = cross / 10;
            mul[i + j + 1] = cross % 10 + '0';
        }
        mul[i] = mul[i] + carry;
    }
    int pos = 0;
    while (pos >= 0 && mul[pos] == '0')
    {
        pos++;
    }
    bool q = 0;
    q = (pos == 0) ? 1 : 0;
    switch (q)
    {
    case 1:
        BigInt *ret=new BigInt(mul);
        delete[] mul;
        mul = NULL;
        return *ret;
    }

    int ns = mulsize - pos;
    char* result1 = new char[ns+ 1];
    for (int i = 0; i < ns; i++)
    {
        result1[i] = mul[pos + i];
    }
    result1[ns] = '\0';
    BigInt *ret=new BigInt(result1);
    delete[] mul;
    delete[] result1;
    mul, result1 = NULL;
    return *ret;
}
BigInt BigInt::operator*=(const BigInt& rhs) {
    *this = *this * rhs; 
    return *this;
}
bool BigInt::operator<(const BigInt& val) const
{
    if (ptr[0] == '-' && val.ptr[0] != '-')
    {
        return true;
    }
    if (val.ptr[0] == '-' && ptr[0] != '-')
    {
        return false;
    }
    if (size != val.size)
    {
        return size < val.size;
    }
    if (ptr[0] != '-' && val.ptr[0] != '-')
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != val.ptr[i])
            {
                return ptr[i] < val.ptr[i];
            }
        }
    }
    if (ptr[0] == '-' && val.ptr[0] == '-')
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != val.ptr[i])
            {
                return ptr[i] > val.ptr[i];
            }
        }
    }
    return false;
}
bool BigInt::operator<=(const BigInt& val) const
{
    if (ptr[0] == '-' && val.ptr[0] != '-')
    {
        return true;
    }
    if (val.ptr[0] == '-' && ptr[0] != '-')
    {
        return false;
    }
    if (size != val.size)
    {
        return size < val.size;
    }
    if (ptr[0] != '-' && val.ptr[0] != '-')
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != val.ptr[i])
            {
                return ptr[i] < val.ptr[i];
            }
        }
    }
    if (ptr[0] == '-' && val.ptr[0] == '-')
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != val.ptr[i])
            {
                return ptr[i] > val.ptr[i];
            }
        }
    }
    return true;
}
bool BigInt::operator==(const BigInt& val) const
{
    if (size != val.size)
    {
        return false;
    }
    for (int i = 0; i < size; i++)
    {
        if (ptr[i] != val.ptr[i])
        {
            return false;
        }
    }

    return true;
}
bool BigInt::operator==(const char* val) const
{
    for (int i = 0; i < size; i++)
    {
        if (ptr[i] != val[i])
        {
            return false;
        }
    }

    return true;
}
bool BigInt::operator!=(const BigInt& val) const
{
    if (size != val.size)
    {
        return true;
    }

    for (int i = 0; i < size; i++)
    {
        if (ptr[i] != val.ptr[i])
        {
            return true;
        }
    }

    return false;
}
bool BigInt::operator>(const BigInt& val) const
{
    if (ptr[0] == '-' && val.ptr[0] != '-')
    {
        return false;
    }
    if (val.ptr[0] == '-' && ptr[0] != '-')
    {
        return true;
    }
    if (size != val.size)
    {
        return size > val.size;
    }
    if (ptr[0] != '-' && val.ptr[0] != '-')
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != val.ptr[i])
            {
                return ptr[i] > val.ptr[i];
            }
        }
    }
    if (ptr[0] == '-' && val.ptr[0] == '-')
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != val.ptr[i])
            {
                return ptr[i] < val.ptr[i];
            }
        }
    }
    return false;
}
bool BigInt::operator>=(const BigInt& val) const
{
    if (ptr[0] == '-' && val.ptr[0] != '-')
    {
        return false;
    }
    if (val.ptr[0] == '-' && ptr[0] != '-')
    {
        return true;
    }
    if (size != val.size)
    {
        return size > val.size;
    }
    if (ptr[0] != '-' && val.ptr[0] != '-')
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != val.ptr[i])
            {
                return ptr[i] > val.ptr[i];
            }
        }
    }
    if (ptr[0] == '-' && val.ptr[0] == '-')
    {
        for (int i = 0; i < size; i++)
        {
            if (ptr[i] != val.ptr[i])
            {
                return ptr[i] < val.ptr[i];
            }
        }
    }
    return true;
}
BigInt BigInt::Power(string exp)
{
    BigInt Res(1);
    BigInt expo(exp);
    BigInt zero(0);
    BigInt base(*this);
    BigInt two(2);
    if (expo == zero)
    {
        return Res;
    }
    if (expo == Res)
    {
        return base;
    }
    for (BigInt i(1); i < expo; i++)
    {
        base = base * *this;
    }
    return base;
}
BigInt& BigInt::operator++()
{
    *this = *this + 1;
    return *this;
}
BigInt BigInt::operator++(int a)
{
    BigInt ab(*this);
    *this = *this + 1;
    return *this;
    return ab;
}
BigInt BigInt::operator--(int a)
{
    BigInt ab(*this);
    *this = *this - 1;
    return ab;
}
BigInt& BigInt::operator--()
{
    *this = *this - 1;
    return *this;
}
ostream& operator<<(ostream& output, const BigInt& val)
{
    for (int i = 0; i < val.size; i++)
    {
        output << val.ptr[i];
    }
    return output;
}
istream& operator>>(istream& input, BigInt& val)
{
    int a = 0;
    cout << "Enter Number: " << endl;
    cin >> a;
    BigInt ret(a);
    val = ret;
    return input;
}
string BigInt::toString(const BigInt val)
{
    BigInt ret(val);
    return string(ret);
}
BigInt::operator int()
{
    if (ptr[0] == '-')
        return size - 1;
    return size;
}