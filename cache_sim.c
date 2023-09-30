#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * For running tests from a seperate file, structs and functions are
 * defined in a header file. For sake of assignment submission,
 * define them here.
 */
// #define RUN_UNIT_TESTS 0
// #ifdef RUN_UNIT_TESTS
// #include "cache_sim.h"
// #else

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
  uint64_t evicts;
} cache_stat_t;

int countBits(uint8_t n);

bool is_power_of_two(uint32_t n);

int verify_cache_size(uint32_t cache_size);

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

// #endif

uint32_t block_size = 64;
cache_stat_t cache_statistics;

static void print_cache_hit(const mem_access_t *access);
static void print_cache_miss(const mem_access_t *access);

static void print_cache_hit(const mem_access_t *access)
{
  printf("0x%x - Cache hit\n", access->address);
}

static void print_cache_miss(const mem_access_t *access)
{
  printf("0x%x - Cache miss\n", access->address);
}

/* Reads a memory access from the trace file and returns
 * 1) access type (instruction or data access
 * 2) memory address
 */
mem_access_t read_transaction(FILE* ptr_file) {
  char type;
  mem_access_t access;

  if (fscanf(ptr_file, "%c %x\n", &type, &access.address) == 2) {
    if (type != 'I' && type != 'D') {
      printf("Unkown access type\n");
      exit(0);
    }
    access.accesstype = (type == 'I') ? instruction : data;
    return access;
  }

  /* If there are no more entries in the file,
   * return an address 0 that will terminate the infinite loop in main
   */
  access.address = 0;
  return access;
}

bool is_power_of_two(uint32_t n) {
    return n && !(n & (n - 1));
}

int verify_cache_size(uint32_t cache_size)
{
  if (cache_size < 128) {
    printf("Cache size is too small, minimum of 128 bytes\n");
    return -1;
  } else if (cache_size > 4096) {
    printf("Cache size is too large, maximum of 4096 bytes\n");
    return -1;
  } else if (!is_power_of_two(cache_size)) {
    printf("Invalid cache size. It must be a power of 2\n");
    return -1;
  }

  return 0;
}

int countBits(uint8_t n) {
  int count = 0;

  while (n > 1) {
      count++;
      n >>= 1;
  }

  return count;
}

uint32_t get_cache_length(uint32_t cache_size, cache_org_t cache_org)
{
  if (cache_org == uc) {
    /* Unifed cache */
    return(cache_size / block_size);
  } else {
    /* split cache */
    return(cache_size / block_size / 2);
  }
}

void set_cache_bits(cache_bits_t *cache_bits,
                  uint32_t cache_length,
                  cache_map_t cache_mapping,
                  cache_org_t cache_org)
{
  cache_bits->offset = countBits(block_size);

  if (cache_mapping == dm) {
    /* direct mapped */
    cache_bits->index = countBits(cache_length);
  } else {
    /* fully associative */
    cache_bits->index = 0;
  }

  cache_bits->tag = 32 - cache_bits->offset - cache_bits->index;
}

int cache_init(cache_t *cache, uint8_t length)
{
  /* Each cache block includes:
   *  1 valid bit
   *  32 tag bits
   *  64 bytes of data
   */
  cache->block = (cache_block_t *)calloc((size_t)length, sizeof(cache_block_t));

  if (cache->block == NULL) {
    printf("cache memory allocation failed\n");
    return -1;
  }

  /* The following will be used for searching through a fully associative cache */
  cache->length = length;
  cache->start = 0;
  cache->end = 0;
  cache->is_full = false;

  return 0;
}

void cache_deinit(cache_t *cache)
{
  cache->block->tag = 0;
  cache->block->valid = 0;
  free(cache->block);

  cache->length = 0;
  cache->is_full = 0;
  cache->end = 0;
  cache->start = 0;
}

void set_access_identifiers(mem_access_t *access, cache_bits_t cache_bits)
{
  /* byte offset: mask off lower n bits, n being cache_bits.offset */
  access->offset = access->address & MASK(cache_bits.offset);

  /* index: shift address to right by cache_bits.offset number of bits, mask off index*/
  access->index = (access->address >> cache_bits.offset) & MASK(cache_bits.index);

  /* tag: same as was done for index, except now for the tag bits */
  access->tag = (access->address >>
    (cache_bits.index + cache_bits.offset)) & MASK(cache_bits.tag);

  // printf("set_access_identifiers 0x%x - idx: %x, tag: %x, offset: %x\n",
  //   access->address ,access->index, access->tag, access->offset);
}

int access_cache_dm(cache_t *cache, const mem_access_t * access)
{
  /* First check valid bit of index */
  if (cache->block[access->index].valid) {
    /* Valid bit set, so next compare tags */
    if (cache->block[access->index].tag == access->tag) {
      /* Valid bit set and tags match, cache hit! */
      // print_cache_hit(access);
      return 1;
    } else {
      /* Tags do not match, cache miss. Overwrite new address to this block, update tag */
      cache->block[access->index].tag = access->tag;
      // print_cache_miss(access);
      cache_statistics.evicts++;
      return 0;
    }
  } else {
    /* Valid bit is not set, cache miss. Write address to cache */
    cache->block[access->index].valid = true;
    cache->block[access->index].tag = access->tag;
    // print_cache_miss(access);
    return 0;
  }

  return -1;
}

int is_address_in_fa_cache(const cache_t *cache, const mem_access_t *access)
{
  size_t index = cache->start;

  if (index == cache->end && !cache->is_full) {
    // print_cache_miss(access);
    return 0;
  }

  do {
    /* First check that valid bit is set */
    if (cache->block[index].valid == true) {
      /* Next, compare the tags */
      if (cache->block[index].tag == access->tag) {
        // print_cache_hit(access);
        return 1;
      }
    }
    index = (index + 1) % cache->length;
  } while ( index != cache->end );

  /* Cache miss */
  return 0;
}

void transfer_address_to_cache(cache_t *cache, const mem_access_t *access)
{

  /* Check if cache is full */
  if (cache->is_full) {
    /* update start, must evict */
    cache->start = (cache->start + 1) % cache->length;
    cache_statistics.evicts++;
  }

  /* Transfer address, ignoring offset bytes for now */
  cache->block[cache->end].valid = true;
  cache->block[cache->end].tag = access->tag;

  /* Update end pointer, this will wrap around to the
  beginning once the full length of the cache is reached */
  cache->end = (cache->end + 1) % cache->length;

  cache->is_full = (cache->end == cache->start);
}

/**
 * A fully associative cache will be treated as a ring buffer
*/
int access_cache_fa(cache_t *cache, const mem_access_t *access)
{
  /* Iterate through all cache entries */
  if (is_address_in_fa_cache(cache, access)) {
    /* Cache hit */
    return 1;
  } else { /* Miss */
    transfer_address_to_cache(cache, access);
    return 0;
  }
}

// #ifndef RUN_UNIT_TESTS
void main(int argc, char** argv)
{
  int ret;
  uint32_t cache_size;
  uint32_t cache_length;

  cache_map_t cache_mapping;
  cache_org_t cache_org;
  cache_bits_t cache_bits;
  cache_t cache;
  cache_t cache_data;
  cache_t cache_inst;

  // Reset statistics:
  memset(&cache_statistics, 0, sizeof(cache_stat_t));

  /* Read command-line parameters and initialize:
   * cache_size, cache_mapping cache_org, also optional input file
   */
  if (argc < 4) { /* argc should be 2 for correct execution */
    printf(
        "Usage: ./cache_sim [cache size: 128-4096] [cache mapping: dm|fa] "
        "[cache organization: uc|sc] <path_to_trace_file>\n");
    exit(0);
  } else {
    /* argv[0] is program name, parameters start with argv[1] */

    /* Set cache size */
    cache_size = atoi(argv[1]);
    if (verify_cache_size(cache_size)) {
      exit(0);
    }


    /* Set Cache Mapping */
    if (strcmp(argv[2], "dm") == 0) {
      cache_mapping = dm;
    } else if (strcmp(argv[2], "fa") == 0) {
      cache_mapping = fa;
    } else {
      printf("Unknown cache mapping\n");
      exit(0);
    }

    /* Set Cache Organization */
    if (strcmp(argv[3], "uc") == 0) {
      cache_org = uc;
    } else if (strcmp(argv[3], "sc") == 0) {
      cache_org = sc;
    } else {
      printf("Unknown cache organization\n");
      exit(0);
    }
  }

  /** Allocate memory for cache **/
  cache_length = get_cache_length(cache_size, cache_org);
  if (cache_org == uc) {
    if (cache_init(&cache, cache_length)) {
      printf("Failed to allocate memory for unified cache\n");
      exit(0);
    }
  } else { /* split cache */
    if (cache_init(&cache_data, cache_length)) {
      printf("Failed to allocate memory for data cache\n");
      exit(0);
    }
    if (cache_init(&cache_inst, cache_length)) {
      printf("Failed to allocate memory for instruction cache\n");
      exit(0);
    }
  }

  /* Get cache bits, which will be used in placing memory transfers */
  set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);

  /* Open the file to read memory traces.
   * Either user provided with argv[4], or mem_traces.txt
   */
  FILE* ptr_file;
  if (argv[4]) {
    ptr_file = fopen(argv[4], "r");
  } else {
    ptr_file = fopen("mem_trace.txt", "r");
  }
  if (!ptr_file) {
    printf("Unable to open the trace file\n");
    exit(1);
  }

  /* Loop until whole trace file has been read */
  mem_access_t access;
  while (1) {
    access = read_transaction(ptr_file);
    // If no transactions left, break out of loop
    if (access.address == 0) break;
    // printf("%d %x\n", access.accesstype, access.address);
    cache_statistics.accesses++;
    set_access_identifiers(&access, cache_bits);

    /** Perform cache access **/
    if (cache_mapping == dm) {
      if (cache_org == uc) {
        ret = access_cache_dm(&cache, &access);
      } else { /* split cache */
        if (access.accesstype == instruction) {
          ret = access_cache_dm(&cache_inst, &access);
        } else { /* Data */
          ret = access_cache_dm(&cache_data, &access);
        }
      }
    } else { /* fully associative */
      if (cache_org == uc) {
        ret = access_cache_fa(&cache, &access);
      } else { /* split cache */
        if (access.accesstype == instruction) {
          ret = access_cache_fa(&cache_inst, &access);
        } else { /* Data */
          ret = access_cache_fa(&cache_data, &access);
        }
      }
    }

    cache_statistics.hits += ret;
  }

  if (cache_org == uc) {
    cache_deinit(&cache);
  } else {
    cache_deinit(&cache_data);
    cache_deinit(&cache_inst);
  }

  /* Print the statistics */
  // DO NOT CHANGE THE FOLLOWING LINES!
  printf("\nCache Statistics\n");
  printf("-----------------\n\n");
  printf("Accesses: %ld\n", cache_statistics.accesses);
  printf("Hits:     %ld\n", cache_statistics.hits);
  printf("Hit Rate: %.4f\n",
         (double)cache_statistics.hits / cache_statistics.accesses);
  // DO NOT CHANGE UNTIL HERE
  // You can extend the memory statistic printing if you like!
  printf("Evicts:     %ld\n", cache_statistics.evicts);
  /* Close the trace file */
  fclose(ptr_file);
}
// #endif /* RUN_UNIT_TESTS */
