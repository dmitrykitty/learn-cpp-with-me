
#include <bits/stdc++.h>
#include <optional>
#include <unordered_map>
#include <vector>

using namespace std; 

struct Event { 
    long long timestamp; 
    string code; 
    int value; 
}; 

struct Row {
    long long time; 
    vector<optional<int>> cells; 
}; 

struct Table {
    vector<string> codes; 
    vector<Row> rows; 
}; 

class DataParser {
    Table table; 
    unordered_map<string, int> name_to_id;  
    
public: 
    DataParser(vector<string> codes, vector<Event> events): table{std::move(codes), {}} {
        sort(table.codes.begin(), table.codes.end()); 
        table.codes.erase(unique(table.codes.begin(), table.codes.end()), table.codes.end());
        const size_t TABLE_WIDTH = table.codes.size();  

        for(int i = 0; i < TABLE_WIDTH; ++i) {
            name_to_id[table.codes[i]] = i; 
        }

        assert(table.codes.size() == name_to_id.size()); 

        vector<bool> already_updated(TABLE_WIDTH, false); 

        for(const auto& event: events) {
            auto it = name_to_id.find(event.code); 
            if(it == name_to_id.end()) continue;

            
            if(table.rows.empty() || table.rows.back().time != event.timestamp) {
                already_updated.assign(TABLE_WIDTH, false); 
                Row row{
                    .time = event.timestamp,
                    .cells = table.rows.empty() ? 
                        vector<optional<int>>(TABLE_WIDTH, nullopt) :
                        table.rows.back().cells
                };
                row.cells[it->second] = event.value; 
                already_updated[it->second] = true; 
                table.rows.push_back(std::move(row)); 
            } else {
                if(!already_updated[it->second]) {
                    table.rows.back().cells[it->second] = event.value;
                    already_updated[it->second] = true;
                }
            }
        }
    }

    const Table& get_table() const { 
        return table;  
    }
};


// ---- test helpers ----
Row R(long long t, vector<optional<int>> cells) { return Row{t, std::move(cells)}; }

void expect_rows(const vector<Row>& got, const vector<Row>& want) {
    assert(got.size() == want.size());
    for (size_t i = 0; i < want.size(); ++i) {
        assert(got[i].time == want[i].time);
        assert(got[i].cells == want[i].cells);   // optional<int> compares value and emptiness
    }
}

int main() {
    // 1. Statement example: unsorted codes, sparse rows, values differ per column
    {
        DataParser p({"LAST", "BID", "ASK"},
                     {{1, "BID", 100}, {1, "ASK", 102}, {3, "LAST", 101},
                      {4, "ASK", 103}, {4, "BID", 99}});
        Table t = p.get_table();
        assert((t.codes == vector<string>{"ASK", "BID", "LAST"}));
        expect_rows(t.rows, {R(1, {102, 100, nullopt}),
                             R(3, {nullopt, nullopt, 101}),
                             R(4, {103, 99, nullopt})});
        puts("ok");
    }

    // 2. Duplicate codes in the list are removed
    {
        DataParser p({"B", "A", "B"}, {{1, "B", 7}});
        Table t = p.get_table();
        assert((t.codes == vector<string>{"A", "B"}));
        expect_rows(t.rows, {R(1, {nullopt, 7})});
        puts("ok");
    }

    // 3. Unknown code skipped, other events at the same time kept
    {
        DataParser p({"A", "B"}, {{1, "X", 9}, {1, "B", 5}});
        expect_rows(p.get_table().rows, {R(1, {nullopt, 5})});
        puts("ok");
    }

    // 4. A timestamp with only unknown codes produces no row
    {
        DataParser p({"A", "B"}, {{1, "A", 1}, {2, "X", 2}, {3, "B", 3}});
        expect_rows(p.get_table().rows, {R(1, {1, nullopt}), R(3, {nullopt, 3})});
        puts("ok");
    }

    // 5. Unknown code first at a new timestamp, then a valid one: one row, not two
    {
        DataParser p({"A"}, {{1, "A", 1}, {2, "X", 2}, {2, "A", 4}});
        expect_rows(p.get_table().rows, {R(1, {1}), R(2, {4})});
        puts("ok");
    }

    // 6. Duplicate (time, code): the first value wins
    {
        DataParser p({"A", "B"}, {{1, "A", 5}, {1, "A", 7}});
        expect_rows(p.get_table().rows, {R(1, {5, nullopt})});
        puts("ok");
    }

    // 7. Zero and negative values are real values, not empty
    {
        DataParser p({"A", "B"}, {{1, "A", 0}, {1, "B", -3}});
        expect_rows(p.get_table().rows, {R(1, {0, -3})});
        puts("ok");
    }

    // 8. No events: sorted header, no rows
    {
        DataParser p({"Z", "A"}, {});
        Table t = p.get_table();
        assert((t.codes == vector<string>{"A", "Z"}));
        assert(t.rows.empty());
        puts("ok");
    }

    // 9. Only unknown events: no rows
    {
        DataParser p({"A"}, {{1, "X", 1}, {2, "Y", 2}});
        assert(p.get_table().rows.empty());
        puts("ok");
    }

    // 10. Large timestamps survive (fails if time is stored as int)
    {
        DataParser p({"A"}, {{1'000'000'000'000LL, "A", 1}});
        expect_rows(p.get_table().rows, {R(1'000'000'000'000LL, {1})});
        puts("ok");
    }

    // 11. getTable() twice gives the same result (no hidden mutation)
    {
        DataParser p({"B", "A"}, {{1, "A", 1}, {2, "B", 2}});
        Table t1 = p.get_table(), t2 = p.get_table();
        assert(t1.codes == t2.codes);
        expect_rows(t1.rows, t2.rows);
        puts("ok");
    }

    puts("all tests passed");
}