#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <cmath>

using namespace std;

int num_transactions;
float minsup = 0;
map<int, int> global_order;
ofstream ofile;
map<int, int> *global_count_map;

class Node
{
public:
	int item;
	int count;
	vector<Node *> children;
	Node *parent;

	Node()
	{
		item = -1;
	}
	Node(int id, Node *par, int c)
	{
		item = id;
		count = c;
		parent = par;
	}
};

bool sortFunction(const pair<int, int> first, const pair<int, int> second);
bool filterFunction(const pair<int, int> elem);
void createFPTree(vector<pair<int, int>> transactions, Node *tree, map<int, vector<Node *>> *headerTable);
void FPGrowth(vector<Node *> node_list, vector<int> candidate_set);

bool sortFunction(const pair<int, int> first, const pair<int, int> second)
{
	return (first.second > second.second);
}

bool newSortFunction(const pair<int, int> first, const pair<int, int> second)
{
	if ((*global_count_map)[first.first] > (*global_count_map)[second.first])
		return true;
	else if ((*global_count_map)[first.first] == (*global_count_map)[second.first])
	{
		if (global_order[first.first] < global_order[second.first])
			return true;
	}
	return false;
}

bool filterFunction(const pair<int, int> elem)
{
	return (((elem.second * 1.0) / num_transactions) < minsup);
}

bool filterByMap(const pair<int, int> elem)
{
	return (((*global_count_map)[elem.first] * 1.0) / num_transactions < minsup);
}

void printItem(vector<int> items)
{
	for (int i = 0; i < items.size(); i++)
		ofile << items[i] << " ";
	ofile << endl;
}

int main(int argc, char *argv[])
{
	auto start = chrono::high_resolution_clock::now();
	if (argc != 3)
	{
		cout << "Please enter 3 arguments" << endl;
		exit(1);
	}

	string input_file, output_file;
	ifstream inFile;
	map<int, int> count_map;

	input_file = argv[1];
	// minsup = (atof(argv[2])*1.0) / 100;
	output_file = string(argv[2]);

	inFile.open(input_file);
	if (!inFile)
	{
		cout << "File doesn't exist" << endl;
		exit(1);
	}

	num_transactions = 0;
	string line;
	while (getline(inFile, line))
	{
		num_transactions++;
		istringstream iss(line);
		for (string s; iss >> s;)
			count_map[stoi(s)]++;
	}
	inFile.close();

	vector<pair<int, int>> flist(count_map.begin(), count_map.end());
	sort(flist.begin(), flist.end(), &sortFunction);
	float flist_sum = 0;
	float max_sup{0};

	vector<float> v;

	for (int i = 0; i < flist.size(); i++)
	{
		v.push_back(float(flist[i].second) / num_transactions);
	}

	double sum = accumulate(v.begin(), v.end(), 0.0);
	double mean = sum / v.size();

	vector<double> diff(v.size());
	transform(v.begin(), v.end(), diff.begin(), [mean](double x)
			  { return x - mean; });
	double sq_sum = inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
	double stdev = sqrt(sq_sum / v.size());

	double xp = stdev / mean;

	for (int i = 0; i < flist.size() / 2; i++)
	{
		flist_sum += flist[i].second;
	}

	double mean2 = (flist_sum / num_transactions) / (flist.size() / 2);

	// cout<<mean2<<endl;

	flist.erase(remove_if(flist.begin(), flist.end(), &filterFunction), flist.end());
	sort(flist.begin(), flist.end(), &sortFunction);

	minsup = mean2 * xp;

	// cout<<"minsup = "<<minsup<<endl;

	for (int i = 0; i < flist.size(); i++)
		global_order[flist[i].first] = i;

	global_count_map = &count_map;

	map<int, vector<Node *>> headerTable;
	Node tree;

	inFile.open(input_file);
	int check = 1;
	while (getline(inFile, line))
	{
		vector<pair<int, int>> transaction_items;
		istringstream iss(line);
		for (string s; iss >> s;)
		{
			int item = stoi(s);
			if ((count_map[item] * 1.0 / num_transactions) >= minsup)
				transaction_items.push_back(make_pair(item, 1));
		}

		sort(transaction_items.begin(), transaction_items.end(), &newSortFunction); //
		createFPTree(transaction_items, &tree, &headerTable);
		check++;
	}

	ofile.open(output_file);
	for (int i = flist.size() - 1; i >= 0; i--)
		FPGrowth(headerTable[flist[i].first], vector<int>());
	ofile.close();

	auto end = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

	// cout << "Execution time: " << duration << " ms" << endl;
}

void createFPTree(vector<pair<int, int>> transactions, Node *tree, map<int, vector<Node *>> *headerTable)
{
	Node *curNode = tree;
	vector<pair<int, int>>::iterator it;
	for (it = transactions.begin(); it != transactions.end(); it++)
	{
		vector<Node *>::iterator it1;
		bool found = false;
		for (it1 = (*curNode).children.begin(); it1 != (*curNode).children.end(); it1++)
		{
			if ((**it1).item == (*it).first)
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			(**it1).count += (*it).second;
			curNode = *it1;
		}

		if (!found)
		{
			Node *new_node = new Node((*it).first, curNode, (*it).second);
			(*curNode).children.push_back(new_node);
			(*headerTable)[(*it).first].push_back(new_node);
			curNode = new_node;
		}
	}
}

void FPGrowth(vector<Node *> node_list, vector<int> candidate_set)
{
	int count = 0;

	for (int i = 0; i < node_list.size(); i++)
		count += node_list[i]->count;

	if ((count * 1.0 / num_transactions) >= minsup)
	{
		candidate_set.push_back(node_list[0]->item);
		printItem(candidate_set);
		map<int, int> item_count;

		for (int i = 0; i < node_list.size(); i++)
		{
			Node *next_item = node_list[i]->parent;
			while (next_item->item != -1)
			{
				item_count[next_item->item] += node_list[i]->count;
				next_item = next_item->parent;
			}
		}

		global_count_map = &item_count;

		vector<pair<int, int>> new_flist(item_count.begin(), item_count.end());
		new_flist.erase(remove_if(new_flist.begin(), new_flist.end(), &filterFunction), new_flist.end());
		sort(new_flist.begin(), new_flist.end(), &newSortFunction);

		Node new_tree;
		map<int, vector<Node *>> new_headerTable;

		for (int i = 0; i < node_list.size(); i++)
		{
			vector<pair<int, int>> transactions;
			Node *next_item = node_list[i]->parent;
			while (next_item->item != -1)
			{
				if ((item_count[next_item->item] * 1.0 / num_transactions) >= minsup)
				{
					transactions.push_back(make_pair(next_item->item, node_list[i]->count));
				}
				next_item = next_item->parent;
			}
			sort(transactions.begin(), transactions.end(), &newSortFunction);

			createFPTree(transactions, &new_tree, &new_headerTable);
		}

		for (int i = new_flist.size() - 1; i >= 0; i--)
		{
			FPGrowth(new_headerTable[new_flist[i].first], candidate_set);
		}
	}
}