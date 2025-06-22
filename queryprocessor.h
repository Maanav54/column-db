#pragma once
#include <vector>
#include <string>
#include "Table.h"
#include <map>

class QueryProcessor
{
private:
    std::map<std::string, Table> tables;

    // Helper functions for query processing
    std::vector<std::string> parseQuery(const std::string &query);
    std::vector<std::string> getSelectedColumns(const std::vector<std::string> &tokens);
    void printSelectedColumns(const Table &table, const std::vector<std::string> &columns);
    void handleWhereClause(const Table &table, const std::vector<std::string> &tokens);

public:
    QueryProcessor();
    void addTable(const std::string &name, const Table &table);
    void executeQuery(const std::string &query);
};
