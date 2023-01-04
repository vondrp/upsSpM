/*
 * Author : Pierre-Henri Symoneaux
 */

#include "hashmap.h"
#include <stdio.h>

static unsigned int ht_calc_hash(char* key)
{
    unsigned int h = 5381;
    while(*(key++))
    {
        h = ((h << 5) + h) + (*key);
    }

    return h;
}

hashtable_t *ht_create(unsigned int capacity)
{
    hashtable_t* hasht = malloc(sizeof(hashtable_t));
    if(!hasht)
    {
        return NULL;
    }

    hasht->table = (hash_elem_t **) malloc(capacity * sizeof(hash_elem_t *));
    if(!(hasht->table)) {
        free(hasht);
        return NULL;
    }
    hasht->capacity = capacity;
    hasht->e_num = 0;
    unsigned int i;
    for(i = 0; i < capacity; i++)
        hasht->table[i] = NULL;
    return hasht;
}

/* 	Store data in the hashtable. If data with the same key are already stored,
	they are overwritten, and return by the function. Else it returns NULL.
*/
void* ht_put(hashtable_t* hasht, char* key, void* data)
{
    if(data == NULL)
        return NULL;
    unsigned int h = ht_calc_hash(key) % hasht->capacity;
    hash_elem_t* e = hasht->table[h];

    while(e != NULL)
    {
        if(!strcmp(e->key, key))
        {
            void* ret = e->data;
            e->data = data;
            return ret;
        }
        e = e->next;
    }

    // Getting here means the key doesn't already exist

    if((e = malloc(sizeof(hash_elem_t)+strlen(key)+1)) == NULL)
        return NULL;
    strcpy(e->key, key);
    e->data = data;

    // Add the element at the beginning of the linked list
    e->next = hasht->table[h];
    hasht->table[h] = e;
    hasht->e_num ++;

    return NULL;
}

/* Retrieve data from the hashtable */
void* ht_get(hashtable_t* hasht, char* key)
{
    if(!hasht) {
        return NULL;
    }

    if(!(hasht->table)) {
        return NULL;
    }
    if(!key) {
        printf("hashmap key cannot be null\n");
    }
    unsigned int h = ht_calc_hash(key) % hasht->capacity;

    hash_elem_t* e = hasht->table[h];
    while(e != NULL)
    {
        if(!strcmp(e->key, key))
            return e->data;
        e = e->next;
    }
    return NULL;
}

/* 	Remove data from the hashtable. Return the data removed from the table
	so that we can free memory if needed */
void* ht_remove(hashtable_t* hasht, char* key)
{
    unsigned int h = ht_calc_hash(key) % hasht->capacity;
    hash_elem_t* e = hasht->table[h];
    hash_elem_t* prev = NULL;
    while(e != NULL)
    {
        if(!strcmp(e->key, key))
        {
            void* ret = e->data;
            if(prev != NULL)
                prev->next = e->next;
            else
                hasht->table[h] = e->next;
            free(e);
            e = NULL;
            hasht->e_num --;
            return ret;
        }
        prev = e;
        e = e->next;
    }
    return NULL;
}

/* List keys. k should have length equals or greater than the number of keys */
void ht_list_keys(hashtable_t* hasht, char** k, size_t len)
{
    if(len < hasht->e_num)
        return;
    int ki = 0; //Index to the current string in **k
    int i = hasht->capacity;
    while(--i >= 0)
    {
        hash_elem_t* e = hasht->table[i];
        while(e)
        {
            k[ki++] = e->key;
            e = e->next;
        }
    }
}

/* 	List values. v should have length equals or greater
	than the number of stored elements */
void ht_list_values(hashtable_t* hasht, void** v, size_t len)
{
    if(len < hasht->e_num)
        return;
    int vi = 0; //Index to the current string in **v
    int i = hasht->capacity;
    while(--i >= 0)
    {
        hash_elem_t* e = hasht->table[i];
        while(e)
        {
            v[vi++] = e->data;
            e = e->next;
        }
    }
}

/* Iterate through table's elements. */
hash_elem_t* ht_iterate(hash_elem_it* iterator)
{
    while(iterator->elem == NULL)
    {
        if(iterator->index < iterator->ht->capacity - 1)
        {
            iterator->index++;
            iterator->elem = iterator->ht->table[iterator->index];
        }
        else
            return NULL;
    }
    hash_elem_t* e = iterator->elem;
    if(e)
        iterator->elem = e->next;
    return e;
}

/* Iterate through keys. */
char* ht_iterate_keys(hash_elem_it* iterator)
{
    hash_elem_t* e = ht_iterate(iterator);
    return (e == NULL ? NULL : e->key);
}

/* Iterate through values. */
void* ht_iterate_values(hash_elem_it* iterator)
{
    hash_elem_t* e = ht_iterate(iterator);
    return (e == NULL ? NULL : e->data);
}

/* 	Removes all elements stored in the hashtable.
	if free_data, all stored datas are also freed.*/
void ht_clear(hashtable_t* hasht, int free_data)
{
    hash_elem_it it = HT_ITERATOR(hasht);
    char* k = ht_iterate_keys(&it);
    while(k != NULL)
    {
        free_data ? free(ht_remove(hasht, k)) : ht_remove(hasht, k);
        k = ht_iterate_keys(&it);
    }
}

/* 	Destroy the hash table, and free memory.
	Data still stored are freed*/
void ht_destroy(hashtable_t* hasht)
{
    ht_clear(hasht, 1); // Delete and free all.
    free(hasht->table);
    free(hasht);
}