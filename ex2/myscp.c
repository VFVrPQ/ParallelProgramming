#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

int main(int argc, char *argv[]){
        //if (argc<2) {
        //      printf("params error!");
        //      return -1;
        //}
        //string filename = argv[1];
        string cmd = "scp -r ~/SA19011125 pp11@node3:~"; 
        system(cmd.c_str());
        printf("%s\n", cmd.c_str());
        cmd = "scp -r ~/SA19011125 pp11@node4:~";
        system(cmd.c_str());
        printf("%s\n", cmd.c_str());
        return 0;
}