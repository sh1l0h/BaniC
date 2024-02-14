#include "../../include/data_structures/hash_map.h"

void hashmap_create(HashMap *hm, size_t initial_size,
                    size_t (*hash)(const void *element), 
                    int (*cmp)(const void *key, const void *arg),
                    float load_factor)
{
    hm->buckets = calloc(initial_size, sizeof(struct HashMapNode *));
    hm->allocated_buckets = initial_size;
    hm->size = 0;

    hm->load_factor = load_factor;
    hm->hash = hash;
    hm->cmp = cmp;
}

void hashmap_destroy(HashMap *hm, void (*free_key)(void *),
                     void (*free_data)(void *))
{
    for (size_t i = 0; i < hm->allocated_buckets; i++) {
        struct HashMapNode *curr = hm->buckets[i];

        while (curr != NULL) {
            struct HashMapNode *temp = curr;
            curr = curr->next;

            if (free_key != NULL)
                free_key(temp->key);

            if (free_data != NULL)
                free_data(temp->data);
            
            free(temp);
        }
    }

    free(hm->buckets);
}

static void hashmap_resize(HashMap *hm, size_t new_buckets_size)
{
    struct HashMapNode **new_buckets = calloc(new_buckets_size,
                                       sizeof(struct HashMapNode *));

    for (size_t i = 0; i < hm->allocated_buckets; i++) {
        struct HashMapNode *curr = hm->buckets[i];
        while (curr != NULL) {
            struct HashMapNode *next = curr->next;

            size_t index = hm->hash(curr->key) % new_buckets_size;
            curr->next = new_buckets[index];
            new_buckets[index] = curr;

            curr = next;
        }
    }

    free(hm->buckets);
    hm->buckets = new_buckets;
    hm->allocated_buckets = new_buckets_size;
}

void hashmap_add(HashMap *hm, void *key, void *element)
{
    size_t index = hm->hash(key) % hm->allocated_buckets;

    struct HashMapNode *new_node = malloc(sizeof(struct HashMapNode));
    new_node->key = key;
    new_node->data = element;

    new_node->next = hm->buckets[index];
    hm->buckets[index] = new_node;
    hm->size++;

    if ((float) hm->size / hm->allocated_buckets > hm->load_factor)
        hashmap_resize(hm, hm->allocated_buckets * 2);
}

void *hashmap_get(HashMap *hm, const void *key)
{
    size_t index = hm->hash(key) % hm->allocated_buckets;

    struct HashMapNode *curr = hm->buckets[index];

    while (curr != NULL) {

        if (!hm->cmp(curr->key, key))
            return curr->data;

        curr = curr->next;
    }

    return NULL;
}

void *hashmap_remove(HashMap *hm, const void *key)
{
    size_t index = hm->hash(key) % hm->allocated_buckets;

    struct HashMapNode *curr = hm->buckets[index];
    struct HashMapNode *prev = NULL;

    while (curr != NULL) {
        if (!hm->cmp(curr->key, key)) {
            struct HashMapNode *next = curr->next;
            hm->size--;

            if (prev == NULL)
                hm->buckets[index] = next;
            else
                prev->next = next;

            void *tmp = curr->data;
            free(curr);
            return tmp;
        }

        prev = curr;
        curr = curr->next;
    }

    return NULL;
}

//source: https://stackoverflow.com/questions/8317508/hash-function-for-a-string
size_t string_hash(const void *key)
{
    const char *string = key;
    size_t result = 37ULL;

    while (*string != '\0') {
        result = (result * 54059ULL) ^ (*string * 76963ULL);
        string++;
    }

    return result;
}

int string_cmp(const void *key, const void *arg)
{
    return strcmp(key, arg);
}
