#include <iostream>
using namespace std;


int LIS(int arr[], int l[], int n){
    
    for(int i = 0; i< n; i++){
        int max_val = 1;
        for(int j =0; j< i; j++){
            if (arr[j] < arr[i]){
                if(l[j]+ 1 > max_val){
                    max_val = l[j]+1;
                }
            }
        }
        l[i] = max_val;
        cout<<"l["<<i<<"] = "<<l[i]<<endl;
    }
    return l[n-1];
}

int main() {
	//code
	
	int t;
    cin>>t;
    for(int i = 0; i< t; i++){
        int n;
        cin>>n;
        int arr[n];
        int l[n];
        for(int x = 0; x< n; x++){
            l[x] = 0;
        }
        for(int j = 0; j< n; j++){
            cin>>arr[j];
        }
        int res = LIS(arr, l, n);
        cout<<res;
    }
	return 0;
}