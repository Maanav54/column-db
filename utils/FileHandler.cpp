#include "FileHandler.h"
#include "../Table.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

void FileHandler::saveTable(const Table &table, const std::string &tableName)
{
    std::ofstream outFile(tableName + ".txt");
    if (!outFile)
    {
        throw std::runtime_error("Could not open file for writing: " + tableName);
    }
    const auto &columns = table.getColumns();
    // Write header
    for (size_t i = 0; i < columns.size(); ++i)
    {
        outFile << columns[i].getName();
        if (i != columns.size() - 1)
            outFile << ",";
    }
    outFile << "\n";
    // Write rows
    size_t rowCount = columns.empty() ? 0 : columns[0].size();
    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = 0; col < columns.size(); ++col)
        {
            outFile << columns[col].getDataAt(row);
            if (col != columns.size() - 1)
                outFile << ",";
        }
        outFile << "\n";
    }
}

Table FileHandler::loadTable(const std::string &tableName)
{
    std::ifstream inFile(tableName + ".txt");
    if (!inFile)
    {
        throw std::runtime_error("Could not open file for reading: " + tableName);
    }
    std::string header;
    if (!std::getline(inFile, header))
    {
        throw std::runtime_error("File is empty: " + tableName);
    }
    std::vector<std::string> colNames;
    std::stringstream ss(header);
    std::string colName;
    while (std::getline(ss, colName, ','))
    {
        colNames.push_back(colName);
    }
    Table table(colNames);
    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.empty())
            continue;
        std::vector<std::string> row;
        std::stringstream rowss(line);
        std::string value;
        while (std::getline(rowss, value, ','))
        {
            row.push_back(value);
        }
        table.insertRow(row);
    }
    return table;
}