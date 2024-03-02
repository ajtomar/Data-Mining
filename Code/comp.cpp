#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>


using namespace std;

bool cmp(pair<vector<int>, vector<int>> a, pair<vector<int>, vector<int>> b)
{
    return a.second.size() > b.second.size();
}

int main(int argc, char *argv[])
{
    auto start = chrono::high_resolution_clock::now();
    int thres = atoi(argv[3]); // setting the threshold for final mapping used
    int trans_count = 0, mapping_count = 0, mapping_data_count = 0, comp_count = 0;
    vector<pair<vector<int>, vector<int>>> main_map;
    string arg4_file = argv[4]; // final output file to be created
    ofstream outfile(arg4_file);
    ifstream inputFile(argv[2]); // reading the frequent itemset to create mapping
    if (!inputFile.is_open())
    {
        cout << " Hey!...Error opening file for reading." << std::endl;
        return 1;
    }
    string line;
    int i = -1;
    while (getline(inputFile, line))
    {
        istringstream iss(line);
        vector<int> in_map;
        vector<int> lab_map;
        for (string s; iss >> s;)
        {
            int item = stoi(s);
            in_map.push_back(item);
        }
        sort(in_map.begin(), in_map.end());
        lab_map.push_back(i);
        lab_map.push_back(0);
        if (in_map.size() > thres - 1)
        {
            main_map.push_back(make_pair(lab_map, in_map));
            i--;
        }
        in_map.clear();
        lab_map.clear();
    }
    inputFile.close();
    sort(main_map.begin(), main_map.end(), cmp);
    ifstream transFile(argv[1]); // Reading the dataset ex: D_small.dat
    if (!transFile.is_open())
    {
        cout << "Error opening file for reading." << std::endl;
        return 1;
    }

    string newline;
    int m = 0, map_start = 0, trans_lines = 0;
    outfile << "0000000000" << endl;
    while (getline(transFile, newline))
    {
        istringstream iss(newline);
        trans_lines++;
        vector<int> trans;
        for (string s; iss >> s;)
        {
            int item = stoi(s);
            trans.push_back(item);
            // trans_count++;
        }
        trans_count += trans.size();
        sort(trans.begin(), trans.end());
        for (int i = 0; i < main_map.size(); i++)
        {
            if (includes(trans.begin(), trans.end(), main_map[i].second.begin(), main_map[i].second.end()) == true)
            {
                for (int j = 0; j < main_map[i].second.size(); j++)
                {
                    for (int l = 0; l < trans.size(); l++)
                    {
                        if (main_map[i].second[j] == trans[l])
                        {
                            // cout<<*(trans.begin()+l)<<" ";
                            trans.erase(trans.begin() + l);
                            break;
                        }
                    }
                }
                trans.push_back(main_map[i].first[0]);
                main_map[i].first[1] = 1;
            }
        }
        comp_count += trans.size();
        for (int x : trans)
            outfile << x << " ";
        map_start++;
        outfile << endl;
        trans.clear();
    }
    outfile << endl;
    for (int k = 0; k < main_map.size(); k++)
    {
        if (main_map[k].first[1] == 1)
        {
            outfile << main_map[k].first[0] << " ";
            mapping_count++;
            for (int m = 0; m < main_map[k].second.size(); m++)
            {
                outfile << main_map[k].second[m] << " ";
                mapping_data_count++;
            }
            outfile << endl;
        }
    }
    outfile.seekp(0);
    outfile << map_start + 3 << " ";

    outfile.close();
    // cout << endl;
    // cout << "Min mapping lenth: " << thres << endl;
    // cout << "No. of transactions: " << trans_lines << endl;
    // cout << "Transaction Count: " << trans_count << endl;
    // cout << "No of mapping: " << mapping_count << endl;
    // cout << "No of mapping data: " << mapping_data_count << endl;
    // cout << "No of compressed data: " << comp_count << endl;
    // cout << "Total compressed data: " << (mapping_count + mapping_data_count + comp_count) << endl;
    // cout << endl
    //      << "Data compressed to " << setprecision(4) << (1 - (float(mapping_count) + float(mapping_data_count) + float(comp_count)) / float(trans_count)) * 100 << "%" << endl;
    
    auto end = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

	// cout << "Execution time: " << duration << " ms" << endl;
    return 0;
}