#ifndef BANIC_HASH_MAP_H
#define BANIC_HASH_MAP_H

#include "../utils.h"

struct HashMapNode {
    void *key;
    void *data;
    struct HashMapNode *next;
};

typedef struct HashMap {
    struct HashMapNode **buckets;
    size_t allocated_buckets;
    size_t size;

    float load_factor;
    size_t (*hash)(const void *element);
    int (*cmp)(const void *key, const void *arg);
} HashMap;

#define _HM_FOREACH(_map, _key, _data, _c)                              \
    for(u64 _i##_c = 0, _keep##_c = 1;                                  \
        _keep##_c && _i##_c < (_map)->allocated_buckets;                \
        _i##_c++)                                                       \
        for(struct HashMapNode *_node##_c = (_map)->buckets[_i##_c];    \
            _keep##_c && _node##_c != NULL;                             \
            _keep##_c = !_keep##_c, _node##_c = _node##_c->next)        \
            for((_key) = _node##_c->key, (_data) = _node##_c->data;     \
                _keep##_c;                                              \
                _keep##_c = !_keep##_c)

#define _HM_FOREACH_DATA(_map, _data, _c)                               \
    for(u64 _i##_c = 0, _keep##_c = 1;                                  \
        _keep##_c && _i##_c < (_map)->allocated_buckets;                \
        _i##_c++)                                                       \
        for(struct HashMapNode *_node##_c = (_map)->buckets[_i##_c];    \
            _keep##_c && _node##_c != NULL;                             \
            (_keep##_c = !_keep##_c, _node##_c = _node##_c->next))      \
            for((_data) = _node##_c->data;                              \
                _keep##_c;                                              \
                _keep##_c = !_keep##_c)

#define hashmap_foreach(map, key, data) _HM_FOREACH(map, key, data, __COUNTER__)
#define hashmap_foreach_data(map, data) _HM_FOREACH_DATA(map, data, __COUNTER__)

void hashmap_create(HashMap *hm, size_t initial_size,
                    size_t (*hash)(const void *element),
                    int (*cmp)(const void *key, const void *arg),
                    float load_factor);
void hashmap_destroy(HashMap *hm, void (*free_key)(void *),
                     void (*free_data)(void *));

void hashmap_add(HashMap *hm, void *key, void *element);
void *hashmap_get(HashMap *hm, const void *key);
void *hashmap_remove(HashMap *hm, const void *key);

size_t string_hash(const void *key);
int string_cmp(const void *key, const void *arg);

#endif
