#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>

using namespace std;

class DataBase
{
    private: 
    class Table
    {
        private:
        vector<vector<string>> data;
        string name;

        public:

    };
    map<string, Table> tbls;
    
    public:

    string create(string table_name, vector<pair<string, bool>> column_names){
        return "Table " + table_name + " has been created";
    }
    string insert(string table_name, vector<string> values){
        return "1 row has been inserted into " + table_name;
    }
    string select(string table_name, string conditions){
        return "selected";
    }
};

DataBase db;

const regex token_regex("[a-zA-Z][a-zA-Z0-9_]*");
smatch base_match;

string make_lower(string cmd)
{
    transform(cmd.begin(), cmd.end(), cmd.begin(), [](char c){return tolower(c);});
    return cmd;
}

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
            return  "expected a '(' after: " + tokens[0] + tokens[1];
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
                return  "expected a '(' after: " + tokens[0] + tokens[1] + tokens[2];
            else
                return  "expected a '(' after: " + tokens[0] + tokens[1];
        i++;
        while(true){
            values.push_back(tokens[i]);
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
            conditions = "NULL";
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