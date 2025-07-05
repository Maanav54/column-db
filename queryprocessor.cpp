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

std::vector<std::string> QueryProcessor::getSelectedColumns(const std::vector<std::string> &tokens, Table &table)
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

        if (tokens[i] == "*")
        {
            std::vector<std::string> allNames;
            for (auto &col : table.getColumns())
                allNames.push_back(col.getName());
            return allNames;
        }
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

void QueryProcessor::printSelectedColumns(Table &table, const std::vector<std::string> &selectedColumns)
{
    auto &columns = table.getColumns();
    size_t rowCount = columns.empty() ? 0 : columns[0].size();

    // Validate selected columns exist
    for (const auto &selectedCol : selectedColumns)
    {
        bool found = false;
        for (auto &col : columns)
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
    if (tokens.empty())
    {
        std::cout << "Error: Invalid query format\n";
        return;
    }

    // Handle INSERT queries
    if (tokens[0] == "INSERT")
    {
        // Expected: INSERT INTO TableName VALUES (val1, val2, ...)
        if (tokens.size() < 6 || tokens[1] != "INTO")
        {
            std::cout << "Error: Invalid INSERT query format\n";
            return;
        }
        std::string tableName = tokens[2];
        auto tableIt = tables.find(tableName);
        if (tableIt == tables.end())
        {
            std::cout << "Error: Table '" << tableName << "' not found\n";
            return;
        }
        // Find "VALUES"
        auto valuesIt = std::find(tokens.begin(), tokens.end(), "VALUES");
        if (valuesIt == tokens.end() || (valuesIt + 1) == tokens.end())
        {
            std::cout << "Error: Missing VALUES keyword or values\n";
            return;
        }

        // Join the remaining tokens into a single string to handle parentheses and commas
        std::string valuesStr;
        for (auto it = valuesIt + 1; it != tokens.end(); ++it)
        {
            if (!valuesStr.empty())
                valuesStr += " ";
            valuesStr += *it;
        }

        // Remove parentheses if present
        if (!valuesStr.empty() && valuesStr.front() == '(' && valuesStr.back() == ')')
        {
            valuesStr = valuesStr.substr(1, valuesStr.size() - 2);
        }

        // Now split by comma and trim spaces/quotes
        std::vector<std::string> values;
        std::istringstream valss(valuesStr);
        std::string val;
        while (std::getline(valss, val, ','))
        {
            // Remove leading/trailing spaces
            size_t start = val.find_first_not_of(" '\"");
            size_t end = val.find_last_not_of(" '\"");
            if (start == std::string::npos || end == std::string::npos)
                values.push_back("");
            else
                values.push_back(val.substr(start, end - start + 1));
        }

        if (values.size() != tableIt->second.getColumns().size())
        {
            std::cout << "Error: Number of values does not match number of columns\n";
            return;
        }
        tableIt->second.insertRow(values);
        std::cout << "Row inserted into '" << tableName << "'\n";
        return;
    }

    if (tokens[0] == "DELETE")
    {
        auto fromIt = std::find(tokens.begin(), tokens.end(), "FROM");
        if (fromIt == tokens.end() || (fromIt + 1) == tokens.end())
        {
            std::cout << "Error: Invalid DELETE query format\n";
            return;
        }
        std::string tableName = *(fromIt + 1);
        auto tableIt = tables.find(tableName);
        if (tableIt == tables.end())
        {
            std::cout << "Error: Table '" << tableName << "' not found\n";
            return;
        }
        auto selectedColumns = getSelectedColumns(tokens, tableIt->second);

        // Handle WHERE clause if present
        auto whereIt = std::find(tokens.begin(), tokens.end(), "WHERE");
        if (whereIt != tokens.end())
        {
            handleWhereClause(tableIt->second, std::vector<std::string>(whereIt, tokens.end()), selectedColumns, true);
        }
        else
        {
            // No WHERE clause, delete all rows
            size_t rowCount = tableIt->second.getColumns()[0].size();
            if (rowCount == 0)

            {
                std::cout << "No rows to delete in table '" << tableName << "'\n";
                return;
            }
            // Delete all rows
            for (int i = static_cast<int>(rowCount) - 1; i >= 0; --i)
            {
                for (auto &col : tableIt->second.getColumns())
                {
                    col.deleteAt(i);
                }
            }
            std::cout << "All rows deleted from table '" << tableName << "'\n";
            return;
        }
    }
    if(tokens[0]=="UPDATE"){
        if(tokens[2]!="SET" || tokens.size() < 5)
        {
            std::cout << "Error: Invalid UPDATE query format\n";
            return;
        }
        auto setIt = std::find(tokens.begin(),tokens.end(),"SET");
        if(setIt == tokens.end() || (setIt + 1) == tokens.end())
        {
            std::cout << "Error: Missing SET clause\n";
            return;
        }
        std::string tableName = tokens[1];
        auto tableIt = tables.find(tableName);
        if (tableIt == tables.end())
        {
            std::cout << "Error: Table '" << tableName << "' not found\n";
            return;
        }
        std::string columnName = *(setIt + 1);
        std::string newValue = *(setIt + 3);
        auto whereIt = std::find(tokens.begin(), tokens.end(), "WHERE");
        if (whereIt == tokens.end() || (whereIt + 1) == tokens.end() || (whereIt + 2) == tokens.end())
        {
            std::cout << "Error: Missing WHERE clause or condition\n";
            return;
        }
        std::string whereColumn = *(whereIt + 1);
        std::string whereValue = *(whereIt + 3);
        auto &columns = tableIt->second.getColumns();
        const Column *targetCol = nullptr;
        for (const auto &col : columns)
        {
            if (col.getName() == whereColumn)
            {
                targetCol = &col;
                break;
            }
        }
        if (!targetCol)
        {
            std::cout << "Error: Column '" << whereColumn << "' not found\n";
            return;
        }
        std::vector<size_t> matchingRows;
        size_t rowCount = columns[0].size();
        for (size_t i = 0; i < rowCount; i++)
        {
            std::string rowValue = targetCol->getDataAt(i);
            if (rowValue == whereValue)
            {
                matchingRows.push_back(i);
            }
        }
        if (matchingRows.empty())
        {
            std::cout << "No rows match the WHERE condition\n";
            return;
        }
        // Update the specified column for matching rows
        for (size_t rowIndex : matchingRows)
        {
            for (auto &col : columns)
            {
                if (col.getName() == columnName)
                {
                    col.update(rowIndex, newValue);
                    std::cout << "Updated '" << columnName << "' to '" << newValue << "' for row " << rowIndex + 1 << "\n";
                    break;
                }
            }
        }
        return;
    }
    if (tokens.size() < 4)
    {
        std::cout << "Error: Invalid query format\n";
        return;
    }

    if (tokens[0] == "SELECT")
    {

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
        auto selectedColumns = getSelectedColumns(tokens, tableIt->second);

        // Handle WHERE clause if present
        auto whereIt = std::find(tokens.begin(), tokens.end(), "WHERE");
        if (whereIt != tokens.end())
        {
            handleWhereClause(tableIt->second, std::vector<std::string>(whereIt, tokens.end()), selectedColumns, false);
        }
        else
        {
            // No WHERE clause, print selected columns
            printSelectedColumns(tableIt->second, selectedColumns);
        }
    }
}

void QueryProcessor::handleWhereClause(Table &table, const std::vector<std::string> &whereTokens, const std::vector<std::string> &selectedColumns, bool isDelete)
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
    if (isDelete)
    {
        // Delete matching rows
        std::sort(matchingRows.rbegin(), matchingRows.rend()); // Sort in reverse order to avoid index shifting issues
        for (size_t rowIndex : matchingRows)
        {
            for (auto &col : table.getColumns())
            {
                    col.deleteAt(static_cast<int>(rowIndex));
            }
        }
        std::cout << "Rows deleted successfully.\n";
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
