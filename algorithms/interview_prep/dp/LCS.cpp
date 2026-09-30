#include <iostream> 
#include <algorithm>
#include <vector>

//solve(i, j) = best solution for A[i:] & B[j:]
//relation solve(i, j) = if letters equal move both else max(move left, move right)
int solve_rec(const std::string& A, const std::string& B, int i, int j) {
    if(i == A.size() || j == B.size()) {
        return 0; 
    }

    int res; 
    if(A[i] == B[j]) {
        res = 1 + solve_rec(A, B, i + 1, j + 1); //move both 
    } else {
        res = std::max(
            solve_rec(A, B, i + 1, j), //only left move
            solve_rec(A, B, i, j + 1) //only right move 
        ); 
    }
    return res; 
}

int LCS_rec(const std::string& A, const std::string& B) {
    return solve_rec(A, B, 0, 0);
}

int solve_memo(const std::string& A, const std::string& B, std::vector<std::vector<int>>& memo, int i, int j) {
    if(i == A.size() || j == B.size()) {
        return 0; 
    }

    if(memo[i][j] != -1) {
        return memo[i][j];
    }

    int res; 
    if(A[i] == B[j]) {
        res = 1 + solve_rec(A, B, i + 1, j + 1); //move both 
    } else {
        res = std::max(
            solve_memo(A, B, memo, i + 1, j), //only left move
            solve_memo(A, B, memo, i, j + 1) //only right move 
        ); 
    }
    return memo[i][j] = res; 
}

int LCS_memo(const std::string& A, const std::string& B) {
    std::vector<std::vector<int>> memo(A.size(), std::vector<int>(B.size(), -1)); 
    return solve_memo(A, B, memo, 0, 0);
}

int LCS_dp(const std::string& A, const std::string& B) {
    std::vector<std::vector<int>> dp(A.size() + 1, std::vector<int>(B.size() + 1, 0)); 

    for(int i = A.size() - 1; i >= 0; --i) {
        for(int j = B.size() - 1; j >= 0; --j) {
            dp[i][j] = A[i] == B[j] ? 
                dp[i + 1][j + 1] + 1 : 
                std::max(dp[i + 1][j], dp[i][j + 1]); 
        }
    }
    return dp[0][0]; 
}





int main() {
    std::string A = "ABCDEK"; 
    std::string B = "BECELMK";
    std::cout << LCS_rec(A, B) << " " << LCS_memo(A, B) << " " << LCS_dp(A, B);  
}
