#include "QueryProcessor.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip> // For std::setw and std::left

QueryProcessor::QueryProcessor()
{
}

void QueryProcessor::addTable(const std::string &name, const Table &table)
{
    tables[name] = table;
    std::cout << "Table '" << name << "' added successfully." << '\n';
}

std::vector<std::string> QueryProcessor::parseQuery(const std::string &query)
{
    std::istringstream iss(query);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> QueryProcessor::getSelectedColumns(const std::vector<std::string> &tokens)
{
    std::vector<std::string> columns;

    // Start from index 1 (after SELECT) until we find FROM
    for (size_t i = 1; i < tokens.size(); i++)
    {
        if (tokens[i] == "FROM")
            break;

        // Skip commas
        if (tokens[i] == ",")
            continue;

        // Remove any trailing commas from column names
        std::string colName = tokens[i];
        if (!colName.empty() && colName.back() == ',')
        {
            colName.pop_back();
        }
        columns.push_back(colName);
    }

    return columns;
}

void QueryProcessor::printSelectedColumns(const Table &table, const std::vector<std::string> &selectedColumns)
{
    const auto &columns = table.getColumns();
    size_t rowCount = columns.empty() ? 0 : columns[0].size();

    // Validate selected columns exist
    for (const auto &selectedCol : selectedColumns)
    {
        bool found = false;
        for (const auto &col : columns)
        {
            if (col.getName() == selectedCol)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::cout << "Error: Column '" << selectedCol << "' not found\n";
            return;
        }
    }

    // Print column headers with padding
    for (const auto &colName : selectedColumns)
    {
        std::cout << std::left << std::setw(15) << colName;
    }
    std::cout << "\n";

    // Print separator line
    for (size_t i = 0; i < selectedColumns.size(); i++)
    {
        std::cout << std::string(15, '-');
    }
    std::cout << "\n";

    // Print data
    for (size_t i = 0; i < rowCount; i++)
    {
        for (const auto &colName : selectedColumns)
        {
            // Find matching column and print its data
            for (const auto &col : columns)
            {
                if (col.getName() == colName)
                {
                    std::cout << std::left << std::setw(15) << col.getDataAt(i);
                    break;
                }
            }
        }
        std::cout << "\n";
    }
}

void QueryProcessor::executeQuery(const std::string &query)
{
    if (query.empty())
    {
        std::cout << "Error: Empty query\n";
        return;
    }

    std::cout << "Executing query: " << query << '\n';

    auto tokens = parseQuery(query);
    if (tokens.size() < 4)
    {
        std::cout << "Error: Invalid query format\n";
        return;
    }

    if (tokens[0] != "SELECT")
    {
        std::cout << "Error: Query must start with SELECT\n";
        return;
    }

    // Get table name
    auto fromIt = std::find(tokens.begin(), tokens.end(), "FROM");
    if (fromIt == tokens.end())
    {
        std::cout << "Error: Missing FROM clause\n";
        return;
    }

    const std::string &tableName = *(fromIt + 1);
    auto tableIt = tables.find(tableName);
    if (tableIt == tables.end())
    {
        std::cout << "Error: Table '" << tableName << "' not found\n";
        return;
    }

    // Get selected columns
    auto selectedColumns = getSelectedColumns(tokens);

    // Handle WHERE clause if present
    auto whereIt = std::find(tokens.begin(), tokens.end(), "WHERE");
    if (whereIt != tokens.end())
    {
        handleWhereClause(tableIt->second, std::vector<std::string>(whereIt, tokens.end()), selectedColumns);
    }
    else
    {
        // No WHERE clause, print selected columns
        printSelectedColumns(tableIt->second, selectedColumns);
    }
}

void QueryProcessor::handleWhereClause(const Table &table, const std::vector<std::string> &whereTokens, const std::vector<std::string> &selectedColumns)
{
    if (whereTokens.size() < 4)
    {
        std::cout << "Error: Invalid WHERE clause syntax\n";
        return;
    }

    std::string columnName = whereTokens[1];
    std::string op = whereTokens[2];
    std::string value = whereTokens[3];

    const auto &columns = table.getColumns();
    const Column *targetCol = nullptr;

    for (const auto &col : columns)
    {
        if (col.getName() == columnName)
        {
            targetCol = &col;
            break;
        }
    }

    if (!targetCol)
    {
        std::cout << "Error: Column '" << columnName << "' not found\n";
        return;
    }

    std::vector<size_t> matchingRows;
    size_t rowCount = columns[0].size();

    for (size_t i = 0; i < rowCount; i++)
    {
        std::string rowValue = targetCol->getDataAt(i);
        bool matches = false;

        try
        {
            if (op == "=")
                matches = (rowValue == value);
            else if (op == ">")
                matches = (std::stoi(rowValue) > std::stoi(value));
            else if (op == "<")
                matches = (std::stoi(rowValue) < std::stoi(value));
            else if (op == ">=")
                matches = (std::stoi(rowValue) >= std::stoi(value));
            else if (op == "<=")
                matches = (std::stoi(rowValue) <= std::stoi(value));
            else if (op == "!=")
                matches = (rowValue != value);
            else
            {
                std::cout << "Error: Unsupported operator '" << op << "'\n";
                return;
            }
        }
        catch (const std::invalid_argument &)
        {
            matches = false;
        }

        if (matches)
            matchingRows.push_back(i);
    }

    if (matchingRows.empty())
    {
        std::cout << "No rows match the WHERE condition\n";
        return;
    }

    // Print only selected columns
    std::cout << "Filtered results:\n";
    for (const auto &colName : selectedColumns)
    {
        std::cout << std::left << std::setw(15) << colName;
    }
    std::cout << "\n";
    for (size_t i = 0; i < selectedColumns.size(); i++)
    {
        std::cout << std::string(15, '-');
    }
    std::cout << "\n";

    for (size_t rowIndex : matchingRows)
    {
        for (const auto &colName : selectedColumns)
        {
            for (const auto &col : columns)
            {
                if (col.getName() == colName)
                {
                    std::cout << std::left << std::setw(15) << col.getDataAt(rowIndex);
                    break;
                }
            }
        }
        std::cout << "\n";
    }
}
