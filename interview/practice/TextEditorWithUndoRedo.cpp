
#include <stdexcept>
#include <string> 
#include <algorithm>
#include <cassert>
#include <iostream>
#include <stack>
#include <optional>

using namespace std; 

struct Edit {
    int start; 
    string removed{}; 
    string inserted{}; 
    bool had_selection{}; 
};

class TextEditor {
    string buffer; 
    stack<Edit> undo_stack; 
    stack<Edit> redo_stack; 
    optional<pair<int, int>> selection; 
    int cursor{0};

public: 
    void insert(const string& text) {
        Edit edit{.start = cursor, .inserted = text};
        fill_from_selection(edit); 
        commit(edit);
    }

    void backspace(int k) {
        validate_input(k);
        //for sure no selection
        if(cursor == 0) {
            return; 
        }
        
        Edit edit; 
        if(!fill_from_selection(edit)) {
            edit.start = max(0, cursor - k); 
            edit.removed = buffer.substr(edit.start, cursor - edit.start); 
        }
        commit(edit); 
    }

    bool undo() {
        if(undo_stack.empty()) {
            return false; 
        }

        Edit edit = undo_stack.top(); undo_stack.pop(); 
        apply(opposite(edit));
        if(edit.had_selection) {
            selection = {edit.start, edit.start + edit.removed.size()};
        } else {
            selection.reset(); 
        }

        redo_stack.push(edit);
        return true; 
    }

    bool redo() {
        if(redo_stack.empty()) {
            return false; 
        }

        Edit edit = redo_stack.top(); redo_stack.pop(); 
        apply(edit); 
        selection.reset(); 

        undo_stack.push(edit); 
        return true;
    }

    bool select(int start, int end) {
        if(start < 0 || start > end || end > buffer.size()) {
            throw invalid_argument("invalid selection"); 
        }

        cursor = end;
        if(start == end) {
            selection.reset();
            return false; 
        }

        selection = {start, end}; 
        return true; 
    }

    void left(int shift) {
        validate_input(shift);
        selection.reset();
        cursor  = max(cursor - shift, 0); 
    }

    void right(int shift) {
        validate_input(shift);
        selection.reset();
        cursor = min((int)buffer.size(), cursor + shift);
    }

    const string& text() const {
        return  buffer; 
    }



private: 
    static void validate_input(int arg) {
        if(arg <= 0) {
            throw invalid_argument("invalid arg: should be non-negative"); 
        }
    }

    void commit(const Edit& edit) {
        if(edit.inserted.empty() && edit.removed.empty()) {
            return; //nothing to change
        }
        apply(edit); 
        selection.reset(); 
        undo_stack.push(edit);
        redo_stack = {};  
    }

    void apply(const Edit& edit) {
        buffer.erase(edit.start, edit.removed.size());   // no-op if removed is empty
        buffer.insert(edit.start, edit.inserted);        // no-op if inserted is empty
        cursor = edit.start + edit.inserted.size();
    }

    bool fill_from_selection(Edit& edit) {
        if(!selection) {
            return false; 
        }
        auto [start, end] = *selection;
        edit.start = start; 
        edit.removed = buffer.substr(start, end - start); 
        edit.had_selection = true;  
        return true; 
    }

    Edit opposite(const Edit& edit) {
        Edit opp = edit; 
        swap(opp.inserted, opp.removed);  
        return opp;
    }
};

template<typename E, typename F>
void expect_error(F f) {
    try {
        f();
    } catch(const E&) {
        return; 
    }
    assert(false && "exception wasn't thrown"); 
}

int main() {
    TextEditor ce1; 

    ce1.insert("hello");
    assert(ce1.text() == string("hello")); 
    ce1.left(2); 
    ce1.insert("XYZ"); 
    assert(ce1.text() == string("helXYZlo")); 
    ce1.backspace(4);
    assert(ce1.text() == string("helo")); 
    ce1.left(10); 
    ce1.insert("!"); 
    assert(ce1.text() == string("!helo")); 
    ce1.right(10); 
    ce1.insert("!"); 
    assert(ce1.text() == string("!helo!")); 

    ce1.backspace(10); 
    assert(ce1.text() == string("")); 
    ce1.insert("hello"); 
    ce1.left(2); 
    ce1.backspace(5); 
    assert(ce1.text() == string("lo")); 

    expect_error<invalid_argument>([&]{ ce1.backspace(-1); });
    expect_error<invalid_argument>([&]{ ce1.left(-1); });
    expect_error<invalid_argument>([&]{ ce1.right(-1); });

    TextEditor te2; 
    te2.insert("abc"); 
    assert(te2.text() == "abc"); 
    te2.left(1); 
    te2.insert("X"); 
    assert(te2.text() == "abXc"); 
    te2.backspace(2); 
    assert(te2.text() == "ac"); 
    te2.undo(); 
    cout << te2.text() << endl; 
    assert(te2.text() == "abXc"); 
    te2.undo(); 
    assert(te2.text() == "abc"); 
    te2.redo();
    assert(te2.text() == "abXc"); 
    te2.insert("!");
    assert(te2.text() == "abX!c"); 

    //check no operation are redo after undo update 
    TextEditor te3; 
    te3.insert("abc"); 
    te3.backspace(1); 
    assert(te3.text() == "ab"); 
    te3.redo(); 
    assert(te3.text() == "ab");
    te3.undo(); 
    assert(te3.text() == "abc");
    te3.redo(); 
    assert(te3.text() == "ab");

    te3.undo(); 
    assert(te3.text() == "abc");
    te3.insert("d"); 
    assert(te3.text() == "abcd");
    te3.redo(); 
    assert(te3.text() == "abcd");

    //equal amount of undo red operation does not change string 
    TextEditor te4; 
    te4.insert("a"); 
    te4.insert("b"); 
    te4.insert("c"); 
    assert(te4.text() == "abc"); 
    te4.left(1); 
    te4.backspace(1); 
    te4.backspace(1); 
    assert(te4.text() == "c"); 

    te4.undo(); 
    te4.undo(); 
    assert(te4.text() == "abc"); 
    te4.undo(); 
    te4.undo(); 
    te4.undo(); 
    assert(te4.text() == ""); 
    te4.redo();
    te4.redo();
    te4.redo();
    assert(te4.text() == "abc"); 
    te4.redo();
    te4.redo();
    assert(te4.text() == "c"); 

    //check empty insert is not saved in undo 
    TextEditor te5; 
    te5.insert("abc"); 
    te5.insert(""); 
    te5.undo(); 
    assert(te5.text() == ""); 

    //undo ad redo correctly save cursor position 
    TextEditor te6; 
    te6.insert("abrakadabra");
    te6.left(3); 
    te6.backspace(7);  
    assert(te6.text() == "abra"); 
    te6.undo(); 
    te6.insert("!"); 
    assert(te6.text() == "abrakada!bra");
    te6.left(3); 
    te6.backspace(5);
    assert(te6.text() == "ada!bra"); 
    te6.undo(); 
    assert(te6.text() == "abrakada!bra"); 
    te6.redo(); 
    te6.insert("!"); 
    assert(te6.text() == "a!da!bra"); 
}