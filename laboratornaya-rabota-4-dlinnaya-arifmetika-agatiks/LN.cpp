#include "LN.h"
#include <cmath>
#include <climits>

using namespace std::literals;

LN::LN(int sign, int* digits,long long size, bool isNaN)
{
    sign_ = sign;
    size_ = size;
    isNaN_ = isNaN;
    if (!isNaN_) {
        digits_ = new int[size_];
        for (long long i = 0; i < size_; i++) {
            digits_[i] = digits[i];
        }
        this->remove_zero();
    } else {
        digits_ = nullptr;
    }
}

LN::LN(const LN &ln)// конструктор копирования
{
    //  если есть а от в, то как должен срабатывать конструктор копирования
    //  при изменении в, меняется ли а
    //  буду считать, что при копировании создаётся новый массив,
    //  а при перемещении указатель в той же области памяти
    sign_ = ln.sign_;
    size_ = ln.size_;
    isNaN_ = ln.isNaN_;
    if (!isNaN_) {
        digits_ = new int[size_];
        for (long long i = 0; i < size_; i++) {
            digits_[i] = ln.digits_[i];
        }
    } else {
        digits_ = nullptr;
    }
}

LN::LN(LN &&ln) //?// конструктор перемещения
{
    //  нужно ли тут удалять переменную от которой строим
    sign_ = ln.sign_;
    size_ = ln.size_;
    isNaN_ = ln.isNaN_;
    digits_ = ln.digits_;
    ln.isNaN_ = true;
    ln.sign_ = 0;
    ln.size_ = 0;
    ln.digits_ = nullptr;
}

LN::LN(long long ln) // из long long со значением по умолчанию: 0
{
    sign_ = getSign(ln);
    isNaN_ = false;
    long long ln_abs = getAbs(ln);
    size_ = getSize(ln_abs);
    //delete[] digits_;
    digits_ = new int[size_];
    for (long long i = 0; i < size_; i++) {
        digits_[i] = (int) ln_abs % 10;
        ln_abs /= 10;
    }
}

LN::LN(const char *str) // из строки символов C (const char *)
{
    long long start = 0;
    long long end = (long long) strlen(str);
    if (end == 0) {
        isNaN_ = true;
        sign_ = 0;
        size_ = 0;
        digits_ = nullptr;
        return;
    }
    isNaN_ = false;
    sign_ = 1;
    if(str[0] == '-') {
        start += 1;
        sign_ = -1;
    }
    while(str[start] == '0' && start < end) {
        start += 1;
    }
    if(start == end) {
        sign_ = 0;
        size_ = 1;
        digits_ = new int[1];
        digits_[0] = 0;
    }
    else {
        size_ = end - start - 1;
        digits_ = new int[size_];
        for (long long i = 0; i < size_; i++) {
            digits_[i] = str[end - i - 2]-'0';
            // ошибка не цифорка
        }
    }
}

LN::~LN()
{
    size_ = 0;
    sign_ = 0;
    if(digits_ != nullptr) {
        delete[] digits_;
    }
    isNaN_ = true;
}

LN LN::operator-() const // унарный минус
{
    if (sign_ == 0 || isNaN_) {
        return *this;
    }
    //return LN(-1);
    return LN(-sign_, digits_, size_, isNaN_);
}

LN LN::abs() const
{
    return LN((sign_ >= 0 ? sign_ : 1), digits_, size_, isNaN_);
}

LN LN::add(const LN&ln) const // вспом сложение, для неотрицательных
{
    if(ln.sign_ == 0 && sign_ == 0) {
        return LN(0ll);
    }
    int carry = 0;
    LN first = *this;
    LN second = ln;
    if (second.size_ > first.size_) {
        LN temp = second;
        second = first;
        first = temp;
    }
    for (long long i = 0; i < first.size_; i++) {
        int temp = carry + first.digits_[i] + (i < second.size_ ? second.digits_[i] : 0);
        carry = temp / 10;
    }
    int bigger = 0;
    if(carry != 0) {
        bigger = 1;
    }
    carry = 0;
    short n_sign = 1;
    long long n_size = first.size_ + bigger;
    int* n_digits = new int[n_size];
    for (long long i = 0; i < first.size_; i++) {
        int temp = carry + first.digits_[i] + (i < second.size_ ? second.digits_[i] : 0);
        carry = temp / 10;
        n_digits[i] = temp % 10;
    }
    if(carry != 0) {
        n_digits[first.size_] = carry;
    }
    return LN(n_sign, n_digits, n_size, false);
}

LN LN::sub(const LN &ln) const // вспом вычитание, для неотриц из большего меньшее
{
    if((ln.sign_ == 0 && sign_ == 0) || *this == ln) {
        return LN(0ll);
    }
    int carry = 0;
    long long n_size = 0;
    for (long long i = 0; i < size_; i++) {
        int temp =  digits_[i] - carry - (i < ln.size_ ? ln.digits_[i] : 0);
        if(temp % 10 != 0) {
            n_size = i + 1;
        }
        carry = (temp < 0) ? 1 : 0;
    }
    carry = 0;
    short n_sign = 1;
    int* n_digits = new int[n_size];
    for (long long i = 0; i < n_size; i++) {
        int temp = digits_[i] - carry - ln.digits_[i];
        if (temp < 0) {
            carry = 1;
            temp += 10;
        }
        n_digits[i] = temp;
    }
    return LN(n_sign, n_digits, n_size, false);
}

LN LN::mul(const LN &ln) const // вспом умножение, для полож
{
    long long n_size = size_ + ln.size_;
    int *n_digits = new int[n_size];
    for (long long i = 0; i < n_size; i++){
        n_digits[i] = 0;
    }
    for (long long i = 0; i < size_; i++) {
        int carry = 0;
        for (long long j = 0; j < ln.size_; j++) {
            int temp = n_digits[i + j] +digits_[i]*
                    (j < ln.size_ ? ln.digits_[j] : 0) + carry;
            n_digits[i + j] = temp % 10;
            carry = temp / 10;
        }
    }
    LN x = LN(1, n_digits, n_size, false);
    x.remove_zero();
    return x;
}

void LN::next()
{
    int *n_digits = new int[size_ + 1];
    size_ += 1;
    for (long long i = size_ - 1; i > 0; i--){
        n_digits[i] = digits_[i-1];
    }
    free(digits_);
    *digits_ = *n_digits;
}
LN LN::div(const LN &ln) const // вспом деление
{
    int *n_digits = new int[size_];
    LN curr;
    std::cout << curr.print() << std::endl;
    for (long long i = size_ - 1; i >= 0; i--) {
        curr.next();
        curr.digits_[0] = digits_[i];
        int digit;
        LN  *ln_digit, *temp;
        for (int m = 0; m < 10; m++) {
            ln_digit = new LN(m);
            std::cout << ln_digit->size_ << std::endl;
            std::cout << ln_digit->print() << std::endl;
            temp = new LN(*ln_digit * ln);
            if(*temp > curr) {
                digit = m - 1;
                break;
            }
        }
        n_digits[i] = digit;
        curr = curr - LN(digit)*curr;
        curr.remove_zero();
    }
    LN *ans = new LN(1, n_digits, size_, false);
    ans->remove_zero();
    return *ans;
}

LN LN::operator+(const LN &ln) const // сложение
{
    if(isNaN_ && ln.isNaN_) {
        return LN("");
    }
    if (sign_ == -1) {
        if (ln.sign_ == -1) {
            return -((-*this) + (-ln));
        } else {
            return ln - (-*this);
        }
    }


    if(ln.sign_ == -1) {
        return  *this - (-ln);
    }
    std::cout << this->print() << " " << ln.print() << std::endl;

    return this->add(ln);
}

LN LN::operator/(const LN &ln) const
{
    if(isNaN_ || ln.isNaN_) {
        return LN("");
    }
    if (ln.sign_ == 0) {
        throw 1;
    }
    if(sign_ == 0) {
        return LN();
    }
    const LN A = this->abs();
    const LN B = ln.abs();
    if (A < B) {
        return LN();
    }
    if(sign_ == ln.sign_) {
        return A.div(B);
    } else {
        return -A.div(B);
    }
}

LN& LN::operator+=(const LN& ln)
{
    return *this = *this + ln;
}

LN& LN::operator-=(const LN& ln)
{
    return *this = *this - ln;
}

LN& LN::operator*=(const LN& ln)
{
    return *this = *this * ln;
}

LN LN::operator-(const LN &ln) const // вычитание
{
    if (isNaN_ && ln.isNaN_) {
        return LN("");
    }
    if (ln.sign_ == -1) {
        return  *this + -ln;
    } else if (sign_ == -1) {
        return -(-*this + ln);
    } else if (*this < ln) {
        return -(ln - *this);
    }
    return this->sub(ln);
}

LN LN::operator*(const LN &ln) const // умножение
{
    if(ln.isNaN_ || isNaN_) {
        return LN("");
    }
    if(ln.sign_ == 0 || sign_ == 0) {
        return LN();
    }
    if(sign_ == -1) {
        if (ln.sign_ == -1) {
            return (-*this).mul(-ln);
        } else {
            return -((-*this).mul(ln));
        }
    } else {
        if (ln.sign_ == -1) {
            return -(this->mul(-ln));
        } else {
            return this->mul(ln);
        }
    }
}

LN& LN::operator=(const LN& ln) // оператор копирования
{
    if(&ln == this)
        return *this;
    delete[] digits_;
    size_ = ln.size_;
    isNaN_ = ln.isNaN_;
    sign_ = ln.sign_;
    if (!isNaN_) {
        digits_ = new int[size_];
        for (long long i = 0; i < size_; i++) {
            digits_[i] = ln.digits_[i];
        }
    }
    return *this;
}

LN& LN::operator=(LN &&ln)
{
    if(&ln == this) {
        return *this;
    }
    delete[] digits_;
    sign_ = ln.sign_;
    size_ = ln.size_;
    isNaN_ = ln.isNaN_;
    digits_ = ln.digits_;
    ln.digits_ = nullptr;
    ln.size_ = 0;
    ln.isNaN_ = true;
    ln.sign_ = 0;
    return *this;
}

short LN::getSign(long long int ln)
{
    if (ln > 0) return 1;
    else if (ln < 0) return -1;
    else return 0;

}

long long LN::getSize(long long ln)
{
    return ln != 0?((long long) (log10(ln)) + 1):1;
}

long long int LN::getAbs(long long int ln)
{
    if (ln >= 0) return ln;
    else return -ln;
}

int LN::hCmp(const LN &ln) const
{
    if (isNaN_ || ln.isNaN_) {
        return (isNaN_ && ln.isNaN_);
    }
    if (size_ > ln.size_) {
        return 1;
    }
    if (size_ < ln.size_) {
        return -1;
    }
    for (long long i = size_; i >= 0; i--) {
        if (digits_[i] > ln.digits_[i]) {
            return 1;
        }
        if (digits_[i] < ln.digits_[i]) {
            return -1;
        }
    }
    return 0;
}

int LN::cmp(const LN &ln) const
{
    if (isNaN_ || ln.isNaN_) {
        return (isNaN_ && ln.isNaN_);
    }
    if (sign_ + ln.sign_ == 0) {
        return sign_;
    }
    return sign_ * hCmp(ln);
}


bool LN::operator<(const LN &ln) const
{
    return cmp(ln) == -1;
}

bool LN::operator!=(const LN &ln) const
{
    return cmp(ln) != 0;
}

bool LN::operator<=(const LN &ln) const
{
    return (cmp(ln) == -1 || cmp(ln) == 0);
}

bool LN::operator>(const LN& ln) const
{
    return cmp(ln) == 1;
}

bool LN::operator>=(const LN &ln) const
{
    return (cmp(ln) == 1 || cmp(ln) == 0);
}

bool LN::operator==(const LN &ln) const
{
    if(ln.isNaN_ != isNaN_ || ln.size_ != size_ || ln.sign_ != sign_) {
        return false;
    } else {
        bool ans = true;
        for (long long i = 0; i < size_; i++) {
            if (digits_[i] != ln.digits_[i]) {
                ans = false;
                break;
            }
        }
        return ans;
    }
}

void LN::remove_zero()
{
    long long i = size_;
    while (i > 1 && digits_[i - 1] == 0) {
        i--;
    }
    if (i != size_) {
        size_ = i;
        int* temp = new int[size_];
        for (long long j = 0; j < size_; j++) {
            temp[j] = digits_[j];
        }
        delete[] digits_;
        digits_ = temp;
    }
}


std::string LN::print() const
{
    if (isNaN_) {
        return std::string("NaN");
    } else {
        std::string str;
        if (sign_ == -1) {
            str.push_back('-');
        } else if (sign_ == 0) {
            return "0";
        }
        int *digits = digits_;
        long long size = size_;
        for (long long i = size - 1; i >= 0; i--) {
            str.push_back(digits[i]+'0');
        }
        return str;
    }
}

LN::LN(std::string_view str)
{
    long long start = 0;
    long long end = (long long) str.size();
    if (end == 0) {
        isNaN_ = true;
        sign_ = 0;
        size_ = 0;
        return;
    }
    isNaN_ = false;
    sign_ = 1;
    if(str[0] == '-') {
        start += 1;
        sign_ = -1;
    }
    while(str[start] == '0' && start < end) {
        start += 1;
    }
    if(start == end) {
        sign_ = 0;
        size_ = 1;
        digits_ = new int[1];
        digits_[0] = 0;
    }
    else {
        size_ = end - start - 1;
        digits_ = new int[size_];
        for (long long i = 0; i < size_; i++) {
            digits_[i] = str[end - i - 2]-'0';
            // ошибка не цифорка
        }
    }
}

LN::operator long long() const
{
    if(isNaN_) {
        return NAN;
    }
    if(sign_ == 0) {
        return 0;
    }
    if (*this < LN(LLONG_MAX)) {
        long long ans = 0, f = 1;
        for (long long i = 0; i < size_; i++) {
            ans += digits_[i] * f;
            f *= 10;
        }
        return sign_ * ans;
    }
    throw -1;// переполнение long long
}

LN::operator bool() const
{
    return *this != LN();
}






