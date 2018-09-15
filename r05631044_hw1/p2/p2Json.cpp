/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
bool
Json::read(const string& jsonFile)
{
   return true; // TODO
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

void Json::print(){
    cout << "{" << endl;
    for(int i=0; i<(int)_obj.size(); i++)
        cout << " " << _obj[i].get_key() << " : " << _obj[i].get_value() << endl;
    cout << "}" << endl;
}

void Json::add(){
    JsonElem newjsonelem;
    string key;
    int value;
    cin >> key >> value;

    key = "\"" + key + "\"";

    newjsonelem.set_key(key);
    newjsonelem.set_value(value);

    _obj.push_back(newjsonelem);
}

float Json::sum(){
    int sum = 0;
    for(int i=0; i<(int)_obj.size(); i++)
        sum += _obj[i].get_value();
    
    return sum;
}

float Json::ave(float sum){
    return (sum/_obj.size());
}

float Json::max(){
    int pos = 0;
    for(int i=1; i<(int)_obj.size(); i++)
        if(_obj[i].get_value() > _obj[pos].get_value()) pos = i;

    return pos;
}

float Json::min(){
    int pos = 0;
    for(int i=1; i<(int)_obj.size(); i++)
        if(_obj[i].get_value() < _obj[pos].get_value()) pos = i;

    return pos;
}

bool Json::exit(){
    return true;
}

void Json::set_obj(JsonElem jsonelem){
    _obj.push_back(jsonelem);
}

vector<JsonElem> Json::get_obj() const{
    return _obj;
}

string JsonElem::get_key() const{
    return _key;
}

int JsonElem::get_value() const{
    return _value;
}

void JsonElem::set_key(string key){
    _key = key;
}

void JsonElem::set_value(int value){
    _value = value;
}
