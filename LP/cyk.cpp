// http://web.cs.ucdavis.edu/~rogaway/classes/120/winter12/CYK.pdf
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <set>

using namespace std;

map<string, vector<string> > inverse_productions;
string str;
int len;
vector< vector < vector<string> > > table;

bool exists(string key){
	if (inverse_productions.find(key) == inverse_productions.end()){
		return false;
	}
	else{
		return true;
	}
}

void add_to_map(string key, string value){
	if (exists(key)){
		vector<string> *t = &(inverse_productions[key]);
		t->push_back(value);
	}
	else{
		vector<string> t;
		t.push_back(value);
		inverse_productions[key] = t;
	}
}

void print_vector(vector<string> v){
	if(v.size() == 0){
		cout<<"NULL";
	}
	for (int i = 0; i < v.size(); ++i)
	{
		cout<<v[i]<<",";

	}
	// cout<<endl;
}

void print_map(){
	map<string, vector<string> >::iterator it = inverse_productions.begin();
	for(it; it != inverse_productions.end(); it++)
	{
		string key = (*it).first;
		vector<string> value = (*it).second;
		cout<<key<<" : ";
		print_vector(value);

	}
	cout<<endl;
}

void print_table(){
	cout<<"table"<<endl;
	for(int i = table.size()-1; i >=0; i--){
		cout<<i<<":\t";
		cout<<"|";
		for(int j = 0; j<table[i].size(); j++){
			print_vector(table[i][j]);
			cout<<"|\t";
		}

		cout<<endl;
	}
}

vector<string> combine (vector<string> first_cell, vector<string> second_cell){
	vector<string> result;
	for(int i = 0; i<first_cell.size(); i++){
		for(int j = 0; j<second_cell.size();j++){
			string generated = first_cell[i]+second_cell[j];
			result.push_back(generated);
		}
	}
	return result;
}

int main(){

	while(1){
		cout<<"Enter the productions. Enter -1 to stop."<<endl;
		string left, right;
		cin>>left;
		if (left == "-1"){
			break;
		}
		cout<<left<<"->";
		cin>>right;
		add_to_map(right, left);

	}

	print_map();

	cout<<"Enter the string to check"<<endl;
	cin>>str;
	len = str.length();

	// Filling the bottom most row.
	string::iterator str_it;
	vector< vector<string> > bottom_row;
	for(str_it = str.begin(); str_it != str.end(); str_it++){
		char c = (*str_it);
		string temp(1,c);
		vector<string> non_terminals = inverse_productions[temp];
		bottom_row.push_back(non_terminals);
	}
	table.push_back(bottom_row);

	// print_table();

	for (int l = 1; l < len; l++){
		vector< vector<string> > row;
		int cur_length = l+1;
		for(int index = 0; index <(len - l); index++){
			vector<string> non_terminals;
			set<string> non_terminals2;
			for(int partition = 1; partition<cur_length; partition++){
				int partition_left = partition;
				int partition_right = cur_length - partition_left;
				// cout<<"index:"<<index<<"\t"<<"left:"<<partition_left<<"\t"<<"right:"<<partition_right<<endl;
				// cout<<"firstcell :\t"<<(partition_left - 1)<<","<<index<<endl;
				// cout<<"secondcell :\t"<<(partition_right - 1)<<","<<(partition_left)+index<<endl;
				vector<string> first_cell = table[partition_left - 1][index];
				vector<string> second_cell = table[partition_right -1][(partition_left) + index];
				// print_vector(first_cell);
				// cout<<endl;
				// print_vector(second_cell);
				// cout<<endl;

				vector<string> combined = combine(first_cell, second_cell);
				// print_vector(combined);
				// cout<<endl;
				
				set<string> non_terminals_set;
				for(int i = 0; i<combined.size(); i++){
					vector<string> intermediate_non_terminals = inverse_productions[combined[i]];
					// if(intermediate_non_terminals.size() == 0)
					// 	continue;
					for(int j = 0; j<intermediate_non_terminals.size(); j++){
						non_terminals_set.insert(intermediate_non_terminals[j]);
					}

				}
				vector<string> temp(non_terminals_set.begin(), non_terminals_set.end());
				
				non_terminals.insert(non_terminals.end(), temp.begin(), temp.end());
				non_terminals2.insert(temp.begin(), temp.end());

			}
			// cout<<"final";
			// print_vector(non_terminals);
			// cout<<endl;
			vector<string> non_terminals3(non_terminals2.begin(), non_terminals2.end());
			row.push_back(non_terminals3);

		}
		// print_table();
		table.push_back(row);
		// for(int i = 0 ; i<row.size(); i++){
		// 	print_vector(row[i]);
		// 	cout<<endl;
		// }
		// print_table();
	}

	print_table();
}

/*

S
AB
S
BC
A
BA
A
a
B
CC
B
b
C
AB
C
a
-1



*/