#pragma once
#include <vector>
#include <string>

class Column {
private:
    std::string name;              
    std::vector<std::string> data;  
    std::string getValueAt(int index) const;

public:
    Column(const std::string& colName);
    std::string getDataAt(size_t index) const;
    void getValueAt(int index);
    void insert(const std::string& value);  
    void print() const;              
    void deleteAt(int index);
    int size() const;      
    std::string getName() const;      
    void update(size_t index, const std::string& newVal);    
    std::vector<int> search(const std::string& value) const;
};

