#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <map>

using namespace std;

const regex token_regex("[a-zA-Z][a-zA-Z0-9_]*");
smatch base_match;

char delims[] = {'"', ',', '(', ')', ';'};

bool if_delim(char c){
    bool delim = 0;
    for (int i = 1; i < 5; i++){
        if (c == delims[i]){
            delim = 1;
        }
    }
    return delim;
}

pair<string, bool> get_token(string& cmd){
    string token;
    int i = 0;
    while (isspace(cmd[i])){
        i++;
    } 
    if(if_delim(cmd[i])){
        token = cmd[i];
        cmd.erase(0, i+1);
        return {token, 1};
    }
    if(cmd[i] == delims[0]){
        token += cmd[i];
        for (int j = i+1; j < cmd.size(); j++){
            if(cmd[j] == delims[0]){
                token += cmd[j];
                cmd.erase(0, j+1);
                return {token, 1};
            }
            else token += cmd[j]; 
        }
        cmd.clear();
        return {"expected a \" after: " + token, 0};
    }
    while(1){
        for (int j = i; j < cmd.size(); j++){
            if(if_delim(cmd[j]) || isspace(cmd[j])){
                cmd.erase(0, j);
                if(!regex_match(token, base_match, token_regex))
                    return {"Not a valid symbols.", 0};
                return {token, 1};
            }
            else token += cmd[j]; 
        }
        cmd.clear();
        return {token, 0};
    }
}


stringstream print_result(vector<vector<string>> rows, vector<string> col_names)
{
    stringstream result;
    size_t col_num = col_names.size();
    result << std::left;
    result << '+' << setfill('-');
    for(int i = 0; i < col_num; ++i)
    {
        result << setw(17 + 8) << "" << '+';
    }
    result << std::endl << std::setfill(' ');
    result << '|';
    for(const auto& c : col_names)
    {
        result << "   " << std::setw(17 + 5) << c.substr(0, 17) << '|';
    }
    result << std::endl;
    result << '+' << setfill('-');
    for(int i = 0; i < col_num; ++i)
    {
        result << setw(17 + 8) << "" << '+';
    }
    result << std::endl << std::setfill(' ');
    for(size_t i = 0; i < rows.size(); ++i)
    {
        result << '|';
        for(const auto& v : rows.at(i))
        {
            result << "  \"" << std::setw(17 + 5) 
                << (v.size() < 17 ? v : (v.substr(0, 17 - 3) + "...")) + "\"" << '|';
        }
        result << std::endl; 
    }
    result << '+' << setfill('-');
    for(int i = 0; i < col_num; ++i)
    {
        result << setw(17 + 8) << "" << '+';
    }
    result << std::endl << std::setfill(' ');
    return result;
}

class DataBase
{
    private: 
    class Table
    {
        public:
        vector<vector<string>> data;
        vector<string> columns;
        string name;
        pair<bool, string> check_cond(const vector<string>& row, string cond){
            size_t i = 0; 
            size_t n = cond.length() - 1;
            while (isspace(cond[i]))
                i++;
            if (cond[i] == '('){
                size_t j = i + 1; 
                int prnths = 1;
                while (prnths > 0 && cond[j] != ';') {
                    prnths += (cond[j] == '(');
                    prnths -= (cond[j] == ')');
                    if(prnths > 0)
                        j++;
                }
                auto [boll1, err1] = check_cond(row, cond.substr(i + 1, j - i - 1) + ';');
                if (err1 != "")
                    return {false, err1};
                bool boll2;
                if (cond[j] == ';')
                    return {false, "Expected closing ')'"};
                i = j + 1;
                while (isspace(cond[i]) && cond[i] != ';')
                    i++;
                string comp;

                if (n > i + 5){ 
                    if(cond[i] == 'A' && cond[i+1] == 'N' && cond[i+2] == 'D'){
                        comp = "AND";
                        i = i + 3;
                    }
                    else if(cond[i] == 'O' && cond[i+1] == 'R'){
                        comp = "OR";
                        i = i + 2;
                    }
                    else
                        return {false, "Incorrect logical operator"};
                }
                else
                    return {false, "Incorrect logical operator"};

                while (isspace(cond[i]) && cond[i] != ';')
                    i++;
                if (cond[i] == '('){
                    size_t j = i + 1; 
                    int prnths = 1;
                    while (prnths > 0 && cond[j] != ';') {
                        prnths += (cond[j] == '(');
                        prnths -= (cond[j] == ')');
                        if(prnths > 0)
                            j++;
                    }
                    auto [boll_t, err_t] = check_cond(row, cond.substr(i + 1, j - i - 1) + ';');
                    if (err_t != "")
                        return {false, err_t};

                    boll2 = boll_t;

                    if (cond[j] == ';')
                        return {false, "Expected closing ')'"};
                }
                else 
                    return {false, "Expected closing second condition"};

                while (isspace(cond[i]) && cond[i] != ';')
                    i++;
                if (comp == "AND")
                    return {boll1 && boll2, ""};
                else
                    return {boll1 || boll2, ""};
            }
            else {
                auto[col, err] = get_token(cond);
                i = 0;
                if(!err)
                    return {false, col};
                if(!regex_match(col, base_match, token_regex))
                    return {false, "Not a valid column name"};
                while (isspace(cond[i]) && i <= n)
                    i++;
                char comp = cond[i++];
                cond = cond.substr(i, cond.size() - i);
                auto[val, err1] = get_token(cond);
                i = 0;
                if(!err1)
                    return {false, val};
                val = val.substr(1, val.size() - 2);
                size_t k = 0;
                for ( ; k < this->columns.size(); ++k){
                    if (this->columns.at(k) == col)
                        break;
                }
                if(k == this->columns.size())
                    return {false, "Invalid column name"};
                if(comp == '>')
                    return {row.at(k) > val, ""};
                else if(comp == '=')
                    return {row.at(k) == val, ""};
                else if(comp == '<')
                    return {row.at(k) < val, ""};
                else
                    return {false, "Not a valid comparison operator"};
            }
        }
        
    };
    map<string, Table> tbls;
    
    public:

    string create(string table_name, vector<pair<string, bool>> column_names){
        Table t;
        t.name = table_name;
        for (auto c : column_names)
            t.columns.push_back(c.first);
        tbls[table_name] = t;
        return "Table " + table_name + " has been created";
    }
    string insert(string table_name, vector<string> values){
        if (tbls.count(table_name)){
            if (values.size() == tbls[table_name].columns.size())
                tbls[table_name].data.push_back(values);
            else
                return "Number of columns does not match.";
        }
        else
            return "No such table exists.";
        
        return "1 row has been inserted into " + table_name;
    }
    string select(string table_name, string conditions){
        vector<vector<string>> result;
        Table t;
        if (tbls.count(table_name))
            t = tbls[table_name];
        else
            return "No such table exists.";
        if (conditions == "")
            result = t.data;
        else {
            for (auto & r : t.data) {
                auto [v, err] = t.check_cond(r, conditions);
                if(err != "")
                    return err;
                else if(v)
                    result.push_back(r);
            }
        }
        return print_result(result, t.columns).str();
    }
};

DataBase db;

string make_lower(string cmd)
{
    transform(cmd.begin(), cmd.end(), cmd.begin(), [](char c){return tolower(c);});
    return cmd;
}


string procces_input(string& cmd)
{
    vector<string> tokens;
    int i = 0;
    while (!cmd.empty()){
        pair<string, bool> s = get_token(cmd);
        if (s.second != true)
            return s.first;
        tokens.push_back(s.first);
        if(make_lower(s.first) == "where"){
            tokens.push_back(cmd);
            tokens.push_back(";");
            break;
        }
    }
    if (make_lower(tokens[0]) == "create"){    //create
        string table_name;
        vector<pair<string, bool>> column_names;
        if(tokens.size() < 6)
            return "incomplete structure";
        i++;
        table_name = tokens[i];
        if(!regex_match(table_name, base_match, token_regex))
            return "Not a valid name";
        i++;
        if (tokens[i] != "(") 
            return  "expected a '(' after: " + tokens[0] + " " + tokens[1];
        i++;
        while(true){
            if(!regex_match(tokens[i], base_match, token_regex))
                return "Not a valid name";
            if(make_lower(tokens[i+1]) == "indexed"){
                column_names.push_back({tokens[i], true});
                i = i+2;
            }
            else{
                column_names.push_back({tokens[i], false});
                i++;
            }
            if(tokens[i] == ")")
                break;
            if(tokens[i] == ";")
                return "expected a ')'";
            if(tokens[i] != ",")
                return "expected a ',' after: " + tokens[i-1];
            i++;
        }
        i++;
        if(tokens[i] !=  ";"){
            return "expected a ';' after: " + tokens[i-1];
        }
        return db.create(table_name, column_names);
    }
    if (make_lower(tokens[0]) == "insert"){        //insert
        string table_name;
        vector<string> values;
        bool into = 0;
        if(tokens.size() < 6)
            return "incomplete structure";
        i++;
        if (make_lower(tokens[1]) == "into"){
            into = 1;
            i++;
        }
        table_name = tokens[i];
        if(!regex_match(table_name, base_match, token_regex))
            return "Not a valid name";
        i++;
        if (tokens[i] != "(") 
            if(into = 1)
                return  "expected a '(' after: " + tokens[0] + " " + tokens[1] + " " + tokens[2];
            else
                return  "expected a '(' after: " + tokens[0] + " " + tokens[1];
        i++;
        while(true){
            values.push_back(tokens[i].substr(1, tokens[i].size() - 2));
            i++;
            if(tokens[i] == ")")
                break;
            if(tokens[i] == ";")
                return "expected a ')'";
            if(tokens[i] != ",")
                return "expected a ',' after: " + tokens[i-1];
            i++;
        }
        i++;
        if(tokens[i] !=  ";"){
            return "expected a ';' after: " + tokens[i-1];
        }
        return db.insert(table_name, values);
    }
    if (make_lower(tokens[0]) == "select"){  //select
        string table_name;
        string conditions;
        if(tokens.size() < 4)
            return "incomplete structure";
        i++;
        if(make_lower(tokens[1]) != "from")
            return  "expected a 'FROM' after: " + tokens[0];
        i++;
        table_name = tokens[i];
        if(!regex_match(table_name, base_match, token_regex))
            return "Not a valid name";
        i++;
        if (make_lower(tokens[i]) == "where"){
            conditions = tokens[i+1];
            i = i+2;
        }
        else{
            conditions = "";
        }
        if(tokens[i] !=  ";"){
            return "expected a ';' after: " + tokens[i-1];
        }
        return db.select(table_name, conditions);
    }
    return " ";
}

int main()
{
    while(true)
    {
        string cmd; 
        getline(cin, cmd, ';');
        if (cmd == "exit")
            return 0;
        cmd.append(";");
        cout << procces_input(cmd);
    }
}