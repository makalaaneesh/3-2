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

void left_factor(string key){
	if (!exists(key))
		return;
	vector<string> value = productions[key];
	int min = 10000;
	string str = "";
	for( int i =0; i<value.size()-1;i++){
		for(int j = i+1; j < value.size();j++){
			int c = 0;
			while(value[i][c] == value[j][c])
				c++;
			if (c > 0 && c< min){
				min = c;
				string s = "";
				for( int x = 0; x<c; x++){
					s += value[i][x];
				}
				str = s;
			}
		}
	}
	if(str == "")
		return;
	cout<<"min string is "<<str<<endl;
	vector<string> matched;
	vector<string> not_matched;
	for( int i =0; i<value.size();i++){
		int c = 0;
		int match= 1;
		for(c; c<min; c++){
			if (value[i][c] != str[c]){
				match = 0;
				break;
			}
		}
		cout<<"matched is "<<match<<endl;
		if(match == 0)
			not_matched.push_back(value[i]);
		else{
			string temp ="";
			for(int x = min; x<value[i].length(); x++){
				temp = temp +value[i][x];
			}
			matched.push_back(temp);
			
		}
	}
	string new_key = key+"'";
	string new_prod = str+new_key;
	not_matched.push_back(new_prod);
	cout<<"not macthed ->"<<endl;
	print_vector(not_matched);
	cout<<"matched"<<endl;
	print_vector(matched);
	productions[key] = not_matched;
	productions[new_key] = matched;
	left_factor(new_key);	        
/*		

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
*/
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
		left_factor(key);
		
        }
}

int main(){
	input();
	print_map();
	eliminate();
	print_map();
}
