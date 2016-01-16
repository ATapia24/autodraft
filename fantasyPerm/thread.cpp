#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>

using namespace std;

struct taskData{
	int number;
	char letter;
};

void *task(void *arg){
	
	taskData data = *(taskData*)arg;
      
	cout<< data.letter << " " << data.number << endl;
}

int main(){
	
	taskData data;
	data.number = 1;
	data.letter = 'a';

    pthread_t thread1, thread2;
    int i1,i2;
    i1 = pthread_create( &thread1, NULL, task, &data);
    i2 = pthread_create( &thread2, NULL, task, &data);

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);

    return 0;

}
