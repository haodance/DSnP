/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"
#include <fstream>
#include <iomanip>

using namespace std;

int command(string);

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands

   ifstream fin(jsonFile);
   JsonElem jsonelem;
   string key;
   int value;
   char temp;
   fin >> temp;
   while(fin >> key){
       if(key == "}") break;
       else if(key == ",") fin >> key;
       fin >> temp >> value;
       jsonelem.set_key(key);
       jsonelem.set_value(value);
       json.set_obj(jsonelem);
   }

   fin.close();

   // cout << "Enter command: ";
   string cmd;
   while (true) {
       cout << "Enter command: ";
       cin >> cmd;

       int sw = command(cmd);
       vector<JsonElem> obj = json.get_obj();
       switch(sw){
           case 0:{
                      json.print();
                      break;
                  }
           case 1:{
                      json.add();
                      break;
                  }
           case 2:{
                      if(obj.size() == 0) cout << "Error: No element found!!" << endl;
                      else cout << "The summation of the values is: " << json.sum() << "." << endl;
                      break;
                  }
           case 3:{
                      if(obj.size() == 0) cout << "Error: No element found!!" << endl;
                      else cout << "The average of the values is: " << fixed << setprecision(1) << json.ave(json.sum()) << "." <<endl;
                      break;
                  }
           case 4:{
                      int max = json.max();
                      if(obj.size() == 0) cout << "Error: No element found!!" << endl;
                      else{
                          cout << "The maximum element is: { ";
                          cout << obj[max].get_key() << " : " << obj[max].get_value() << " }." << endl;
                      }
                      break;
                  }
           case 5:{
                      int min = json.min();
                      if(obj.size() == 0) cout << "Error: No element found!!" << endl;
                      else{
                          cout << "The minimum element is: { ";
                          cout << obj[min].get_key() << " : " << obj[min].get_value() << " }." << endl;
                      }
                      break;
                  }
           case 6:{
                      if(json.exit()) exit(0);
                      break;
                  }
           default:{
                       continue;
                   }
       }
   }

   return 0;
}

int command(string cmd){
    int num = -1;

    if(cmd == "PRINT") num = 0;
    else if(cmd == "ADD") num = 1;
    else if(cmd == "SUM") num = 2;
    else if(cmd == "AVE") num = 3;
    else if(cmd == "MAX") num = 4;
    else if(cmd == "MIN") num = 5;
    else if(cmd == "EXIT") num = 6;

    return num;
}
