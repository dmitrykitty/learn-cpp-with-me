#include <bits/stdc++.h>

using namespace std; 

class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* node, TreeNode* p, TreeNode* q) {
        if(!node) {
            return nullptr; 
        }

        if(node == p || node == q) {
            return node; 
        }

        TreeNode* left = lowestCommonAncestor(node->left, p, q);
        TreeNode* right = lowestCommonAncestor(node->right, p, q);

        //in two different subtrees
        if(left && right) {
            return node; 
        }

        //in one subtree
        return left ? left : right; 
    }
};