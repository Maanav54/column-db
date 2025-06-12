#include "QueryProcessor.h"
#include <bits/stdc++.h>

QueryProcessor::QueryProcessor() {
}

void QueryProcessor::addTable(const std::string& name, const Table& table) {
    tables[name] = table;
    std::cout << "Table '" << name << "' added successfully." << '\n';
}


void QueryProcessor::executeQuery(const std::string& query) {
    std::cout << "Executing query: " << query << '\n';
    std::string token;
    std::istringstream iss(query); 
    std::vector<std::string>tokens;
    while (iss >> token) {
        tokens.push_back(token);
    }
    if (tokens.size() < 4 || tokens[0] != "SELECT" || tokens[1] != "*" || tokens[2] != "FROM") {
        std::cout << "Invalid query format." <<'\n';
        return;
    }
    std::string tableName = tokens[3];
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        it->second.print();
    } else {
        std::cout << "Table '" << tableName << "' not found." << '\n';
    }
}

