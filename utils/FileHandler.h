#pragma once
#include "../Table.h"
#include <string>

class FileHandler {
public:
    static void saveTable(const Table& table, const std::string& tableName);
    static Table loadTable(const std::string& tableName);
};