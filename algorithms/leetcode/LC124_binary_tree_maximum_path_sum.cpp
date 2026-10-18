#include <bits/stdc++.h>

using namespace std; 

class Solution {
    struct BestSum {
        int ext{-1000};
        int completed{-1000} ;
    };
public:
    

    int maxPathSum(TreeNode* root) {
        BestSum sum = maxSum(root);
        return max(sum.completed, sum.ext);
    }


    BestSum maxSum(TreeNode* node) {
        if(!node) {
            return BestSum(); 
        }

        BestSum left = maxSum(node->left);
        BestSum right = maxSum(node->right); 

        BestSum cur; 
        cur.ext = node->val + max(0, max(left.ext, right.ext));

        int path_with_cur_node = node->val + max({0, left.ext, right.ext, right.ext + left.ext});
        cur.completed = max({path_with_cur_node, left.completed, right.completed});

        return cur;
    }
};