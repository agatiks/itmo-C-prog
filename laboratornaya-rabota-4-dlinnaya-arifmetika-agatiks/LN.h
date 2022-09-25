//
// Created by agata on 26.05.2021.
//

#ifndef UNTITLED2_LN_H
#define UNTITLED2_LN_H


#include <string_view>
#include <iostream>
#include <cstring>


class LN {
    //static const int BASE = 10;
public:

    int sign_;
    int* digits_;
    long long size_;
    bool isNaN_;
    LN(int sign, int* digits, long long size, bool isNaN);
    LN(const LN &ln);
    LN(LN && ln); // ?noexcept
    explicit LN(long long int ln = 0ll); // ?explicit
    LN(const char* str);
    LN(std::string_view str);
    ~LN();
    LN& operator=(const LN &ln);
    LN& operator=(LN&& ln);

    LN operator-() const;
    LN abs() const;

    LN operator+(const LN &ln) const;
    LN operator-(const LN &ln) const;
    LN operator*(const LN &ln) const;
    LN operator/(const LN &ln) const;
    LN operator%(const LN &ln) const;
    LN& operator+=(const LN& ln);
    LN& operator-=(const LN& ln);
    LN& operator*=(const LN& ln);

    operator long long() const;
    operator bool() const;

    LN operator~() const;
    bool operator==(const LN &ln) const;
    bool operator<(const LN &ln) const;
    bool operator!=(const LN &ln) const;
    bool operator<=(const LN &ln) const;
    bool operator>(const LN &ln) const;
    bool operator>=(const LN &ln) const;

    std::string print() const;

    int hCmp(const LN &ln) const;

    int cmp(const LN &ln) const;

private:
    void remove_zero();

    LN add(const LN &ln) const;

    LN sub(const LN &ln) const;

    LN mul(const LN &ln) const;

    LN div(const LN &ln) const;

    void next();

    short getSign(long long ln);

    long long getSize(long long ln);

    long long int getAbs(long long int ln);
};


#endif //UNTITLED2_LN_H
