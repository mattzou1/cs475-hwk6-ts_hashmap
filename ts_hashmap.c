#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ts_hashmap.h"

pthread_mutex_t mutex;

/**
 * Creates a new thread-safe hashmap. 
 *
 * @param capacity initial capacity of the hashmap.
 * @return a pointer to a new thread-safe hashmap.
 */
ts_hashmap_t *initmap(int capacity) {
  // TODO
  ts_hashmap_t *hashmap = malloc(sizeof(ts_hashmap_t));
  hashmap->table = malloc(capacity * sizeof(ts_entry_t *));
  //initialize each pointer in the table to NULL
  for (int i = 0; i < capacity; i++) {
    hashmap->table[i] = NULL;
  }
  hashmap->capacity = capacity;
  hashmap->size = 0;
  //initialize lock
  pthread_mutex_init(&mutex, NULL);
  return hashmap;
}

/**
 * Obtains the value associated with the given key.
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int get(ts_hashmap_t *map, int key) {
  // TODO
  pthread_mutex_lock(&mutex);
  int index = (unsigned int)key % map->capacity;
  ts_entry_t *entry = map->table[index];
  //loop through linked list at table[index]
  while (entry != NULL) {
    if (entry->key == key){
      pthread_mutex_unlock(&mutex);
      return entry->value;
    }
    entry = entry->next;
  }
  pthread_mutex_unlock(&mutex);
  return INT_MAX;
}

/**
 * Associates a value associated with a given key.
 * @param map a pointer to the map
 * @param key a key
 * @param value a value
 * @return old associated value, or INT_MAX if the key was new
 */
int put(ts_hashmap_t *map, int key, int value) {
  // TODO
  pthread_mutex_lock(&mutex);
  int index = (unsigned int)key % map->capacity;
  ts_entry_t *entry = map->table[index];
  ts_entry_t *prev = NULL;
  //loop through linkedlist at table[index]
  while (entry != NULL) {
    if (entry->key == key) {
      //key already exists, replace the value and return the old value
      int old_value = entry->value;
      entry->value = value;
      pthread_mutex_unlock(&mutex);
      return old_value;
    }
    prev = entry;
    entry = entry->next;
  }
  //key doesn't exist, create new entry and put into linked list
  ts_entry_t *new_entry = malloc(sizeof(ts_entry_t));
  new_entry->key = key;
  new_entry->value = value;
  new_entry->next = NULL;
  
  if (prev != NULL) {
    prev->next = new_entry;
  } 
  else {
    map->table[index] = new_entry;
  }
  
  // Increment the size of the hashmap
  map->size++;
  pthread_mutex_unlock(&mutex);
  return INT_MAX;
}

/**
 * Removes an entry in the map
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int del(ts_hashmap_t *map, int key) {
  // TODO
  pthread_mutex_lock(&mutex);
  int index = (unsigned int)key % map->capacity;
  ts_entry_t *entry = map->table[index];
  ts_entry_t *prev = NULL;
  //loop through linked list at table[index]
  while (entry != NULL) {
    if (entry->key == key) {
      // Found the key, remove the entry and return the associated value
      int value = entry->value;
      if (prev != NULL) {
        prev->next = entry->next;
      } 
      else {
        map->table[index] = entry->next;
      }
      free(entry);
      
      // Decrement the size of the hashmap
      map->size--;
      pthread_mutex_unlock(&mutex);
      return value;
    }
    prev = entry;
    entry = entry->next;
  }
  
  pthread_mutex_unlock(&mutex);
  return INT_MAX;
}


/**
 * @return the load factor of the given map
 */
double lf(ts_hashmap_t *map) {
  return (double) map->size / map->capacity;
}

/**
 * Prints the contents of the map
 */
void printmap(ts_hashmap_t *map) {
  for (int i = 0; i < map->capacity; i++) {
    printf("[%d] -> ", i);
    ts_entry_t *entry = map->table[i];
    while (entry != NULL) {
      printf("(%d,%d)", entry->key, entry->value);
      if (entry->next != NULL)
        printf(" -> ");
      entry = entry->next;
    }
    printf("\n");
  }
}