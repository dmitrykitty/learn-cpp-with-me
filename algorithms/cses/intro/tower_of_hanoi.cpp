#include <bits/stdc++.h>

using namespace std;

void move_disk(int start, int end) {
    cout << start << " " << end << '\n';
}

void hanoi_tower(int n, int start, int end) {
    if(n == 0) {
        return; 
    }

    int tmp = 6 - (start + end); //number of placeholder tower
    
    hanoi_tower(n - 1, start, tmp);
    move_disk(start, end);
    hanoi_tower(n - 1, tmp, end);
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int n;
    cin >> n; //number of the disks
    cout << ((1 << n) - 1) << '\n'; 
    
    hanoi_tower(n, 1, 3);
}
