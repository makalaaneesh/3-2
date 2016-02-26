#include <iostream>
#include <map>
#include <vector>
#include <string.h>

using namespace std;


map<string, vector<string> > productions;


void print_vector(vector<string> v){
        if(v.size() == 0){
                cout<<"NULL";
        }
        for (int i = 0; i < v.size(); ++i)
        {
                cout<<v[i]<<",";

        }
         cout<<endl;
}

void print_map(){
	//map<string, vector<string> > p = *m;
        cout<<"Printing map\n";
	map<string, vector<string> >::iterator it = productions.begin();
        for(it; it != productions.end(); it++)
        {
                string key = (*it).first;
                vector<string> value = (*it).second;
                cout<<key<<" : ";
                print_vector(value);

        }
        cout<<endl;
}




bool exists(string key){
        if (productions.find(key) == productions.end()){
                return false;
        }
        else{
                return true;
        }
}

void add_to_map(string key, string value){
        if (exists(key)){
                vector<string> *t = &(productions[key]);
                t->push_back(value);
        }
        else{
                vector<string> t;
                t.push_back(value);
                productions[key] = t;
        }
}

void eliminate_left_recursion(string key){
	if (!exists(key))
		return;
	vector<string> value = productions[key];
	vector<string> starting_with_key;
	vector<string> not_starting_with_key;
	for(int i = 0; i<value.size(); i++){
		int c = 0;
		while(key[c] == value[i][c])
			c++;
		if (c == 0){
			not_starting_with_key.push_back(value[i]);
		}
		else if (c == key.length()){
			string new_prod = "";
			for(int j = c; j<value[i].length(); j++){
				new_prod = new_prod + value[i][j];
			}
			starting_with_key.push_back(new_prod);
		}
		
	}
	if (starting_with_key.size() == 0){
		cout<<"No left recursion for "<<key<<endl;
		return ;
	}
	string new_key = key+"'";
	vector<string> new_value;
	for( int i = 0; i< starting_with_key.size(); i++){
		starting_with_key[i] += new_key;
	}
	for(int i = 0; i<not_starting_with_key.size(); i++){
		not_starting_with_key[i] += new_key;
	}
	starting_with_key.push_back("_");
	productions[key] = not_starting_with_key;
	productions[new_key] = starting_with_key;
}
			

void input(){
	cout<<"exit to stop ";
	while(1){
		string s;
		cin>>s;
		if(s =="exit"){
			break;
		}
		cout<<s<<"->";
		string right;
		cin>>right;
		//cout<<s<<"->"<<right<<endl;
		add_to_map(s, right);
	}	
}
void eliminate(){

map<string, vector<string> >::iterator it = productions.begin();
        for(it; it != productions.end(); it++)
        {
                string key = (*it).first;
                //vector<string> value = (*it).second;
                //cout<<key<<" : ";
                //print_vector(value);
		eliminate_left_recursion(key);
        }
}

int main(){
	input();
	print_map();
	eliminate();
	print_map();
}
