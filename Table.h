#pragma once
#include <vector>
#include <string>
#include "Column.h"
class Table
{
private:
    std::vector<Column> columns;

public:
    Table();
    Table(const std::vector<std::string> &colNames); // create columns by names
    void update(const std::string &columnName, int index, const std::string &newVal);
    void insertRow(const std::vector<std::string> &rowData); // insert a full row
    void print() const;
    std::vector<int> search(const std::string &columnName, const std::string &value) const;
    std::vector<Column> &getColumns() { return columns; }
    const std::vector<Column> &getColumns() const { return columns; }
};
