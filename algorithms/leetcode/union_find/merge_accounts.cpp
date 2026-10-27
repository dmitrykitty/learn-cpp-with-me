/*
A company runs several products, and every product keeps its own user accounts. 
You get a list of accounts. Each account is a list whose first entry is the user’s name, 
and the rest are the email addresses attached to that account.

Marketing wants to know how many real people they have. Two accounts belong to the same person 
if they share at least one email address, and this holds transitively: 
if A shares an email with B, and B shares one with C, then A, B and C are one person. 
Two accounts can have the same name and still be different people if they share no email.

Produce one merged account per real person. Each merged account starts with the name, 
followed by that person’s unique emails in sorted order. The merged accounts can be returned in any order.

Restate the problem in your own words, ask your clarifying questions, and walk me through your approach before you code. 
Then write the C++, dry-run it on an example, and state time, auxiliary space, output space and recursion depth.
*/



//list account[0] - username -> account[1..n] - emails
//two accounts belong two the same person if emails1 intersect emails2 != 0 

#include <bits/stdc++.h>

using namespace std; 

    struct DisjoinSet {
        vector<int> parent; 
        vector<int> size; 

        DisjoinSet(int number): parent(number), size(number, 1){
            for(int i = 0; i < number; i++) {
                parent[i] = i;
            }
        }

        int find(int x) {
            while(x != parent[x]) {
                parent[x] = parent[parent[x]];
                x = parent[x];
            }
            return x; 
        }

        void merge(int a, int b) {
            a = find(a);
            b = find(b);
            
            if(a == b) return; 
            
            if(size[a] < size[b]) swap(a, b); 

            parent[b] = a; 
            size[a] += size[b];
        }
    };

    vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
        unordered_map<string, int> email_to_id; 
        vector<string> id_to_email; 
        vector<string> id_to_user; 
        int id = 0;

        for(const auto& account: accounts) {
            for(int i = 1; i < account.size(); ++i) {
                auto [it, inserted] = email_to_id.try_emplace(account[i]);
                if(inserted) {
                    it->second = id; 
                    id_to_email.push_back(account[i]); 
                    id_to_user.push_back(account[0]);
                    id++;
                } 
            }
        }

        DisjoinSet dsu(email_to_id.size()); 
        for(const auto& account: accounts) {
            int first = email_to_id[account[1]]; 
            for(int i = 2; i < account.size(); ++i) {
                dsu.merge(first, email_to_id[account[i]]); 
            }
        }

        unordered_map<int, vector<string>> grouped_emails; 
        for(int i = 0; i < email_to_id.size(); i++) {
            grouped_emails[dsu.find(i)].push_back(id_to_email[i]); 
        }

        vector<vector<string>> result; 
        for(auto& [key, emails] : grouped_emails) {
            vector<string> user_emails = {id_to_user[key]}; 
            sort(emails.begin(), emails.end());
            user_emails.insert(user_emails.end(), emails.begin(), emails.end());
            result.push_back(user_emails);
        }
        return result;  
    }