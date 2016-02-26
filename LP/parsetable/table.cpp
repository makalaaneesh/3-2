#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <algorithm>

using namespace std;


map<string, vector<string> > productions;
map<string, vector<string> >firsts;
struct node{
	vector<string> edges;
	vector<string> follow_list;
};
map<string, node> follow_graph;
string start = "";
map<string, map<string, string> >parse_table;


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

void print_parse_table(map<string, map<string, string> > &m){
	map<string , map<string, string> >::iterator it = m.begin();
	for(it; it!=m.end();it++){
		string key = (*it).first;
		map<string,string> value = (*it).second;
		map<string, string>::iterator it2 = value.begin();
		cout<<key<<"->";
		for(it2; it2!=value.end(); it2++){
			string key2 = (*it2).first;
			string value2 = (*it2).second;
			cout<<key2<<":["<<value2<<"]\t";
		}
		cout<<endl;
	}

}


bool exists(string key, map<string, map<string,string> > &m){
	
        if (m.find(key) == m.end()){
                return false;
        }
        else{
                return true;
        }
}

bool exists(string key, map<string, vector<string> > &m){
        if (m.find(key) == m.end()){
                return false;
        }
        else{
                return true;
        }
}

bool exists(string key, vector<string> v){
	if(find(v.begin(), v.end(),key)!=v.end()){
		return true;
	}
	else{
		return false;
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

// void add_edge(string left, string right, map<string, node> &m){
// 	if (exists(left, m)){
// 			node *n = &(m[key]);
//             vector<string> *t = &(m[key]);
//             t->push_back(value);
// 	}
//     else{
//             vector<string> t;
//             t.push_back(value);
//             m[key] = t;
//     }
// }

vector<string> first(string key, vector<string> value){

	//vector<string> value = productions[key];
	vector<string> to_return;
	if (! isupper(key[0])){
		to_return.push_back(key);
		return to_return;
	}
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
				vector<string> rec_f = first(temp, productions[temp]);
				for( int x = 0; x <rec_f.size(); x++){
					if (rec_f[x] != "_"){
						string c = rec_f[x];
						string temp = "";
						temp = temp + c;
						to_return.push_back(temp);
						added++;
					}
				}
				//if (rec_f.size() == 1 && rec_f[0] == "_"){
				if(exists("_",rec_f)){
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

	vector<string> to_return;
	if (key == start)
		to_return.push_back("$");
		map<string, vector<string> >::iterator it = productions.begin();
	
    for(it; it != productions.end(); it++){
    	string _key = (*it).first;	
		vector<string> value = productions[_key];
		for(int i = 0; i<value.size();i++){
			for(int j = 0; j<value[i].length(); j++){
				char c = value[i][j];
				string temp = "";
				temp = temp + c;
				if(temp == key){
					// cout<<_key<<endl;
					if (j+1 >= value[i].length()){
						// add_egde(_key, key, follow_graph);
						if (key != _key){
							// cout<<"Calling follow for "<<_key<<endl;
							vector<string> fo = follow(_key);
							// print_vector(fo);
							for(int x = 0; x<fo.size(); x++){
								if(fo[x] != "_")
									if (!exists(fo[x], to_return))
										to_return.push_back(fo[x]);
							}
						}
						
					}
					else{
						char next = value[i][j+1];
						string n = "";
						n = n + next;
						vector<string> f = first(n, productions[n]);
						for(int x = 0; x<f.size(); x++){
							if(f[x] != "_")
								if (!exists(f[x], to_return))
									to_return.push_back(f[x]);
						}
						if (exists("_",f)){
							// add_egde(_key, key, follow_graph);

							if (key != _key){
							
							vector<string> fo = follow(_key);
							// cout<<"Calling follow for "<<_key;
							// print_vector(fo);
							for(int x = 0; x<fo.size(); x++){
								if(fo[x] != "_")
									if (!exists(fo[x], to_return))
										to_return.push_back(fo[x]);
								}
							}
						}
					}
					/*
 * 						if last char of the string or first(j+1) has Epsillon in it, then graph (key->temp)
 * 						else add first(j+1) to it.
 * 					*/
 				}
 			}
		}
	}
	return to_return;
}

void add_to_parse_table(string nt, string t, string p){
	
        if (exists(nt, parse_table)){
                map<string, string>*m = &(parse_table[nt]);
                (*m)[t] = p;
        }
        else{
                map<string,string> m;
                m[t] = p;
		parse_table[nt] = m;
	}
}

void construct_parse_table(){
	//add_to_parse_table("E","i","TA");
	//print_parse_table(parse_table);
	
	map<string, vector<string> >::iterator it = productions.begin();
        for(it; it != productions.end(); it++)
        {
                string key = (*it).first;
                vector<string> value = (*it).second;
                //cout<<key<<" : ";
                //print_vector(value);
		int i;
		for( i = 0 ; i< value.size(); i++){
			string right = value[i];
			vector<string> temp;
			temp.push_back(right);
			vector<string> f = first("X",temp);
			
			//print_vector(f);
			for(int j = 0; j< f.size(); j++){
				if(f[j] != "_"){
					add_to_parse_table(key, f[j], right);
				}
			}
			if (exists("_", f)){
				vector<string> fo = follow(key);
				for(int j = 0; j< fo.size(); j++){
					add_to_parse_table(key, fo[j], right);
				}
			} 
			//break;
		}
	}
	cout<<"---------------PARSE TABLE--------------\n";	
	print_parse_table(parse_table);
	cout<<"--------------------x-------------------\n";
}

void input(){
	cin>>start;
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
void calculate_follow(){


map<string, vector<string> >::iterator it = productions.begin();
        for(it; it != productions.end(); it++)
        {
                string key = (*it).first;
                //vector<string> value = (*it).second;
                //cout<<key<<" : ";
                //print_vector(value);
        // if(key == "E"){
			vector<string> f = follow(key);
			cout<<"follow("<<key<<")=";
			print_vector(f);
			// break;
		// }	
		
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
        // if(key == "E"){
			vector<string> f = first(key,productions[key]);
			cout<<"first("<<key<<")=";
			print_vector(f);
			// break;
		// }	
		
        }
}
int main(){
	input();
	print_map(productions);
	calculate_follow();
	calculate_first();
	construct_parse_table();
	// print_map(productions);
}
