#include <iostream>
#include <vector>

class BigInt{
    static const long long BASE = 1000;
    std::vector<long long> _digits;
    bool _is_negative;
public:
    BigInt();
    template<typename Integers>
    explicit BigInt(Integers i);

    friend std::ostream& operator<<(std::ostream&, const BigInt&);
    friend std::istream& operator>>(std::istream&, BigInt&);
    const BigInt operator -() const;
    friend bool operator <(const BigInt&, const BigInt&);
    friend bool operator ==(const BigInt&, const BigInt&);

};

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
        for (long long i = static_cast<long long>(bi._digits.size())-1; i>=0; --i)
            os << bi._digits[i];
    }
    return os;
}

// istream from stdin
std::istream& operator>>(std::istream& in, BigInt& bi){
    char current_input;
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

int main() {
    BigInt bi, bi2;
    std::cin >> bi >> bi2;
    if (bi == bi2){
        std::cout << "=";
    } else if (bi < bi2){
        std::cout << "<";
    } else
        std::cout << ">";
    return 0;
}
