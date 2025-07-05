#include <iostream>
#include "Column.h"
#include "Table.h"
#include "QueryProcessor.h"
#include "utils/FileHandler.h"
using namespace std;
QueryProcessor qp;

int main()
{
    cout << "Column-Store DB Started!!" << '\n';

    // Load or create a default table for demonstration
    Table t({"ID", "Name", "Age"});
    t.insertRow({"1", "Alice", "20"});
    t.insertRow({"2", "Bob", "22"});
    FileHandler::saveTable(t, "StudentTable");
    Table loaded = FileHandler::loadTable("StudentTable");
    qp.addTable("StudentTable", loaded);

    cout << "Type SQL-like queries (e.g., SELECT * FROM StudentTable), or type EXIT to quit.\n";
    string input;
    while (true)
    {
        cout << "\n> ";
        getline(cin, input);
        if (input == "EXIT" || input == "exit" || input == "quit" || input == "QUIT")
        {
            cout << "Exiting Column-Store DB.\n";
            break;
        }
        if (input == "HELP" || input == "help")
        {
            cout << "Supported commands:\n";
            cout << "  SELECT ...\n  INSERT ...\n  UPDATE ...\n  DELETE ...\n";
            cout << "Type EXIT to quit.\n";
            continue;
        }
        if (input.empty())
            continue;
        qp.executeQuery(input);
    }
    return 0;
}
