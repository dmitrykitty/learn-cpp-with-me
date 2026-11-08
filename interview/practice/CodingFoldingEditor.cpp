#include <cstdio>
#include <stdexcept>
#include <vector>
#include <utility>
#include <iostream>
#include <stack>
#include <cassert>

using namespace std; 

struct BlockData {
    int close_line; 
    bool folded; 
    int parent_open_line; 
}; 

class CodeEditor {
    vector<string> code; 
    vector<BlockData> line_info; 
    vector<pair<int, int>> blocks;
     
public: 
    CodeEditor(vector<string> cd): 
        code(std::move(cd)), line_info(code.size(), {-1, false, -1}), blocks(get_blocks())  {}

    const vector<pair<int, int>>& get_code_blocks() const {
        return blocks;
    }

    void fold(int line) {
        set_fold_value(line, true);
    }

    void unfold(int line) {
        set_fold_value(line, false);
    }



    vector<string> render() const {
        vector<string> projection; 
        for(int line = 0; line < code.size(); ++line) {
            if(line_info[line].folded) {
                projection.push_back(code[line] + " ..."); 
                line = line_info[line].close_line; 
            } else {
                projection.push_back(code[line]); 
            }
        }
        return projection; 
    }

private:
    vector<pair<int, int>> get_blocks() {
        stack<int> braces; 

        for(int i = 0; i < code.size(); ++i) {
            for(char ch : code[i]) {
                if(ch == '{') {
                    if(!braces.empty()) {
                        line_info[i].parent_open_line = braces.top(); 
                    }
                    braces.push(i);
                    break;
                } else if(ch == '}') {
                    assert(!braces.empty());
                    int open_idx = braces.top(); 
                    braces.pop(); 
                    line_info[open_idx].close_line = i; 
                    break;
                }
            }
        }
        assert(braces.empty()); 

        vector<pair<int, int>> pairs; 
        for(int i = 0; i < line_info.size(); ++i) {
            if(line_info[i].close_line != -1) {
                pairs.push_back({i, line_info[i].close_line}); 
            }
        }

        return pairs; 
    }

    void set_fold_value(int line, bool value) {
        if(line < 0 || line >= code.size()) {
            throw invalid_argument("invalid line number");
        }

        auto& block_data = line_info[line]; 

        if(block_data.close_line == -1) {
            throw invalid_argument("no block started in line");
        }

        if(is_hidden(line)) {
          throw invalid_argument("block is folded: operation is unvailable");  
        }

        block_data.folded = value; 
    }

    bool is_hidden(int line) const {
        for(int p = line_info[line].parent_open_line; p != -1; p = line_info[p].parent_open_line) {
            if(line_info[p].folded) {
                return true;
            }
        }
        return false;
    }
};

void print(const vector<string>& rendered) {
    for(int i = 0; i < rendered.size(); ++i) {
        cout << rendered[i] << '\n'; 
    }
}

void compare_vectors(const vector<string>& expected, const vector<string>& real) {
    assert(expected.size() == real.size()); 
    for(int i = 0; i < expected.size(); ++i) {
        assert(expected[i] == real[i]); 
    }
}


int main() {
    vector<string> code1 = {
        "{ ", "{", "};", "{", "{", "}//hello", "  }", "};"
    };
    CodeEditor ce(std::move(code1));

    auto blocks = ce.get_code_blocks(); 

    assert(blocks.size() == 4); 
    assert(blocks[0].first == 0 && blocks[0].second == 7); 
    assert(blocks[1].first == 1 && blocks[1].second == 2); 
    assert(blocks[2].first == 3 && blocks[2].second == 6); 
    assert(blocks[3].first == 4 && blocks[3].second == 5);
    
    vector<string> code2 = {
        "namespace app {", " int counter;", "void tick() {", "counter++;", "}", "}", "  class Logger {", "void log() {;", "}", "};"
    };

    CodeEditor ce2(std::move(code2));
    blocks = ce2.get_code_blocks(); 

    assert(blocks.size() == 4); 
    assert(blocks[0].first == 0 && blocks[0].second == 5); 
    assert(blocks[1].first == 2 && blocks[1].second == 4); 
    assert(blocks[2].first == 6 && blocks[2].second == 9); 
    assert(blocks[3].first == 7 && blocks[3].second == 8);


    //empty
    blocks = CodeEditor({}).get_code_blocks(); 
    assert(blocks.empty()); 

    //no braces
    vector<string> code3 = {
        " int counter;", "void tick(); "
    };

    CodeEditor ce3(std::move(code3));
    blocks = ce3.get_code_blocks(); 
    assert(blocks.empty()); 

    vector<string> code4 = {
        "int x;", "void f() {", "g();", "}", "void h() {", "  k();", "}", "int y;"
    };
    CodeEditor ce4(code4); 
    auto rendered = ce4.render(); 
    
    compare_vectors(code4, rendered); 

    //throw if line incorrect
    bool thrown = false; 
    try {
        ce4.fold(-1);
    } catch(...) {
        thrown = true; 
    }

    assert(thrown);
    
    thrown = false; 
    try {
        ce4.fold(20);
    } catch(...) {
        thrown = true; 
    }

    assert(thrown);

    ce4.fold(1); 
    vector<string> target = {"int x;", "void f() { ...", "void h() {", "  k();", "}", "int y;"};
    compare_vectors(target, ce4.render()); 


    ce4.unfold(1); 
    compare_vectors(code4, ce4.render()); 

    //nested foldings test
    vector<string> code5 = {
        " class A {", 
        "   void f() {", 
        "     g();",
        "   }",
        "   void h() {",
        "     k();",
        "   }",
        " }"
    };
    CodeEditor ce5(code5); 
    print(code5); 
    cout << "--------------------\n";
    ce5.fold(1); 
    vector<string> fold1 = {
        " class A {", 
        "   void f() { ...", 
        "   void h() {",
        "     k();",
        "   }",
        " }"
    };
    compare_vectors(fold1, ce5.render()); 



    ce5.fold(0); 
    vector<string> fold0 = {
        " class A { ...", 
    };
    compare_vectors(fold0, ce5.render()); 

    //now now block available for fold / unfold except 0
    thrown = false;
    try {
        ce5.fold(1);
    } catch(...) {
        thrown = true; 
    }
    assert(thrown);

    thrown = false;
    try {
        ce5.fold(2);
    } catch(...) {
        thrown = true; 
    }
    assert(thrown);

    thrown = false;
    try {
        ce5.unfold(1);
    } catch(...) {
        thrown = true; 
    }
    assert(thrown); 

    ce5.unfold(0); 
    compare_vectors(fold1, ce5.render()); 

    ce5.unfold(1); 
    compare_vectors(code5, ce5.render()); 
}