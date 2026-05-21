#include <bits/stdc++.h>

using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    string s; 
    vector<int> v(26, 0); 

    cin >> s; 

    int odd_idx = -1;

    for(int i = 0; i < s.size(); ++i){
        v[s[i] - 'A']++; 
    }

    for(int i = 0; i < v.size(); ++i){
        if(v[i] & 1){
            if(odd_idx != -1){
                cout << "NO SOLUTION";
                return 0; 
            }
            odd_idx = i;
        }
    }

    string left; 
    left.reserve(s.size() / 2);

    for(int i = 0; i < 26; ++i){
        left.append(v[i] / 2, char('A' + i)); 
    }

    string res = left; 

    if(odd_idx != -1){
        res.push_back(char('A' + odd_idx));
    }

    reverse(left.begin(), left.end()); 
    res += left;
     
    cout << res;
}