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

    string create(string table_name, vector<pair<string, bool>> column_names);
    string insert(string table_name, vector<string> values);
    string select(string table_name, string conditions);
};

DataBase db;

const regex token_regex("[a-zA-Z][a-zA-Z0-9_]*");
smatch base_match;

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
    }
    if (make_lower(tokens[0]) == "create"){
        string table_name;
        vector<pair<string, bool>> column_names;
        if(tokens.size() < 6)
            return "incomplete structure";
        i++;
        string table_name = tokens[i];
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
                break;
            if(tokens[i] != ",")
                return "expected a ',' after: " + tokens[i-1];
            i++;
        }
        i++;
        if(tokens[i] !=  ";"){
            return "expected a ';' after: " + tokens[i-1];
        }
        db.create(table_name, column_names);
        return "Table " + table_name + " has been created";
    }
}

char delims[] = {'"', ',', '(', ')', ';'};

bool if_delim(char c){
    bool if_delim = 0;
    for (int i = 1; i < 5; i++){
        if (c == delims[i]){
            if_delim = 1;

        }
    }
    return if_delim;
}

pair<string, bool> get_token(string& cmd){
    string token;
    int i = 0;
    while (isspace(cmd[i])){
        i++;
    } 
    if(if_delim(cmd[i])){
        token = cmd[i];
        cmd.erase(0, i);
        return {token, 1};
    }
    if(cmd[i] == delims[0]){
        token += cmd[i];
        for (int j = i+1; j > cmd.size(); j++){
            if(cmd[j] == delims[0]){
                token += cmd[j];
                cmd.erase(0, j);
                return {token, 1};
            }
            else token += cmd[j]; 
        }
        cmd.clear();
        return {"expected a \" after: " + token, 0};
    }
    while(1){
        for (int j = i; j > cmd.size(); j++){
            if(if_delim(cmd[j]) || isspace(cmd[j])){
                cmd.erase(0, j-1);
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


int main()
{
    while(true)
    {
        std::string cmd; 
        getline(cin, cmd, ';');

        if (cmd == "exit")
            return 0;

        cout << procces_input(cmd);
    }
}