#include<bits/stdc++.h>
#include "Column.h"

Column::Column(const std::string& colName) {
    name = colName;
}

void Column::insert(const std::string& value){
    data.push_back(value);
}

void Column::print() const{
    std::cout<< "Column Name: " << name << '\n';
    for(auto& it: data) {
        std::cout<<it<<" ";
    }
    std::cout << '\n';
}

void Column::getValueAt(int index){
    if(index < 0 || index >= data.size()){
        std::cout<<"Index out of bounds!"<<'\n';
        return;
    }
    else{
        std::cout<< "Value at index " << index << ": " << data[index] << '\n';
    }
}


void Column::deleteAt(int index){
    if(index<0||index>data.size()){
        std::cout<<"Index out of bounds!"<<'\n';
        return;
    }
    data.erase(data.begin() + (index-1));
    std::cout << "Deleted value at index"<<'\n';
}


int Column::size() const{
    std::cout<<"Size of column "<<name<<": "<<data.size()<<'\n';
    return data.size();
}
std::string Column::getName() const {
    return name;
}

std::string Column::getDataAt(size_t index) const {
    if (index < data.size()) {
        return data[index];
    }
    return "";  
}

void Column::update(size_t index, const std::string& newVal) {
    if (index < data.size()) {
        data[index] = newVal;
    } else {
        std::cout << "Index out of bounds!\n";
    }
}

std::vector<int>Column::search(const std::string& value) const{
    std::vector<int> indices;
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == value) {
            indices.push_back(i);
        }
    }
    return indices;
}