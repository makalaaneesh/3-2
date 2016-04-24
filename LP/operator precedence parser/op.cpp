#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
using namespace std;
int main()
{
	char term[10],tab[10][10],st[10];int i,m,n,j,k,l;
	cout<<"enter the no of terminal\n";
	cin>>n;
	cout<<"enter terminal\n";
	for(i=0;i<n;i++)
	{
		st[i]=NULL;
		term[i]=NULL;
	    for(j=0;j<n;j++)
	    tab[i][j]=NULL;
        }
	for(i=0;i<n;i++)cin>>term[i];
	cout<<"fill table\n";
	
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			cin>>tab[i][j];
		}
	}
	cout<<"print\n";
	for(i=0;i<n;i++)
	cout<<"\t"<<term[i];
	cout<<endl;
	for(i=0;i<n;i++)
	{
		cout<<term[i];
		for(j=0;j<n;j++)
		{ cout<<"\t"<<tab[i][j];
	    }
	    cout<<endl;
	}
	int row,col,top=0;char str[20];
  cout<<"enter the string to parse\n";
  cin.getline(str,20);
  cin.getline(str,20);
  int ip=0;st[top]='$';
  while(ip<strlen(str))
  {

  	cout<<"stack is [";
  	for(int x = 0; x < top; x++){
  		cout<<st[x];
  	}
  	cout<<"]"<<endl;
  	for(i=0;i<n;i++)
  	{
	  	if(st[top]==term[i])
	  		col=i;
	  	if(str[ip]==term[i])
		  	row=i;
	}
	if(st[top]=='$' && str[ip]=='$')
		{
		cout<<"string is accepted\n";
		break;
	}
	else if(tab[col][row]=='<' || tab[col][row]=='=')
	{
		st[++top]=tab[col][row];
		st[++top]=str[ip];
		cout<<"shift "<<str[ip]<<endl;
		ip++;
		//cout<<"shift operation\n";
	}
	else
	{
	 if(tab[col][row]=='>')
	{
		cout<<"("<<st[top]<<","<<str[ip]<<")\t";
		while(st[top]!='<')--top;
		top=top-1;
		
		cout<<"reduce operation\n";
	}
	else
    {
    cout<<"not accepted\n";
    break;
    }
   }
  }  
}
