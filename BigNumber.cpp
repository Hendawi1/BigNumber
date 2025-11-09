#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cstdlib>
#include <stdexcept>

using namespace std;

typedef unsigned char  BASE;
typedef unsigned short DBASE;

#define BASE_SIZE (sizeof(BASE) * 8)
#define DBASE_SIZE (sizeof(DBASE) * 8)
#define BASENUM ((DBASE)1 << BASE_SIZE)

class BigNumber{
    vector<BASE> coefs;
public:
    BigNumber();                        // new default constructor (zero)
    BigNumber(unsigned int len);        // constructor that creates random limbs (previously mode==1)
    BigNumber(string&);
    BigNumber(const BigNumber&);
    ~BigNumber() = default;

    bool operator== (const BigNumber&) const;
    bool operator!= (const BigNumber&) const;
    bool operator>  (const BigNumber&) const;
    bool operator<  (const BigNumber&) const;
    bool operator>= (const BigNumber&) const;
    bool operator<= (const BigNumber&) const;

    BigNumber& operator=  (const BigNumber&);

    BigNumber  operator+  (const BASE&);
    BigNumber& operator+= (const BASE&);
    BigNumber  operator-  (const BASE&);
    BigNumber& operator-= (const BASE&);
    BigNumber  operator*  (const BASE&);
    BigNumber& operator*= (const BASE&);
    BigNumber  operator/  (const BASE&);
    BigNumber& operator/= (const BASE&);
    BigNumber  operator%  (const BASE&);
    BigNumber& operator%= (const BASE&);

    BigNumber  operator+  (const BigNumber&);
    BigNumber& operator+= (const BigNumber&);
    BigNumber  operator-  (const BigNumber&);
    BigNumber& operator-= (const BigNumber&);
    BigNumber  operator*  (const BigNumber&);
    BigNumber& operator*= (const BigNumber&);
    BigNumber  operator/  (const BigNumber&);
    BigNumber& operator/= (const BigNumber&);
    BigNumber  operator%  (const BigNumber&);
    BigNumber& operator%= (const BigNumber&);

    void OutputHex();
    void InputHex();

    BigNumber Shift(int bits);

    unsigned int getLength() const
    {
        return coefs.size();
    }
    
    void PrintBase256();

    friend ostream& operator<< (ostream&, const BigNumber&);
    friend istream& operator>> (istream&, BigNumber&);
};




//--------------------------------------------------------------




BigNumber::BigNumber() {
    coefs.push_back(0);
}

BigNumber::BigNumber(unsigned int len){
    if (!len) return;

    for (int i = 0; i < len-1; i++)
        coefs.push_back(rand());
    BASE coef = rand();
    while (!coef) coef = rand();
    coefs.push_back(coef);
}




BigNumber::BigNumber(string& numInHex){
    if (numInHex.empty()){
        coefs.push_back(0);
        return;
    }

    bitset<BASE_SIZE> coeff(0);
    int n = 0;
    int i = numInHex.size() - 1;

    while (i >= 0){
        int tmpHex = 0;
        // [0,1,..,E,F] = [48,..,102]
        if (numInHex[i] >= '0' && numInHex[i] <= '9') tmpHex = numInHex[i] - '0'; // - 48
        // [a,b,..,f] = [97,..,102] 
        else if (numInHex[i] >= 'a' && numInHex[i] <= 'f') tmpHex = numInHex[i] - 'a' + 10; // -97 + 10
        // [A,..,F] = [65,..70]
        else if (numInHex[i] >= 'A' && numInHex[i] <= 'F') tmpHex = numInHex[i] - 'A' + 10; // -65 + 10
        else {
            cout << "Wrong symbol!!!\n";
            exit(-1);
        }
        // 0 = 0000 ; F = 1111
        bitset<BASE_SIZE> tmp(tmpHex); // [0000****]
        coeff |= tmp << n;
        n += 4; // adin symvol = 4-bits

        if (n >= BASE_SIZE) {
            n = 0;
            coefs.push_back(coeff.to_ulong());
            coeff.reset();
        }
        i--;
    }

    if (n > 0) coefs.push_back(coeff.to_ulong());

    int len = coefs.size();
    while (coefs[len - 1] == 0 && len > 1){
        len--;
        coefs.pop_back();
    }
}




BigNumber::BigNumber(const BigNumber& other){
    coefs = other.coefs;
}



// Added: comparison operators and assignment (merged implementations)
bool BigNumber::operator== (const BigNumber& other) const
{
    return coefs == other.coefs; // victor method
}

bool BigNumber::operator!= (const BigNumber& other) const
{
    return coefs != other.coefs; // victor method
}

bool BigNumber::operator> (const BigNumber& other) const
{
    int len_l = coefs.size();
    int len_r = other.coefs.size();

    if (len_l > len_r)
        return true;

    if (len_l < len_r)
        return false;

    for (int i = len_l - 1; i >= 0; i--)
    {
        if (coefs[i] < other.coefs[i])
            return false;
        if (coefs[i] > other.coefs[i])
            return true;
    }

    return false;
}

bool BigNumber::operator< (const BigNumber& other) const
{
    int len_l = coefs.size();
    int len_r = other.coefs.size();

    if (len_l < len_r)
        return true;

    if (len_l > len_r)
        return false;

    for (int i = len_l - 1; i >= 0; i--)
    {
        if (coefs[i] > other.coefs[i])
            return false;
        if (coefs[i] < other.coefs[i])
            return true;
    }

    return false;
}

bool BigNumber::operator<= (const BigNumber& other) const
{
    return !(*this > other);
}

bool BigNumber::operator>= (const BigNumber& other) const
{
    return !(*this < other);
}

BigNumber& BigNumber::operator= (const BigNumber& other)
{
    if (this != &other)
    {
        coefs.clear();
        for (int i = 0; i < other.coefs.size(); i++)
            coefs.push_back(other.coefs[i]);
    }
    return *this;
}









// --------------------- Added BASE-operand methods --------------------

// Addition of a small digit (BASE)
BigNumber BigNumber::operator+ (const BASE& num)
{
    BigNumber res = *this;  // نبدأ بنسخة من العدد الأصلي
    DBASE carry = num;      // نعتبر الرقم كحمل ابتدائي
    
    for (size_t i = 0; i < res.coefs.size() && carry > 0; i++)
    {
        DBASE tmp = DBASE(res.coefs[i]) + carry;
        res.coefs[i] = BASE(tmp);
        carry = tmp >> BASE_SIZE;
    }
    
    if (carry > 0)
        res.coefs.push_back(BASE(carry));
    int len = res.coefs.size();
    while (len > 1 && res.coefs[len - 1] == 0) {
        res.coefs.pop_back();
        len--;
    }
        
    return res;
}

BigNumber& BigNumber::operator+= (const BASE& num)
{
    *this = *this + num;
    return *this;
}

BigNumber BigNumber::operator- (const BASE& num){
    // التحقق من السالبية بحسب القواعد المطلوبة
    if (coefs.size() < 1 || (coefs.size() == 1 && coefs[0] < num)) throw std::underflow_error("Negative result");

    BigNumber res = *this;
    DBASE borrow = num;

    for (size_t i = 0; i < res.coefs.size() && borrow > 0; i++)
    {
        DBASE tmp = DBASE(res.coefs[i]) + BASENUM - borrow;
        res.coefs[i] = BASE(tmp);
        borrow = 1 - (tmp >> BASE_SIZE);  // 1 إذا كان هناك قرض، 0 إذا لا
    }

    int len = res.coefs.size();
    while (len > 1 && res.coefs[len - 1] == 0) {
        res.coefs.pop_back();
        len--;
    }
    return res;

}


BigNumber& BigNumber::operator-= (const BASE& num)
{
    *this = *this - num;
    return *this;
}

// Multiplication by small digit (BASE)
BigNumber BigNumber::operator* (const BASE& num)
{
    int len = coefs.size();
    if (len == 0) return BigNumber();

    BigNumber res(0);
    DBASE carry = 0;
    for (int i = 0; i < len; ++i) {
        DBASE prod = DBASE(coefs[i]) * DBASE(num) + carry;
        res.coefs.push_back(BASE(prod));
        carry = prod >> BASE_SIZE;
    }

    if (carry != 0) res.coefs.push_back(BASE(carry));
    return res;
}

BigNumber& BigNumber::operator*= (const BASE& num)
{
    *this = *this * num;
    return *this;
}

// Division by small digit (BASE)
BigNumber BigNumber::operator/ (const BASE& num)
{
    if (num == 0) {
        cout << "Error other == 0 !!!!\n";
        exit(-3);
    }
    int len = coefs.size();
    if (len == 0) return BigNumber();

    BigNumber res;
    res.coefs.resize(len, 0);

    DBASE tmp = 0;
    BASE r = 0;
    int j = 0;
    while (j < len) {
        /*
        
NUM: 1234   =    1    2   3   4
                 10³ 10² 10¹ 10⁰

	OSTATAK FROM:  1 -> 2:  × 10
	OSTATAK FROM:  2 -> 3:  × 10  
	OSTATAK FROM:  3 -> 4:  × 10

-----------------------------------------------

NUM: [155,  75,  30]
      256² 256¹ 256⁰

	OSTATAK FROM 30 -> 75:  × 256
	OSTATAK FROM 75 -> 155:  × 256
        
        */
        tmp = ( (DBASE)r << BASE_SIZE ) + DBASE(coefs[len - 1 - j]);
        res.coefs[len - 1 - j] = BASE(tmp / DBASE(num));
        r = BASE(tmp % DBASE(num));
        j++;
    }

    // strip leading zeros
    int idx = res.coefs.size() - 1;
    while (idx > 0 && res.coefs[idx] == 0) {
        res.coefs.pop_back();
        idx--;
    }
    return res;
}

BigNumber& BigNumber::operator/= (const BASE& num)
{
    *this = *this / num;
    return *this;
}

// Remainder mod small digit (BASE)
BigNumber BigNumber::operator% (const BASE& num)
{
    if (num == 0) {
        cout << "Error: in operator% (other % 0)!\n";
        exit(-4);
    }
    int len = coefs.size();
    DBASE tmp = 0;
    BASE r = 0;
    int i = 0;
    while (i < len) {
        tmp = ((DBASE)r << BASE_SIZE) + DBASE(coefs[len - 1 - i]);
        r = BASE(tmp % DBASE(num));
        i++;
    }
    BigNumber res;
    res.coefs.clear();
    res.coefs.push_back(r);
    return res;
}

BigNumber& BigNumber::operator%= (const BASE& num)
{
    *this = *this % num;
    return *this;
}


// --------------------- end of BASE-operand methods --------------------
// --------------------- BigNumber-operand methods --------------------

BigNumber BigNumber::operator+ (const BigNumber& other) {

    if (coefs.empty() || (coefs.size()==1 && coefs[0]==0)) return other;
    if (other.coefs.empty() || (other.coefs.size()==1 && other.coefs[0]==0)) return *this;
    

    BigNumber res(0);
    DBASE carry = 0;
    size_t max_len = max(coefs.size(), other.coefs.size());

    for (size_t i = 0; i < max_len; i++)
    {
        DBASE a_val = (i < coefs.size()) ? DBASE(coefs[i]) : 0;
        DBASE b_val = (i < other.coefs.size()) ? DBASE(other.coefs[i]) : 0;
        
        DBASE tmp = a_val + b_val + carry;
        res.coefs.push_back(BASE(tmp));
        carry = tmp >> BASE_SIZE;
    }

    if (carry != 0) res.coefs.push_back(BASE(carry));
    int len = res.coefs.size();
    while (len > 1 && res.coefs[len - 1] == 0) {
        res.coefs.pop_back();
        len--;
    }

    return res;
}

BigNumber& BigNumber::operator+= (const BigNumber& other){
    *this = *this + other;
    return *this;
}

BigNumber BigNumber::operator- (const BigNumber& other)
{
    // small - BIG (!!!!)
    if (*this < other) throw std::underflow_error("Error: Cannot subtract larger BigNumber");
    // 1 - 1 = 0
    if (*this == other) return BigNumber();

    BigNumber res(0);
    DBASE borrow = 0;
    size_t max_len = coefs.size();

    for (size_t i = 0; i < max_len; i++)
    {
        DBASE a_val = DBASE(coefs[i]);
        DBASE b_val = (i < other.coefs.size()) ? DBASE(other.coefs[i]) : 0;
        
        DBASE tmp = a_val + BASENUM - b_val - borrow;
        res.coefs.push_back(BASE(tmp));
        borrow = 1 - (tmp >> BASE_SIZE);
    }

    // zero delete
    while (res.coefs.size() > 1 && res.coefs.back() == 0) res.coefs.pop_back();
    return res;
}

BigNumber& BigNumber::operator-= (const BigNumber& other){
    *this = *this - other;
    return *this;
}


BigNumber BigNumber::operator* (const BigNumber& other)
{
    // 0 * any = any * 0 = 0
    if ((coefs.empty() || (coefs.size()==1 && coefs[0]==0)) || (other.coefs.empty() || (other.coefs.size()==1 && other.coefs[0]==0))) return BigNumber();

    int len_l = coefs.size();
    int len_r = other.coefs.size();
    int res_len = len_l + len_r;

    BigNumber res;
    res.coefs.resize(res_len, 0);

    DBASE tmp;
    BASE carry = 0;

    for (int j = 0; j < len_r; j++)
    {
        if (other.coefs[j] != 0)
        {
            carry = 0;
            for (int i = 0; i < len_l; i++)
            {
                tmp = DBASE(coefs[i]) * DBASE(other.coefs[j]) + DBASE(res.coefs[i + j]) + DBASE(carry);
                res.coefs[i + j] = BASE(tmp);
                carry = BASE(tmp >> BASE_SIZE);
            }
            res.coefs[j + len_l] = carry;
        }
    }


    while (res.coefs.size() > 1 && res.coefs.back() == 0) res.coefs.pop_back();
    return res;
}


BigNumber& BigNumber::operator*= (const BigNumber& other)
{
    *this = *this * other;
    return *this;
}



BigNumber BigNumber::operator/ (const BigNumber& other)
{

    BigNumber zeroNum;
    BigNumber u(*this);
    BigNumber v(other);

    int sizeL = this->coefs.size();
    int sizeR = other.coefs.size();


    if (sizeR == 0) {
        cout << "Error: in operator/ (other / 0)!\n";
        exit(-3);
    }
    if (sizeL == 0) return zeroNum;    // 0 / v = 0
    if (sizeR == 1)                      // деление на одну цифру (оптимизация)
        return *this / other.coefs[0];
    if (*this < other) return zeroNum;   // если делимое < делитель -> 0
    if (*this == other) {                // если равны -> 1
        BigNumber one;
        one.coefs.clear();
        one.coefs.push_back(1);
        return one;
    }


    const DBASE b = BASENUM; // ((DBASE)1 << BASE_SIZE)

    int n = sizeR;            // длина делителя v (число "цифр")
    int m = sizeL - sizeR;    // длина частного минус 1 (индекс старшей позиции)


    BASE d = BASE(b / (DBASE(v.coefs.back()) + DBASE(1)));


    u = u * d;
    v = v * d;


    if (u.coefs.size() == sizeL) u.coefs.push_back(0);


    BigNumber q;
    q.coefs.resize(m + 1, 0);


    for (int i = m; i >= 0; --i)
    {

        while (i + n >= (int)u.coefs.size()) u.coefs.push_back(0);

        // Пробное частное q_hat: берем две старшие цифры u_{i+n} и u_{i+n-1}
        DBASE u2 = DBASE(u.coefs[i + n]);
        DBASE u1 = DBASE(u.coefs[i + n - 1]);
        DBASE u0 = DBASE(u.coefs[i + n - 2]); // пригодится в проверке
        DBASE v1 = DBASE(v.coefs[n - 1]);
        DBASE v2 = DBASE(v.coefs[n - 2]);

        DBASE qhat = ( (u2 << BASE_SIZE) + u1 ) / v1;
        DBASE rhat = ( (u2 << BASE_SIZE) + u1 ) % v1;


        while (qhat == b || (qhat * v2) > ((rhat << BASE_SIZE) + u0)) {
            qhat--;
            rhat += v1;
            if (rhat >= b) break; // защита, если rhat превысил базу
        }


        BigNumber t = v * (BASE)qhat;
        t = t.Shift(i);


        if (u < t) {
            qhat--;
            t = v * (BASE)qhat;
            t = t.Shift(i);

            while (u < t) {
                qhat--;
                t = v * (BASE)qhat;
                t = t.Shift(i);
            }
        }

        u = u - t;
        q.coefs[i] = (BASE)qhat;
    }



    while (q.coefs.size() > 1 && q.coefs.back() == 0) q.coefs.pop_back();

    return q;
}


// ----------  inout  ------------


void BigNumber::InputHex()
{
    cout << "Input (in HEX format): ";
    string numInHex;
    cin >> numInHex;
    *this = BigNumber(numInHex);

}

void BigNumber::OutputHex(){
    int len = coefs.size();
    if (len == 0) { cout << "0\n"; return; }
    std::cout << std::hex << (unsigned int)coefs[len-1];
    for (int i=len-2; i>=0; i--){
        std::cout.width(BASE_SIZE/4);
        std::cout.fill('0');
        std::cout << std::hex << (unsigned int)coefs[i];
    }
    std::cout << std::dec << endl; // restore decimal mode for later output
}

// ---------- PrintBase256  ------------

void BigNumber::PrintBase256()
    {
        cout << "Base-256: ";
        for (size_t i = 0; i < coefs.size(); ++i) {
            cout << static_cast<unsigned int>(coefs[i]);
            if (i < coefs.size() - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }


// print
ostream& operator<< (ostream& out, const BigNumber& other){
    string output;
    BASE base_10 = 10;
    BigNumber number(other);
    BigNumber zeroNum;

    while (number != zeroNum){
        BASE tmp = (number % (BASE)base_10).coefs[0];
        output += (tmp + '0'); // ASSCI TRANS
        number /= (BASE)base_10;
    }
    if (!output.size()) output = "0";
    reverse(output.begin(), output.end());
    cout << output << endl;
    return out;
}

//get
istream& operator>> (istream& in, BigNumber& other){
    string input_str;
    cin >> input_str;

    for (char c : input_str) if (!isdigit(c)) throw invalid_argument("Input error: Non-digit character found!");
    other = BigNumber(); 
    for (char c : input_str) {
        BASE digit = c - '0';
        other = other * (BASE)10 + digit;
    }
    while (other.coefs.size() > 1 && other.coefs.back() == 0) other.coefs.pop_back();
    return in;
}


int main()
{
    srand(time(nullptr));

    BigNumber a(5);
    a.PrintBase256();

    string hexNum = "1A3F9B";
    BigNumber b(hexNum);
    b.PrintBase256();

    BigNumber c = b;
    c.PrintBase256();

    cout << "Length of a: " << a.getLength() << endl;
    cout << "Length of b: " << b.getLength() << endl;
    cout << "Length of c: " << c.getLength() << endl;

    // ---- Tests for comparison operators and assignment ----
    cout << "\n--- Comparison & Assignment Tests ---\n";

    cout << "b == c : " << (b == c ? "true" : "false") << endl;
    cout << "b != c : " << (b != c ? "true" : "false") << endl;
    cout << "b >= c : " << (b >= c ? "true" : "false") << endl;
    cout << "b <= c : " << (b <= c ? "true" : "false") << endl;

    cout << "a == b : " << (a == b ? "true" : "false") << endl;
    cout << "a != b : " << (a != b ? "true" : "false") << endl;
    cout << "a > b  : " << (a > b ? "true" : "false") << endl;
    cout << "a < b  : " << (a < b ? "true" : "false") << endl;

    // Test assignment operator
    BigNumber d;
    cout << "\nBefore assignment d: ";
    d.PrintBase256();
    d = a;
    cout << "After d = a, d: ";
    d.PrintBase256();
    cout << "d == a : " << (d == a ? "true" : "false") << endl;

    // Get test number from user
    BASE testNum = rand();
    cout << "Testing with number: " << (unsigned int)testNum << endl;

    // ---- Tests for arithmetic operations with BASE ---
    cout << "\n--- Arithmetic Operations with BASE Tests ---\n";

    // Test addition
    cout << "\n--- Addition with BASE ---\n";
    cout << "a = "; a.OutputHex();
    cout << "testNum = " << (unsigned int)testNum << endl;
    BigNumber add_result = a + testNum;
    cout << "a + testNum = "; add_result.OutputHex();

    // Test subtraction
    cout << "\n--- Subtraction with BASE ---\n";
    BigNumber sub_result = add_result - testNum;
    cout << "(a + testNum) - testNum = "; sub_result.OutputHex();
    cout << "a == sub_result : " << (sub_result == a ? "true" : "false") << endl;

    // Test multiplication
    cout << "\n--- Multiplication with BASE ---\n";
    BigNumber mul_result = a * testNum;
    cout << "a * testNum = "; mul_result.OutputHex();

    // Test division with BASE
    cout << "\n--- Division with BASE ---\n";
    BigNumber div_result = mul_result / testNum;
    cout << "(a * testNum) / testNum = "; div_result.OutputHex();
    cout << "a == div_result : " << (div_result == a ? "true" : "false") << endl;

    // Test modulo with BASE
    cout << "\n--- Modulo with BASE ---\n";
    BigNumber mod_result = mul_result % testNum;
    cout << "(a * testNum) % testNum = "; mod_result.OutputHex();
    cout << "mod_result == 0 : " << (mod_result == BigNumber() ? "true" : "false") << endl;

    // ---- Tests for BigNumber arithmetic operations ---
    cout << "\n--- BigNumber Arithmetic Operations Tests ---\n";

    // Test BigNumber addition
    b = BigNumber(4);
    cout << "\n--- BigNumber Addition ---\n";
    cout << "a = "; a.OutputHex();
    cout << "b = "; b.OutputHex();
    BigNumber big_add = a + b;
    cout << "a + b = "; big_add.OutputHex();

    // Test BigNumber subtraction
    cout << "\n--- BigNumber Subtraction ---\n";

    BigNumber big_sub = big_add - a;
    cout << "(a + b) - a = "; big_sub.OutputHex();
    cout << "b == big_sub : " << (big_sub == b ? "true" : "false") << endl;


    // Test BigNumber multiplication
    cout << "\n--- BigNumber Multiplication ---\n";
    BigNumber big_mul = a * b;
    cout << "a * b = "; big_mul.OutputHex();

    // Test compound operations
    cout << "\n--- Compound Operations Tests ---\n";
    
/*    // Test +=
    BigNumber e = a;
    cout << "e = a = "; e.OutputHex();
    e += b;
    cout << "After e += b: "; e.OutputHex();

    // Test -=
    e -= b;
    cout << "After e -= b: "; e.OutputHex();
    cout << "e == a : " << (e == a ? "true" : "false") << endl;

    // Test *=
    BigNumber f = a;
    cout << "f = a = "; f.OutputHex();
    cout << "Before f *= testNum: "; f.OutputHex();
    f *= testNum;
    cout << "After f *= testNum: "; f.OutputHex();
*/
    // Test edge cases
    cout << "\n--- Edge Cases Tests ---\n";
    
    // Test with zero
    BigNumber zero;
    BigNumber add_zero = a + zero;
    cout << "a + zero = "; add_zero.OutputHex();
    cout << "a == (a + zero) : " << (a == add_zero ? "true" : "false") << endl;

    // Test subtraction to zero
    BigNumber sub_self = a - a;
    cout << "a - a = "; sub_self.OutputHex();
    cout << "Is result zero? " << (sub_self == zero ? "true" : "false") << endl;

    // Test multiplication by zero
    BigNumber mul_zero = a * zero;
    cout << "a * zero = "; mul_zero.OutputHex();
    cout << "Is result zero? " << (mul_zero == zero ? "true" : "false") << endl;

    // Test multiplication by one
    BigNumber one;
    one = one + 1; // إنشاء العدد 1
    BigNumber mul_one = a * one;
    cout << "a * one = "; mul_one.OutputHex();
    cout << "a == (a * one) : " << (a == mul_one ? "true" : "false") << endl;

    // ---- Simple User Input/Output Tests ---
    cout << "\n--- >> << ---\n";
    
    // Test user input
    BigNumber num1, num2;
    
    cout << "1st number: ";
    cin >> user_num1;
    
    cout << "2nd number: ";
    cin >> user_num2;
    
    // Display results
    cout << "First number: " << user_num1 << endl;
    cout << "Second number: " << user_num2 << endl;
    
    // Basic operations
    cout << "Sum: " << user_num1 + user_num2 << endl;
    cout << "Difference: " << user_num1 - user_num2 << endl;
    
    return 0;
}
