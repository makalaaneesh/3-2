#include <iostream>
#include <map>
#include <vector>
#include <string.h>

using namespace std;


map<string, vector<string> > productions;
//map<string, vector<string> >firsts;

map<string, vector<string> > follow_graph;



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

void print_map(map<string, vector<string> > &m){
	//map<string, vector<string> > p = *m;
        cout<<"Printing map\n";
	map<string, vector<string> >::iterator it = m.begin();
        for(it; it != m.end(); it++)
        {
                string key = (*it).first;
                vector<string> value = (*it).second;
                cout<<key<<" : ";
                print_vector(value);

        }
        cout<<endl;
}




bool exists(string key, map<string, vector<string> > &m){
        if (m.find(key) == m.end()){
                return false;
        }
        else{
                return true;
        }
}

void add_to_map(string key, string value, map<string, vector<string> > &m){
        if (exists(key, m)){
                vector<string> *t = &(m[key]);
                t->push_back(value);
        }
        else{
                vector<string> t;
                t.push_back(value);
                m[key] = t;
        }
}

vector<string> first(string key){
	vector<string> value = productions[key];
	vector<string> to_return;
	for(int i =0; i<value.size();i++){
		char c = value[i][0];
		if( islower(c) ){
			//cout<<"LOWER"<<value[i][0]<<endl;
			//char c = value[i][0];
			string temp = "";
			temp = temp + c;
			to_return.push_back(temp);
		}
		// else first symbol is a NT
		else if (isupper(c)){
			//cout<<"UPPER"<<value[i][0]<<endl;
			int j = 0;
			//char c = value[i][j];
			int added = 0;
			while( isupper(c) && j< value.size()){
				string temp = "";
				temp = temp + c;
				vector<string> rec_f = first(temp);
				for( int x = 0; x <rec_f.size(); x++){
					if (rec_f[x] != "_"){
						string c = rec_f[x];
						string temp = "";
						temp = temp + c;
						to_return.push_back(temp);
						added++;
					}
				}
				if (rec_f.size() == 1 && rec_f[0] == "_"){
					j++;
					c = value[i][j];
					
				}
				else{
					break;
				}
			}
			if(added == 0 && j<value.size() && islower(value[i][j])){
				char c = value[i][j];
				string temp = "";
				temp = temp +c;
				to_return.push_back(temp);
			}
		}
		else{
			//cout<<"SPECIAL"<<value[i][0]<<endl;
			char c = value[i][0];
			string temp = "";
			temp = temp + c;
			//cout<<"["<<temp<<"]"<<endl;
			to_return.push_back(temp);
		}
	}	
	return to_return;
}
			
vector<string> follow(string key){
		map<string, vector<string> >::iterator it = productions.begin();
	
        for(it; it != productions.end(); it++)
        {
                string key = (*it).first;	
		vector<string> value = productions[key];
		for(int i = 0; i<value.size();i++){
			for(int j = 0; value[i][j]!='\0'; j++){
				char c = value[i][j];
				string temp = "";
				temp = temp + c;
				if(temp == key){
					/*
 * 						if last char of the string or first(j+1) has Epsillon in it, then graph (key->temp)
 * 						else add first(j+1) to it.
 * 					*/
	 				}
	 			}
			}
		}
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
		add_to_map(s, right, productions);
	}	
}
void calculate_first(){

map<string, vector<string> >::iterator it = productions.begin();
        for(it; it != productions.end(); it++)
        {
                string key = (*it).first;
                //vector<string> value = (*it).second;
                //cout<<key<<" : ";
                //print_vector(value);
		vector<string> f = first(key);
		cout<<key<<"->";
		print_vector(f);
		
        }
}

int main(){
	input();
	print_map(productions);
	calculate_first();
	print_map(productions);
}
