/*
Natalia is a coordinator at a technical university in Kraków, and she is preparing the study plan for a new bachelor programme. The programme has n courses, numbered from 0 to n - 1. Some courses can only be taken after others: for example, “Algorithms II” requires “Algorithms I” to be completed first, and “Operating Systems” might require both “Computer Architecture” and “C Programming”. Natalia has a list of such rules. Each rule is a pair [a, b], which means “course b must be completed before course a can be taken”.

A course can have several prerequisites, and a course can be the prerequisite of several others. Some courses have no rules at all. A student may take as many courses as they like in the same semester, as long as all the prerequisites of each course were completed in an earlier semester, and the programme has no limit on the number of semesters. So the only question is whether the plan is logically possible. Before the plan goes to print, the dean wants to know whether a student could complete all n courses in some order that respects every rule.

You’re given n and the list of rules, and you return true if all courses can be completed and false otherwise. There are at most 2000 courses and at most 5000 rules, and no rule is listed twice.
*/

//state[i] -> {0, 1, 2} 0 - unvisited, 1 - in process, 2-black - visited

#include <bits/stdc++.h>

bool detect_cycle_with_dfs(int cur_course, const std::vector<std::vector<int>>& adj, std::vector<int>& state) {
    state[cur_course] = 1; 
    for(int next_course: adj[cur_course]) {
        if(state[next_course] == 1) {
            return true;
        }
        if(state[next_course] == 0 && detect_cycle_with_dfs(next_course, adj, state)) {
            return true; 
        }
    }
    state[cur_course] = 2;
    return false; 
}

bool can_finish_all(int num_courses, const std::vector<std::vector<int>>& prerequisites) {
    std::vector<std::vector<int>> adj(num_courses); 

    for(const auto& pair: prerequisites) {
        adj[pair[0]].push_back(pair[1]); 
    }

    std::vector<int> state(num_courses, 0);
    
    for(int course = 0; course < num_courses; course++) {
        if(state[course] == 0) {
            if(detect_cycle_with_dfs(course, adj, state)) {
                return false;
            }
        }
    }
    return true; 
}