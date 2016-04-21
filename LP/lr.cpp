#include <string.h>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

map<string, vector<string> > productions;

class item{
	public:
	string left;
	string right;
	int dot_position;
	int str_length;

    bool compareItem(item i2){
        if(left == i2.left && right == i2.right && dot_position == i2.dot_position)
            return true;
        return false;
    }

    bool isFinalItem(){
        if(dot_position >= str_length){
            return true;
        }
        return false;
    }

    string nextChar(){
        if(dot_position >= str_length){
            return "_";
        }
        string temp = "";
        temp = temp + right.at(dot_position);
        // cout<<temp;
        return temp;
    }
    void print(){
        cout<<"["<<left<<" -> ";
        for(int i =0 ; i<right.length(); i++){
            if (dot_position == i)      
            {
                /* code */
                cout<<".";

            }
            cout<<right.at(i);
        }
        cout<<"]\n";
    }   
};


class canonical_set{
	public:
	int num;
	vector<item> items;
	map<string, canonical_set> transitions ; 

    void print(){
        cout<<endl<<"-------"<<num<<"------\n";

        for (int i = 0; i< items.size(); i++){
            items[i].print();
        }

        cout<<endl<<"-------------\n";
    }
};

canonical_set first;
int ind;

vector<canonical_set> of_items;


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
       // map<string, vector<string> > p = *m;
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
string start;
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
                cout<<s<<"->"<<right<<endl;
                add_to_map(s, right, productions);
       }
                                        
}

int checkDup(canonical_set c1){

    bool dup = false;
    for(int i = 0; i< of_items.size(); i++){

        canonical_set possible_dup = of_items[i];
        if(possible_dup.items.size() == c1.items.size()){
            bool dup2 = true;
            for(int j = 0; j< possible_dup.items.size(); j++){
                item i1 = possible_dup.items[j];
                bool dup1 = true;
                for(int k = 0; k< c1.items.size(); k++){
                    item i2 = c1.items[k];
                    if ( !i1.compareItem(i2)){
                        dup1 = false;
                        break;
                    }
                }
                if(dup1 == false){
                    dup2 = false;
                    break;
                }
            }
            if (dup2){
                return i;
            }
        }
    }
    if (!dup){
        return -1;
    }
}
int checkDup2(canonical_set c1){

    bool dup = false;
    for(int i = 0; i< of_items.size(); i++){


        canonical_set possible_dup = of_items[i];
        if(possible_dup.items.size() == c1.items.size()){
            // cout<<"size same for"<<i;
            bool dup2 = false;
            for(int j = 0; j< possible_dup.items.size(); j++){
                item i1 = possible_dup.items[j];
                bool dup1 = false;
                for(int k = 0; k< c1.items.size(); k++){
                    item i2 = c1.items[k];
                    if ( i1.compareItem(i2)){
                        dup1 = true;
                        // cout<<"dup1 false"<<endl;
                        // i1.print();
                        // i2.print();
                        break;
                    }
                }
                if(dup1 == true){
                    // cout<<"dup2 false"<<endl;
                    dup2 = true;
                    break;
                }
            }
            if (dup2){
                return i;
            }
        }
    }
    if (!dup){
        return -1;
    }
}



void generate_children(canonical_set &c){
    for(int it = 0; it< c.items.size();it++){
        item i = c.items[it];
        if(i.isFinalItem()){
            continue;
        }
        string left = "";
        left = left +  i.nextChar();
        if (c.transitions.find(left) == c.transitions.end()){ //not yet created. create now
            canonical_set child;
            child.num = ind;
            item iCopy = i;
            iCopy.dot_position++;
            child.items.push_back(iCopy);

            c.transitions[left] =child;

            ind++;

        }
        else{
            item iCopy = i;
            iCopy.dot_position++;
            c.transitions[left].items.push_back(iCopy);

        }
    }
}





void perform_closure(canonical_set &c){
    // c.print();
    // return;
    // vector<item>::iterator it;
    int counter = 0;


    // vector<item> temp_list;
    // for(it = c.items.begin(); it<c.items.end(); it++){
    cout<<c.items.size();
    for(int it = 0; it< c.items.size();it++){
        // cout<<"closre";
        item i = c.items[it];
        // cout<<i.nextChar();
        string left = "";
        left = left +  i.nextChar();
        // cout<<"left"<<left;
        if (productions.find(left) == productions.end()){ // can't find it. in case of a terminal
            continue;
        }
        for(int j = 0; j< productions[left].size(); j++){
            string right = productions[left][j];
            // cout<<endl<<"riight"<<right;
            //checking for duplicates
            bool dup = false;
            for(int k = 0; k< c.items.size(); k++){
                item possible_dup = c.items[k];
                if (possible_dup.left == left && possible_dup.right == right && possible_dup.dot_position == 0){
                    dup = true;
                    break;
                }
            }
            // finished checking for duplicates
            if (!dup){ // if not duplicate then add to items
                item temp;
                temp.left = left;
                temp.right = right;
                temp.dot_position = 0;
                temp.str_length = right.length();
                // temp.print();
                c.items.push_back(temp);
                // cout<<"added item";

                // temp.print();
            }
        }
    }
}

void print_entire_set(){
    cout<<"ENTIRE SET----------"<<endl;
    for(int i = 0; i< of_items.size();i++){
        cout<<i;
        of_items[i].print();
    }
    cout<<"ENTIRE SET----------"<<endl;
}


void expand(canonical_set c){
    c.print();
    generate_children(c);
    // if(c.num >=5){
    //     cout<<"SSSSSSSSSSSSSSSSs"<<endl;
    //     int dup = checkDup(of_items[2]);
    //     cout<<dup;

    //         return;}
    // return;
    map<string, canonical_set >::iterator it = c.transitions.begin();
        for(it; it != c.transitions.end(); it++)
        {
                string key = (*it).first;
                canonical_set value = (*it).second;
                perform_closure(value);
                int dup = checkDup2(value);
                // cout<<"DUPPPPPP"<<dup<<endl;
                if(dup == -1){

                    of_items.push_back(value);
                    expand(value);
                }


        }
}       

int generate(){
    cout<<"GENERATE";
	first.num = ind++;
	cout<<endl<<"--"<<start<<"--"<<endl;
	print_vector(productions[start]);
	int l = productions[start].size();
	for(int i= 0; i< l; i++){
		item temp;
		temp.left = start;
		temp.right = productions[start][i];
		temp.dot_position = 0;
		temp.str_length = (productions[start][i].length());// change later
		first.items.push_back(temp);
        
	}
	first.print();
    perform_closure(first);
    first.print();


    of_items.push_back(first);
    cout<<"XXXXXXXXXXXXXXXXXXXXXX";
    expand(first);
    // expand(of_items[1]);
    print_entire_set();
    // int dup = checkDup2(of_items[6]);
    // cout<<"dup"<<dup;



 //    generate_children(first);
	// map<string, canonical_set >::iterator it = first.transitions.begin();
 //        for(it; it != first.transitions.end(); it++)
 //        {
 //                string key = (*it).first;
 //                canonical_set value = (*it).second;
 //                perform_closure(value);
 //                value.print();

 //        }


}        


                         

int main(){
    ind = 0;
	input();
	print_map(productions);
	generate();	
}

