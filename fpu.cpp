#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class FPU {
public:
    FPU(): TC(0), PS("0"), PC(0), st(8, 0){}

    string to_754(double n) {
        string res;
        if (n == 0) {
            res = "0|0000000000|000000000000000000000000000000000000000";
        } else if (abs(n) >= (pow(2, 9) - 1)) {
            bool sign = n < 0;
            res = "|1111111111|000000000000000000000000000000000000000";
            res.insert(0, 1, (char) (sign + '0'));
        } else if (isnan(n)){
            res = "0|1111111111|100010000001000100000100001000010000000";
        } else {
            bool sign = n < 0;
            n = abs(n);
            int ch = static_cast<int>(log2(n) + 511);
            if (ch - 511 > log2(n)) {
                ch--;
            }
            int ch_copy = ch;
            string binary = "", mantissa = "";
            for (int i = 0; i < 10; i++) {
                binary = (ch_copy % 2 == 0 ? "0" : "1") + binary;
                ch_copy /= 2;
            }
            n = n / pow(2, ch - 127) - 1;
            for (int i = 0; i < 39; i++) {
                n *= 2;
                if (n >= 1) {
                    mantissa += '1';
                    n -= 1;
                } else {
                    mantissa += '0';
                }
            }
            res = "|" + binary + "|" + mantissa;
            res.insert(0, 1, (char) (sign + '0'));
        }
        return res;
    }


    void Load(double value) {
        // Зсув регістрів на 1 вниз
        for (int i = st.size() - 1; i > 0; --i) {
            st[i] = st[i - 1];
        }
        st[0] = value;

    }

    void Sub() {
        double op2 = st[0];
        double op1 = st[1];
        st[1] = 0;
        st[0] = op1 - op2;
        for (int i = 1; i < st.size() - 1; i++) {
            st[i] = st[i + 1];
        }
    }


    void Add() {
        double op2 = st[0];
        double op1 = st[1];
        st[1] = 0;
        if (op1 == -op2){
            st[0] = 0;
        }
        st[0] = op1 + op2;
        for (int i = 1; i < st.size() - 1; i++) {
            st[i] = st[i + 1];
        }
    }

    void Mult() {
        double op2 = st[0];
        double op1 = st[1];
        st[1] = 0;
        st[0] = op1 * op2;
        for (int i = 1; i < st.size() - 1; i++) {
            st[i] = st[i + 1];
        }
    }

    void Div() {
        double op2 = st[0];
        double op1 = st[1];
        st[1] = 0;

        double result = op1 / op2;
        if (op2 == 0) {
            st[0] = (numeric_limits<double>::infinity());
        } else if (isnan(result)) {
            st[0] = (numeric_limits<double>::quiet_NaN());
        } else {
            st[0] = result;
        }

        for (int i = 1; i < st.size() - 1; i++) {
            st[i] = st[i + 1];
        }
    }

    void display(string com, double val) {

        cout << "**************************************************" << endl;
        cout << com << endl;
        if (com == "Load"){
            cout <<"Operand: " << val << endl;
        }
        cout << "Register:" << endl;
        for (int i = 0; i < 8; i++) {
            cout << "R" << i << ":  " << to_754(st[i]) << endl;
        }
        cout << "TC: " << TC << endl;
        cout << "PC: " << PC << endl;

        cout << "**************************************************" << endl;
    }

    void doCommand(string com, double val) {
        if (com == "Load") {
            Load(val);
        } else if (com == "Add") {
            Add();
        } else if (com == "Sub") {
            Sub();
        } else if (com == "Mult") {
            Mult();
        } else if (com == "Div") {
            Div();
        } else {
            std::cout << "Invalid command: " << com << std::endl;
        }
    }

    void waitForKeyPress() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void mainProg(string com, double val = 0.0) {
        TC = 0;

        waitForKeyPress();
        tact1(com, val);
        waitForKeyPress();
        tact2(com, val);
    }

    void tact1(string com, double val) {
        TC = 1;
        PC += 1;
        display(com, val);
    }

    void tact2(string com, double val) {
        TC = 2;
        doCommand(com, val);
        display(com, val);
    }

private:
    int TC;
    string PS = "0";
    int PC;
    string com;
    vector<double> st;
};

int main(){
    FPU fpu;
    string a, b;
    cout << "Enter a: ";
    cin >> a;
    cout << "Enter b: ";
    cin >> b;

    cout << endl;

    cout << "Task  F(a,b)=(b-a)/(b+4*a)-b/a \n P = 10 bits; M = 39 bits" << endl;

    double A = stod(a);
    double B = stod(b);

    fpu.mainProg("Load", B);
    fpu.mainProg("Load", A);
    fpu.mainProg("Sub");
    fpu.mainProg("Load", A);
    fpu.mainProg("Load", 4);
    fpu.mainProg("Mult");
    fpu.mainProg("Load", B);
    fpu.mainProg("Add");
    fpu.mainProg("Div");
    fpu.mainProg("Load", B);
    fpu.mainProg("Load", A);
    fpu.mainProg("Div");
    fpu.mainProg("Sub");

    return 0;

}

