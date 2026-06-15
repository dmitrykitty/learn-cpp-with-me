#include <bits/stdc++.h>

using namespace std;

int n; 
vector<string> res; 
vector<int> cnt(26, 0);
string cur; 


void backtrack(){
    if(cur.size() == n){
        res.push_back(cur);
        return;
    }

    for(int i = 0; i < 26; ++i){
        if(cnt[i] == 0){
            continue;
        }

        cur.push_back('a' + i);
        cnt[i]--;

        backtrack();

        cur.pop_back();
        cnt[i]++;
    }
}


int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    string s; 
    cin >> s; 

    n = s.size();

    for(const char ch : s){
        cnt[ch - 'a']++;
    }

    backtrack();

    cout << res.size() << '\n';
    for(const string& s : res){
        cout << s << '\n';
    }
}