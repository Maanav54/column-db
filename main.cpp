#include <iostream>
#include "Column.h"
#include "Table.h"
#include "QueryProcessor.h"
using namespace std;
QueryProcessor qp;

int main() {
    cout << "Column-Store DB Started!!" << '\n';
    Column agecol("Age");
    
    agecol.insert("25");
    agecol.insert("30");
    agecol.insert("35");
    agecol.insert("40");

    agecol.print();
    agecol.size();

    Table studentTable({"ID", "Name", "Age"});

    studentTable.insertRow({"1", "Alice", "20"});
    studentTable.insertRow({"2", "Bob", "22"});
    studentTable.insertRow({"3", "Charlie", "21"});

    //studentTable.print();

    studentTable.update("Name", 1, "zoya");    
    studentTable.update("Age", 2, "26");    

    // std::cout << "\nAfter Update:\n";
    // studentTable.print();
    qp.addTable("StudentTable", studentTable);
    //qp.executeQuery("SELECT * FROM StudentTable");
    qp.executeQuery("SELECT * FROM StudentTable WHERE Age > 20");
    qp.executeQuery("SELECT Name, Age FROM StudentTable");
    qp.executeQuery("SELECT Age FROM StudentTable WHERE Name = zoya");
    return 0;
}

