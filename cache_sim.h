#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MASK(n) ((1U << (n)) - 1)

/* direct mapped or fully associative*/
typedef enum { dm, fa } cache_map_t;
/* Unified cache or split cache (instruction/data) */
typedef enum { uc, sc } cache_org_t;
typedef enum { instruction, data } access_t;

typedef struct cache_block_t {
  uint8_t byte[64];
  uint32_t tag;
  bool valid;
} cache_block_t;

typedef struct cache_t {
  /* start, end, and is_full to be used only with fully associative FIFO mechanism */
  uint8_t length;
  uint8_t start;
  uint8_t end;
  uint8_t is_full;
  cache_block_t *block;
} cache_t;

typedef struct cache_bits_t {
  uint8_t tag;
  uint8_t index;
  uint8_t offset;
} cache_bits_t;

typedef struct mem_access_t {
  uint32_t address;
  uint32_t tag;
  uint32_t index;
  uint32_t offset;
  access_t accesstype;
} mem_access_t;

typedef struct cache_stat_t {
  uint64_t accesses;
  uint64_t hits;
} cache_stat_t;

int countBits(uint8_t n);

uint32_t get_cache_length(uint32_t cache_size, cache_org_t cache_org);

void set_cache_bits(cache_bits_t *cache_bits,
                  uint32_t cache_length,
                  cache_map_t cache_mapping,
                  cache_org_t cache_org);

int cache_init(cache_t *cache, uint8_t length);

void cache_deinit(cache_t *cache);

void set_access_identifiers(mem_access_t *access, cache_bits_t cache_bits);

int access_cache_dm(cache_t *cache, const mem_access_t *access);

int access_cache_fa(cache_t *cache, const mem_access_t *access);

int is_address_in_fa_cache(const cache_t *cache, const mem_access_t *access);

void transfer_address_to_cache(cache_t *cache, const mem_access_t *access);
