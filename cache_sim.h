#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* direct mapped or fully associative*/
typedef enum { dm, fa } cache_map_t;
/* Unified cache or split cache (instruction/data) */
typedef enum { uc, sc } cache_org_t;
typedef enum { instruction, data } access_t;

typedef struct {
  uint32_t address;
  access_t accesstype;
} mem_access_t;

typedef struct {
  uint64_t accesses;
  uint64_t hits;
  // You can declare additional statistics if
  // you like, however you are now allowed to
  // remove the accesses or hits
} cache_stat_t;

/* Tag, index, and offset bits */
typedef struct {
  uint8_t tag;
  uint8_t index;
  uint8_t offset;
} cache_bits_t;

int countBits(uint32_t n);

void set_cache_bits(cache_bits_t *cache_bits,
                  uint32_t cache_size,
                  cache_map_t cache_mapping,
                  cache_org_t cache_org);