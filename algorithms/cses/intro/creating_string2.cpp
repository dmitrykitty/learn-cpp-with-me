#include <bits/stdc++.h>

using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    string s;
    vector<string> res;

    cin >> s;
    sort(s.begin(), s.end());
    
    do {
        res.push_back(s);
    } while(next_permutation(s.begin(), s.end()));

    cout << res.size() << '\n'; 
    for(const string& s: res){
        cout << s << '\n';
    }
}