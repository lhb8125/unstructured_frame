#include <mpi.h>
#include <stdio.h>
 
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
 
    int world_size;  //总进程数<br>　　 
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int wrank;  //进程等级号<br>　　 
    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
    if (wrank!=1) //进程不为1的执行该操作
        printf("Rang %d sur %d.\n", wrank, world_size);
 
    MPI_Finalize();
 
    return 0;
}
