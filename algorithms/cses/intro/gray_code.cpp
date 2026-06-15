#include <bits/stdc++.h>

using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int x; 
    cin >> x; 

    int count = 1 << x; 

    for(int i = 0; i < count; ++i){
        int gray = i ^ (i >> 1);
        for(int j = x - 1; j >= 0; --j){
            if(gray & (1 << j)){
                cout << "1";
            } else {
                cout << "0";
            }
        }
        cout << '\n';
    }
} 