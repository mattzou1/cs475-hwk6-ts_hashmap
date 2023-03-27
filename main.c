
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "ts_hashmap.h"

void* par_operations(void* arg);

int main(int argc, char *argv[]) {
	extern pthread_mutex_t mutex;
	if (argc < 3) {
		printf("Usage: %s <num threads> <hashmap capacity>\n", argv[0]);
		return 1;
	}

  	srand(time(NULL));
	int num_threads = atoi(argv[1]);
	int capacity = (unsigned int) atoi(argv[2]);

	// TODO: Write your test
	ts_hashmap_t *hashmap = initmap(capacity);
	pthread_t *threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
	for (int i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, par_operations, (void*)hashmap);
	}
	for(int i = 0; i < num_threads; i++){
		pthread_join(threads[i], NULL);
	}
	free(threads);
	pthread_mutex_destroy(&mutex);
	printmap(hashmap);
	//free hashmap
	// Free each entry in the table
    for (int i = 0; i < hashmap->capacity; i++) {
        ts_entry_t* entry = hashmap->table[i];
        while (entry != NULL) {
            ts_entry_t* next = entry->next;
            free(entry);
            entry = next;
        }
    }
    // Free the table itself
    free(hashmap->table);
    // Free the hashmap struct
    free(hashmap);
	return 0;
}

void* par_operations(void* arg){
	ts_hashmap_t* hashmap = (ts_hashmap_t*)arg;
	for(int i = 0; i < 100; i++){
		int num = rand() % 3;
		int key = rand() % 100;
		if(num == 0){
			get(hashmap, key);
		}
		else if(num == 1){
			put(hashmap, key, key);
		}
		else{
			del(hashmap, key);
		}
	}
	return NULL;
}
