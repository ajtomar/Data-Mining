#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3) // modified
    {
        cout << "Please enter 3 arguments" << endl;
        exit(1);
    }

    string compressed_file, decom_file;

    compressed_file = string(argv[1]);
    decom_file = string(argv[2]);

    ifstream infile;
    ofstream ofile;

    string line;
    int key = 0;
    vector<vector<int>> x_map;
    infile.open(compressed_file);

    if (!infile)
    {
        cout << "File doesn't exist" << endl;
        exit(1);
    }

    int map_start;
    getline(infile, line);
    istringstream iss(line);
    string s;
    iss >> s;
    map_start = stoi(s) - 2;

    // cout << map_start;

    for (int i = 0; i < map_start; i++)
    {
        getline(infile, line);
    }

    while (getline(infile, line))
    {
        vector<int> temp;
        istringstream iss(line);
        for (string s; iss >> s;)
        {
            int item = stoi(s);
            temp.push_back(item);
        }
        x_map.push_back(temp);
    }
    infile.close();

    infile.open(compressed_file);
    ofile.open(decom_file);
    if (!infile)
    {
        cout << "File doesn't exist" << endl;
        exit(1);
    }

    getline(infile, line);

    for (int i = 0; i < map_start; i++)
    {
        getline(infile, line);
        vector<int> temp;
        istringstream iss(line);
        for (string s; iss >> s;)
        {
            int item = stoi(s);
            if (item >= 0)
            {
                temp.push_back(item);
            }
            else
            {
                for (int i = 0; i < x_map.size(); i++)
                {
                    if (item == x_map[i][0])
                    {
                        temp.insert(temp.end(), x_map[i].begin() + 1, x_map[i].end());
                    }
                }
            }
        }
        for (int j = 0; j < temp.size(); j++)
        {
            ofile << temp[j] << " ";
        }
        ofile << endl;
        temp.clear();
    }
    ofile.close();
    infile.close();
    return 0;
}