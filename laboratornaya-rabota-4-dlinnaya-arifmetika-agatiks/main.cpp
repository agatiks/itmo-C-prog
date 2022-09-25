#include <fstream>
#include <stack>
#include <set>
#include <iostream>
#include "LN.h"
using namespace std;
using namespace std::literals;
LN operator ""_ln(const char *str) {
    return LN(str);
}

int main(int argc, char* argv[])
{
    if(argc != 3) {
        printf("Wrong number of arguments.");
        return 1;
    }
    ifstream fin(argv[1]);
    if (!fin.is_open()) {
        printf("Can't find or open file named \"%s\" for reading.", argv[1]);
        return 1;
    }
    ofstream fout(argv[2]);
    if (!fout.is_open()) {
        printf("Can't find or open file named \"%s\" for writing.", argv[2]);
        fin.close();
        return 1;
    }
    const set<string> OPER {"+", "-", "*", "/", "%", "~", "_", "<", "<=", ">", ">=", "==", "!="};
    stack<LN> numbers;
    string str;
    try{
    while (getline(fin, str)) {
        if (OPER.count(str.c_str())) {
            if (str == "~") {
                /*LN a = numbers.top();
                numbers.pop();
                numbers.push(~a);*/
            } else if (str == "_") {
                LN a = numbers.top();
                numbers.pop();
                numbers.push(-a);
            } else if (str.size() == 1) {
                LN b = numbers.top();
                numbers.pop();
                LN a = numbers.top();
                numbers.pop();
                switch(str[0]) {
                case('+'):
                    numbers.push(a + b);
                    break;
                case('-'):
                    numbers.push(a - b);
                    break;
                case('*'):
                    numbers.push(a * b);
                    break;
                case('/'):
                    //numbers.push(a / b);
                    break;
                case('%'):
                    //numbers.push(a % b);
                    break;
                case('<'):
                    numbers.push(LN(a < b));
                case('>'):
                    numbers.push(LN(a > b));
                }
            } else if (str.size() == 2) {
                LN b = numbers.top();
                numbers.pop();
                LN a = numbers.top();
                numbers.pop();
                switch (str[0]) {
                case('>'):
                    numbers.push(LN(a >= b));
                    break;
                case('<'):
                    numbers.push(LN(a <= b));
                    break;
                case('='):
                    numbers.push(LN(a == b));
                    break;
                case('!'):
                    numbers.push(LN(a != b));
                    break;
                }
            }
        } else {
            numbers.push(LN(str));
        }
    }
    } catch (std::exception e) {
        cout << "Memory error" << endl;
        fin.close();
        fout.close();
        return 2;
    } catch (...)//Ловим всё
    {
        cout << "Error";
    } catch (int e) {
        if(e == -1){            
            cout << "Ariphmetical error" <<endl;
            return 2;
        }        
    } catch (std::string message) {
    } 
    fin.close();
    while(!numbers.empty()) {
        fout << numbers.top().print() << endl;
        numbers.pop();
    }
    fout.close();
    return 0;
}

