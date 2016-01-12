// http://web.cs.ucdavis.edu/~rogaway/classes/120/winter12/CYK.pdf
#include <iostream>
#include <string.h>
#include <vector>
#include <map>

using namespace std;

map<string, vector<string> > inverse_productions;

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
	for (int i = 0; i < v.size(); ++i)
	{
		cout<<v[i]<<",";

	}
	cout<<endl;
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
}


int main(){

	add_to_map("a", "A");
	add_to_map("a", "C");
	add_to_map("b", "B");

	print_map();

}