/****************************************************************************************************\
 * FILE: hashmap.h                                                                                  *
 * AUTHOR: Michael Kamau                                                                            *
 *                                                                                                  *
 * PURPOSE: The program is an implementation of a hash map that stores data in key value pairs,     *
 *          the program uses xxh64 algorithm for hashing and foward chaining for collision          *
 *          handling                                                                                *
 *                                                                                                  *
 *  USAGE: To instantiate the program :                                                             *
 *                `HashMap *map = create_hashmap(capacity)`                                         *
 *                 capacity is the number of buckets to create                                      *
 *                                                                                                  *
 *                  The more the capacity the less the chances of collisions                        *
 *                                                                                                  *
 *                 inserting and reading values from the map:                                       *
 *                    -inserting:                                                                   *
 *                               insert_to_hashmap(map, key, len ,value)                            *
 *                    -reading:                                                                     *
 *                              get_hasmap_value(map, key, len)                                     *
 *                                                                                                  *
 \***************************************************************************************************/


#ifndef C4C_HASHMAP
#define C4C_HASHMAP

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "utils.hpp"

/**
 * The constants below are a set of carefully chosed prime numbers
 * by the designers of the xxh64 algorithm.
 * 
 * I have not memorized them and refered directly from its documentation
 */
#define XXH_PRIME64_1 11400714785074694791ULL
#define XXH_PRIME64_2 14029467366897019727ULL
#define XXH_PRIME64_3 1609587929392839161ULL
#define XXH_PRIME64_4 9650029242287828579ULL
#define XXH_PRIME64_5 2870177450012600261ULL

#define rotl64(x, r) (((x) << (r)) | ((x) >> (64 - (r))))

struct HashNode
{
    void *key;
    size_t key_len;

    void *value;

    uint64_t hash;
    HashNode *next; /* For forward chaining */
};

struct HashMap
{
    HashNode **buckets; /* array of bucket heads */
    size_t capacity;
    size_t size;

    uint64_t seed;
};

/**
 This implementation follows the canonical XXH64 design. The goal is not
cryptographic security, but extremely fast, well-distributed hashing for
hash tables, interners, and systems code.

High-level structure:

  helpers -> bulk processing -> tail processing -> final avalanche

Key ideas to keep in mind when reading or modifying this code:

1) Safe raw memory access
   ----------------------
   All reads use memcpy-based helpers (read_u64/read_u32) instead of casts.
   This avoids undefined behavior from unaligned access and strict aliasing,
   while still compiling to optimal machine code.

2) Bit rotation
   ------------
   Rotations are used instead of shifts to preserve information while moving
   entropy across bit positions. This is essential for avalanche behavior.

3) Core mixing primitive (xxh64_round)
   -----------------------------------
   This is the atomic mixing step:
     - multiply input by a large prime
     - add to accumulator
     - rotate
     - multiply again
   It is reused conceptually throughout the algorithm.

4) Parallel accumulation (v1..v4)
   -------------------------------
   Large inputs are processed using four independent 64-bit accumulators.
   This enables instruction-level parallelism, hides multiplication latency,
   and improves early diffusion. Input is consumed in 32-byte stripes
   (4 × 64-bit words) for cache-friendly, branch-free processing.

5) Accumulator merge
   ------------------
   After bulk processing, the four lanes are rotated differently, combined,
   and re-mixed into a single hash value. This step collapses parallel state
   while preserving entropy from each lane.

6) Small-input path
----------------
For inputs smaller than 32 bytes, no parallel lanes are used. The hash
starts from a seed + prime base to ensure reasonable diffusion without
   unnecessary overhead.

7) Length mixing
   --------------
   The total input length is always added to the hash. This prevents prefix
   collisions and ensures that size is part of the hash identity.

8) Tail processing
   ---------------
   Remaining bytes are processed in descending order of size:
     - 8-byte chunks
     - 4-byte chunk
     - individual bytes
   Each step uses appropriate mixing to ensure every byte influences the
   final hash. Many hash bugs occur in this phase if done incorrectly.

9) Final avalanche
   ----------------
   A sequence of XOR-shifts and multiplications destroys any remaining
   structure and guarantees strong diffusion. Even low-entropy or patterned
   inputs should produce well-distributed output.
 */

static inline uint64_t read_u64(const void *ptr)
{
    uint64_t val;

    memcpy(&val, ptr, sizeof(val));

    return val;
}

static inline uint32_t read_u32(const void *ptr)
{
    uint32_t val;

    memcpy(&val, ptr, sizeof(val));

    return val;
}

static inline uint64_t xxh64_round(uint64_t acc, uint64_t input)
{
    acc += input * XXH_PRIME64_2;
    acc = rotl64(acc, 31);
    acc *= XXH_PRIME64_1;

    return acc;
}

static inline uint64_t xxh64(const void *input, size_t len, uint64_t seed)
{
    const uint8_t *p = (const uint8_t *)input;
    const uint8_t *end = p + len;
    uint64_t h64;

    if (len >= 32)
    {
        const uint8_t *limit = end - 32;

        uint64_t v1 = seed + XXH_PRIME64_1 + XXH_PRIME64_2;
        uint64_t v2 = seed + XXH_PRIME64_2;
        uint64_t v3 = seed + 0;
        uint64_t v4 = seed - XXH_PRIME64_1;

        do
        {
            v1 = xxh64_round(v1, read_u64(p));
            p += 8;
            v2 = xxh64_round(v2, read_u64(p));
            p += 8;
            v3 = xxh64_round(v3, read_u64(p));
            p += 8;
            v4 = xxh64_round(v4, read_u64(p));
            p += 8;
        } while (p <= limit);

        h64 =
            rotl64(v1, 1) +
            rotl64(v2, 7) +
            rotl64(v3, 12) +
            rotl64(v4, 18);

        h64 ^= xxh64_round(0, v1);
        h64 *= XXH_PRIME64_1;

        h64 ^= xxh64_round(0, v2);
        h64 *= XXH_PRIME64_1;

        h64 ^= xxh64_round(0, v3);
        h64 *= XXH_PRIME64_1;

        h64 ^= xxh64_round(0, v4);
        h64 *= XXH_PRIME64_1;
    }
    else
    {
        h64 = seed + XXH_PRIME64_5;
    }
    h64 *= len;

    while (p + 8 <= end)
    {
        uint64_t k1 = xxh64_round(0, read_u64(p));
        h64 ^= k1;
        h64 = rotl64(h64, 27) * XXH_PRIME64_1 + XXH_PRIME64_4;
        p += 8;
    }

    if (p + 4 <= end)
    {
        h64 ^= (uint64_t)read_u32(p) * XXH_PRIME64_1;
        h64 = rotl64(h64, 23) * XXH_PRIME64_2 + XXH_PRIME64_3;
        p += 4;
    }
    while (p < end)
    {
        h64 ^= (*p) * XXH_PRIME64_5;
        h64 = rotl64(h64, 11) * XXH_PRIME64_1;
        p++;
    }

    h64 ^= h64 >> 33;
    h64 *= XXH_PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= XXH_PRIME64_3;
    h64 ^= h64 >> 32;

    return h64;
}

/**
 * This function initializes the hashmap
 * It takes one parametere:
 * 
 * - capacity -> the number of buckets to be created
 * 
 * Notice that we use calloc, because we want the bucket to be zerod out
 * so that setting a key at an index found by the hashing algo works well
 * 
 */
HashMap *create_hash_map(size_t capacity)
{

    HashMap *hash_map = (HashMap *)malloc(sizeof(HashMap));
    hash_map->capacity = capacity;
    hash_map->seed = 0;
    hash_map->size = 0;
    hash_map->buckets = (HashNode**)calloc(capacity, sizeof(HashNode*));

    return hash_map;
}

/**
 * This function inserts a key and value to the hash map
 * It takes four parameters:
 * - map -> a pointer to the map that is being inserted to
 * - key -> the key
 * - len -> the length of the key, we require this for dynamic memory allocation
 * - value -> This is the value of the item being saved in key
 * 
 * Notice that we use void for generics, 
 */
bool insert_to_hashmap(HashMap *map, const void *key, size_t len, void *value)
{
    uint64_t hash = xxh64(key, len, map->seed);
    uint64_t idx = hash % map->capacity;

    HashNode *node = (HashNode *)malloc(sizeof(HashNode));

    if(!node)
    {
        return false;
    }

    node->hash = hash;
    node->key = malloc(len * sizeof(char));

    if(!node->key)
    {
        free(node);
        return false;
    }
    node->key_len = len;
    memcpy(node->key, key, len);
    node->value = value;

    node->next = map->buckets[idx];
    map->buckets[idx] = node;
    map->size++;

    return true;
}


/**
 * This function returns the value of a given key.
 * It takes three parameters:
 * - map -> the map to search the key from
 * - key -> the key to search for
 * - len -> the length of the key
 * 
 * we need the length of the key to check against the length store in the 
 * node i.e node->len,
 * why? We just want to be super sure that everything about the key matches with
 * what we stored in the hashmap
 * 
 * Notice that we use void, for generic, this means when using this function
 * it is the responsibility of the caller to cast the data to the appropriate 
 * type. Failure to this will result to unwanted results
 */
void *get_hash_map_value(HashMap *map, const void *key, size_t len)
{
    uint64_t hash = xxh64(key, len, map->seed);
    uint64_t idx = hash % map->capacity;

    for (HashNode *node = map->buckets[idx]; node != NULL; node = node->next)
    {
         DEBUG_PRINT("The key is: ", (char *)node->key);
         DEBUG_PRINT("Being checked against key: ", (char *)key);
         DEBUG_PRINT("The length of the key is: ",len);

        if (node->key_len == len && memcmp(node->key, key, len) == 0)
        {
            return node->value;
        }
    }
    return NULL;
}


/**
 * This function clears the map
 */
void clear_hash_map(HashMap *map)
{
    if(!map)
    {
        return;
    }

    for(int i = 0; i < map->capacity; i++)
    {
        HashNode *temp = map->buckets[i];

        while(temp)
        {
            HashNode *next = temp->next;
            free(temp->key);
            free(temp);
            temp = next;
        }
    }

    free(map->buckets);
    free(map);

}

#endif // C4C_HASHMAP