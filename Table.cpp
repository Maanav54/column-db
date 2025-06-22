#include "Table.h"
#include<bits/stdc++.h>

Table::Table() {}


// Constructor
Table::Table(const std::vector<std::string>& colNames) {
    for(auto& it:colNames){
        columns.push_back(Column(it));
    }
}

// Insert a full row
void Table::insertRow(const std::vector<std::string>& rowData) {
    int n=rowData.size();
    if (n != columns.size()) {
        std::cout << "Row size mismatch!" << '\n';
        return;
    }
    for(int i=0;i<n;i++){
        columns[i].insert(rowData[i]);
    }
}

// Print all columns
void Table::print() const {
    std::cout<<"Column Names: ";
    for(auto& col:columns){
        std::cout<<col.getName()<<" ";
    }
    std::cout<<'\n';

    size_t rows = columns.empty() ? 0 : columns[0].size();
    std::cout << "Number of rows: " << rows << "\n";

    // Loop over each row
    for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < columns.size(); ++j) {
        std::cout << columns[j].getDataAt(i);
        if (j != columns.size() - 1) std::cout << " "; 
    }
    std::cout << '\n';  
}
}


void Table::update(const std::string& columnName, int index, const std::string& newVal){
    for(auto& col: columns){
        if(col.getName()==columnName){
            col.update(index,newVal);
        }
    }
} 

std::vector<int> Table::search(const std::string& columnName, const std::string& value) const{
    for(auto& col:columns){
        if(col.getName()==columnName){
            return col.search(value);
        }
    }
}

