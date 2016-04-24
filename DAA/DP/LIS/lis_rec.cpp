#include <iostream>
using namespace std;
int count = 0;
int LIS(int arr[], int start, int end, int curvalue, int c){
    if(c > count){
        count = c;
    }
    for(int i = start; i< end; i++){
        if(arr[i] > curvalue){
            LIS(arr, i+1, end, arr[i], c+1);
        }
    }
}


int main(){
    int t;
    cin>>t;
    for(int i = 0; i< t; i++){
        int n;
        cin>>n;
        int arr[n];
        for(int j = 0; j< n; j++){
            cin>>arr[j];
        }
        count = 0;
        LIS(arr, 0, n,-1, 0);
        cout<<count;
    }
}

/*



1
6
5 2 7 4 3 8

1
16
0 8 4 12 2 10 6 14 1 9 5 13 3 11 7 15

*/