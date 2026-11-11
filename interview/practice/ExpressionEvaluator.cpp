#include <stdexcept>
#include <string> 
#include <cassert>
#include <stack>
#include <unordered_map>

using namespace std; 

struct State {
    long long result{0}; 
    long long term{0}; 
    char op{'+'}; 
}; 

long long apply_op(long long a, long long b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b; 
        case '*': return a * b;
        case '/': 
            if(b == 0) {
                throw logic_error("division by zero"); 
            }
            return a / b; 
        default: throw logic_error("unknown operator");
    }
}

int precedence(char ch) {
    if(ch == '+' || ch == '-') {
        return 1; 
    }
    return 2; 
}

void apply_value(State& state, long long num) {
    if(precedence(state.op) == 1) {
        state.result += state.term; 
        state.term = 0; 
    } 
    state.term = apply_op(state.term, num, state.op);
}

bool is_digit(char ch) {
    return ('0' <= ch && ch <= '9');
}

bool is_variable_start(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_'; 
}

bool is_next_variable_char(char ch) {
    return is_variable_start(ch) || is_digit(ch); 
}

int digit_of(char ch) {
    return (int)(ch - '0'); 
}

long long evaluate(const string& expr, const unordered_map<string, long long>& vars = {}) {
    stack<State> st; 
    int i = 0, n = expr.size(); 
    State cur{.result = 0, .term = 0, .op = '+'}; 

    while(i < n) {
        if(expr[i] == ' ') {
            i++; 
            continue;
        } else if(is_digit(expr[i])) {
            long long num = 0; 
            while(i < n && is_digit(expr[i])) {
                num = num * 10 + (long long)digit_of(expr[i]); 
                i++;
            }
            apply_value(cur, num);
        } else if(expr[i] == '(') {
            st.push(cur); 
            cur = State{}; 
            i++;
        } else if(expr[i] == ')') {
            long long parethness_result = cur.result + cur.term; 
            cur = st.top(); st.pop();
            apply_value(cur, parethness_result);  
            i++;
        } else if(is_variable_start(expr[i])) {
            int start = i; 
            while(i < n && is_next_variable_char(expr[i])) {
                i++; 
            }
            string var = expr.substr(start, i - start);
            auto it = vars.find(var); 
            if(it == vars.end()) {
                throw invalid_argument("unknown variable name: " + var); 
            }
            apply_value(cur, it->second); 
        } else {
            cur.op = expr[i]; 
            i++; 
        }
    }
    assert(st.empty()); 
    return cur.result + cur.term; 
}

int main() {
    assert(evaluate("") == 0); 
    assert(evaluate("42") == 42); 
    assert(evaluate("12+15") == 27); 
    assert(evaluate("12-15") == -3); 
    assert(evaluate("     23 + 1     - 5") == 19); 
    assert(evaluate("             ") == 0); 
    assert(evaluate("0 - 5 - 5- 5     - 555555") == -555570); 
    assert(evaluate("1 + 2 - 3 + 4 - 5 + 6 - 7 + 9 - 10 + 11 - 12") == -4); 
    assert(evaluate("3000000000 + 1") == 3000000001LL);
    assert(evaluate("20 / 4 - 16 /    8") == 3);
    assert(evaluate("1 + 20 * 2 / 4 - 10") == 1); 
    assert(evaluate("5 * 3 / 8 * 5 / 4 * 9 / 3") == 3); 
    assert(evaluate("153 / 27 * 5 - 125") == -100); 
    assert(evaluate("20 - 6 / 4 * 3") == 17);
    assert(evaluate("0 - 7 / 2") == -3);   // -7 / 2 truncates to -3, not -4
    assert(evaluate("2 + 3 * 4") == 14);
    assert(evaluate("8 / 2 / 2") == 2);    // left to right

    assert(evaluate("((7+3))") == 10); 
    assert(evaluate("1 + (1 + (1 + (1 + 1)))") == 5); 
    assert(evaluate("2 * (5 + 4) / (4 - 13)") == -2);
    assert(evaluate("((7+3) * (5 -7)) * (10-12)") == 40); 
    assert(evaluate("1 * (1 + (1 * 1 / (1 - 2)))") == 0);
    assert(evaluate("(0 + (1) + ((2)) + (((3))) + ((((4))))) * (0-5)") == -50); 
}