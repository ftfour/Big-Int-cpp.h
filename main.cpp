#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

class BigInt{
    static const long long BASE = 1000;
    static const long long BASE_LENGHT = 3;
    std::vector<long long> _digits;
    bool _is_negative;
public:
    class divide_by_zero: public std::exception {  };
    BigInt();
    template<typename Integers>
    explicit BigInt(Integers i);

    friend std::ostream& operator<<(std::ostream&, const BigInt&);
    friend std::istream& operator>>(std::istream&, BigInt&);
    const BigInt operator -() const;
    friend bool operator <(const BigInt&, const BigInt&);
    friend bool operator ==(const BigInt&, const BigInt&);
    const BigInt operator +() const;
    BigInt& operator +=(const BigInt&);
    friend const BigInt operator +(BigInt, const BigInt&);
    friend const BigInt operator -(BigInt, const BigInt&);
    template<typename Integers>
    friend BigInt operator /(const BigInt& , Integers );
    friend BigInt operator *(const BigInt& , const BigInt& );
//    template<typename Integers>
//    friend BigInt operator *(const BigInt& , Integers );
    template<typename Integers>
    friend BigInt operator %(const BigInt& ,  Integers );

    void _remove_leading_zeros();
};

int main() {
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");
    BigInt bi, bi2;
    int a;
    input >> bi >> bi2;
    output << bi * bi2;
    input.close(); output.close();
    return 0;
}


// create BigInt with value 0
BigInt::BigInt(){
    this->_is_negative = false;
};

// create BigInt from integers template;
template<typename Integers>
BigInt::BigInt(Integers i) {
    this->_is_negative = i < 0;
    do{
        this->_digits.push_back(std::abs(i) % BigInt::BASE);
    }while ( i /= BigInt::BASE);
}

// ostream to stdout;
std::ostream& operator <<(std::ostream& os, const BigInt& bi){
    if (bi._digits.empty()) os << 0;
    else {
        if (bi._is_negative) os << "-";
        os << bi._digits[bi._digits.size()-1];
        for (long long i = static_cast<long long>(bi._digits.size())-2; i>=0; --i)
            os << std::setfill('0') << std::setw(BigInt::BASE_LENGHT) << bi._digits[i];
    }
    return os;
}

// istream from stdin
std::istream& operator>>(std::istream& in, BigInt& bi){
    char current_input;
    bi._is_negative = false;
    current_input = (char)in.get();
    if (current_input=='-'){
        bi._is_negative = true;
        current_input = (char)in.get();
    }
    while (current_input != '\n' && current_input != ' ' && !in.eof()){
        if (bi._digits.size() == 0) {
            bi._digits.push_back(current_input-'0');
        }
        else{
            for ( long long i = static_cast<long long>(bi._digits.size()) - 1; i >= 0; --i){
                bi._digits[i] *= 10;
                if (bi._digits[i] > BigInt::BASE){
                    if (i==bi._digits.size()-1) bi._digits.push_back(0);
                    bi._digits[i+1] += (bi._digits[i]) / BigInt::BASE;
                    bi._digits[i] %= BigInt::BASE;
                }
            }
            bi._digits[0] += current_input - '0';
        }
        current_input = (char)in.get();
    }
    return in;
};


const BigInt BigInt::operator -() const {
    BigInt copy(*this);
    copy._is_negative = !copy._is_negative;
    return copy;
}

bool operator ==(const BigInt& left, const BigInt& right) {
    if (left._is_negative != right._is_negative) return false;
    if (left._digits.empty()) {
        if (right._digits.empty() || (right._digits.size() == 1 && right._digits[0] == 0)) return true;
        else return false;
    }

    if (right._digits.empty()) {
        if (left._digits.size() == 1 && left._digits[0] == 0) return true;
        else return false;
    }

    if (left._digits.size() != right._digits.size()) return false;
    for (size_t i = 0; i < left._digits.size(); ++i) if (left._digits[i] != right._digits[i]) return false;

    return true;
}
// compare bigints
bool operator <(const BigInt& left, const BigInt& right){
    if (left == right) return false;
    if (left._is_negative) {
        if (right._is_negative) return ((-right) < (-left));
        else return true;
    }
    else if (right._is_negative) return false;
    else {
        if (left._digits.size() != right._digits.size()) {
            return left._digits.size() < right._digits.size();
        }
        else {
            for (long long i = static_cast<long long>(left._digits.size()) - 1; i >= 0; --i) {
                if (left._digits[i] != right._digits[i]) return left._digits[i] < right._digits[i];
            }

            return false;
        }
    }
}

// return copy
const BigInt BigInt::operator +() const {
    return BigInt(*this);
}
// sum
const BigInt operator +(BigInt left, const BigInt& right) {
    if (left._is_negative) {
        if (right._is_negative) return -(-left + (-right));
        else return right - (-left);
    }
    else if (right._is_negative) return left - (-right);
    int carry = 0;
    for (size_t i = 0; i < std::max(left._digits.size(), right._digits.size()) || carry != 0; ++i) {
        if (i == left._digits.size()) left._digits.push_back(0);
        left._digits[i] += carry + (i < right._digits.size() ? right._digits[i] : 0);
        carry = left._digits[i] >= BigInt::BASE;
        if (carry != 0) left._digits[i] -= BigInt::BASE;
    }

    return left;
}
BigInt &BigInt::operator+=(const BigInt & value) {
    return *this = (*this + value);
}

const BigInt operator -(BigInt left, const BigInt& right) {
    if (right._is_negative) return left + (-right);
    else if (left._is_negative) return -(-left + right);
    else if (left < right) return -(right - left);
    int carry = 0;
    for (size_t i = 0; i < right._digits.size() || carry != 0; ++i) {
        left._digits[i] -= carry + (i < right._digits.size() ? right._digits[i] : 0);
        carry = left._digits[i] < 0;
        if (carry != 0) left._digits[i] += BigInt::BASE;
    }

    return left;
}
//

template<typename Integers>
BigInt operator/(const BigInt &left, Integers right) {
    if ( right == 0) throw BigInt::divide_by_zero();
    BigInt result;
    result._is_negative = left._is_negative != (right < 0);
    size_t abs_right = abs(right);
    size_t shift = 0;
    result._digits.resize(left._digits.size());
    for (long long i = static_cast<long long>(left._digits.size()) - 1 ; i >= 0; --i){
        size_t current = left._digits[i] + shift * BigInt::BASE;
        result._digits[i] = current / abs_right;
        shift = current % abs_right;
        if (!result._digits[i]){
            result._digits.pop_back();
        }
    }
    return result;
}
BigInt operator*(const BigInt &left, const BigInt &right) {
    BigInt result;
    result._digits.resize(left._digits.size() + right._digits.size());
    for (size_t i = 0; i < left._digits.size(); ++i) {
        int carry = 0;
        for (size_t j = 0; j < right._digits.size() || carry != 0; ++j) {
            long long cur = result._digits[i + j] +
                            left._digits[i] * 1LL * (j < right._digits.size() ? right._digits[j] : 0) + carry;
            result._digits[i + j] = static_cast<int>(cur % BigInt::BASE);
            carry = static_cast<int>(cur / BigInt::BASE);
        }
    }

    result._is_negative = left._is_negative != right._is_negative;
    result._remove_leading_zeros();
    return result;
}

// A = A/B * B + A mod B;
// A mod = -(A/B) * B
template<typename Integers>
BigInt operator%(const BigInt &left, Integers right) {
    if ( right == 0) return left;
    BigInt right_bi(right);
    BigInt result = left - (left/right) * right_bi;
    if (result._is_negative) result += right_bi;
    result._remove_leading_zeros();
    return result;
}

void BigInt::_remove_leading_zeros() {
    while (this->_digits.size() > 1 && this->_digits.back() == 0) {
        this->_digits.pop_back();
    }

    if (this->_digits.size() == 1 && this->_digits[0] == 0) this->_is_negative = false;
}
