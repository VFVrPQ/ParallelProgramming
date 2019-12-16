#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <iomanip>
using namespace std;
#define DEBUG(x) cout<<x<<endl;
const int N = 1e4+3;
const int M = 1e9+7;
const int INF = 2147483647;

int n,m,S;
int G[N][N],used[N];
long long dis[N];
void dijkstra(int S){
    for (int i=0;i<n;i++) dis[i] = INF;
    memset(used, 0, sizeof used);//used用来标记是否在Va内

    dis[S]=0;
    while (1){
        int k=-1;
        for (int i=0;i<n;i++){
            if (!used[i] && (k==-1 || dis[k]>dis[i])) k=i;
        }
        if (k==-1) break;
        used[k]=1;
        for (int i=0;i<n;i++){
            if (G[k][i]!= INF && dis[k] + G[k][i] < dis[i]){
                dis[i] = dis[k] + G[k][i];
            }
        }
    }

}
int main()
{
	freopen("2.txt","r",stdin);
	//freopen("out.txt","w",stdout);
    scanf("%d",&n); 
    S=0;
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            G[i][j]=INF;
        }
    }
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            scanf("%d", &G[i][j]);
        }
    }
    dijkstra(S);
    for (int i=0;i<n;i++){
        if (i) printf(" ");
        printf("%lld", dis[i]);
    }
    return 0;
}