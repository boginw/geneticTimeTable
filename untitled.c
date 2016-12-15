#include <stdio.h>
#include <stdlib.h>
#include <pthreadGC2.h>
#include <stdint.h>
 
// Let us create a global variable to change it in threads
int g = 0;
 
// The function to be executed by all threads
void *myThreadFun(void *vargp)
{
    // Store the value argument passed to this thread
    int myid = (intptr_t) vargp;
 
    // Let us create a static variable to observe its changes
    static int s = 0;
 
    // Change static and global variables
    ++s; ++g;
 
    // Print the argument, static and global variables
    printf("Thread ID: %d, Static: %d, Global: %d\n", myid, ++s, ++g);
}
 
int main()
{
    int i;
    pthread_t tid;
 
    // Let us create three threads
    for (i = 0; i < 4; i++)
        pthread_create(&tid, NULL, myThreadFun, (void *)(intptr_t) i);
    
    pthread_exit(NULL);

    printf("Result Global: %d\n", ++g);
    return 0; 
}