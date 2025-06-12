#pragma once
#include "Table.h"
#include <string>
#include <unordered_map>

class QueryProcessor {
private:
    std::unordered_map<std::string, Table> tables;

public:
    QueryProcessor();
    void addTable(const std::string& name, const Table& table);
    void executeQuery(const std::string& query);
};
