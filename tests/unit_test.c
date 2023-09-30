/**
 * @file test.c
 * @author Richard McCrae (https://github.com/richardmccrae-nordicsemi)
 * @brief
 * @version 0.1
 * @date 2023-09-03
 *
 * @copyright Copyright (c) 2023
 *
 * Test suite for palindrome.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>
#include "../cache_sim.h"

void setUp(void) { /* Do setup here like memory allocation, etc. */ }
void tearDown(void) { /* Do teardown here like memory deallocation, etc. */ }

int ret;
uint8_t tag;
uint32_t t_block_size = 64;
uint32_t cache_length;
uint32_t cache_size;
cache_bits_t cache_bits;
cache_map_t cache_mapping;
cache_org_t cache_org;

/**
 * requirements
 *
 * 32-bit address length
 * cache block size is 64-bytes
 * cache is FIFO
 * command line args:
 *      cache size in bytes or kB (128 - 4096B)
 *      cache mapping: direct mapped or fully associative
 *      cache organization: unified or split
 *          split: data and instruction each get half of total cache size
 *
 * Must calculate:
 *      number of blocks: cache size/block size
 *      number of bits for block offset, index, and tag
 *
 * Output given test input:
 *      number of access to cache
 *      number of hits
 *      hit rate
*/

/**       set_cache_bits        **/
void test_set_cache_bits_dm_uc(void)
{
    cache_mapping = dm;
    cache_org = uc;

    cache_size = 128;
    cache_length = cache_size / t_block_size;
    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(1, cache_bits.index);
    tag = 32-6-1;
    TEST_ASSERT_EQUAL_UINT8(tag, cache_bits.tag);

    cache_size = 4096;
    cache_length = cache_size / t_block_size;
    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.index);
    tag = 32-6-6;
    TEST_ASSERT_EQUAL_UINT8(tag, cache_bits.tag);
}

void test_set_cache_bits_dm_sc(void)
{
    cache_mapping = dm;
    cache_org = sc;

    cache_size = 128;
    cache_length = cache_size / t_block_size / 2;
    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    tag = 32-6;
    TEST_ASSERT_EQUAL_UINT8(tag, cache_bits.tag);

    cache_size = 4096;
    cache_length = cache_size / t_block_size / 2;
    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(5, cache_bits.index, "Unexpected index bits");
    tag = 32-6-5;
    TEST_ASSERT_EQUAL_UINT8(tag, cache_bits.tag);
}

void test_set_cache_bits_fa_uc(void)
{
    cache_mapping = fa;
    cache_org = uc;

    cache_size = 128;
    cache_length = cache_size / t_block_size;
    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    TEST_ASSERT_EQUAL_UINT8(26, cache_bits.tag);

    cache_size = 4096;
    cache_length = cache_size / t_block_size;
    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    TEST_ASSERT_EQUAL_UINT8(26, cache_bits.tag);
}

void test_set_cache_bits_fa_sc(void)
{
    cache_mapping = fa;
    cache_org = sc;

    cache_size = 128;
    cache_length = cache_size / t_block_size / 2;
    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    TEST_ASSERT_EQUAL_UINT8(26, cache_bits.tag);

    cache_size = 4096;
    cache_length = cache_size / t_block_size / 2;
    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    TEST_ASSERT_EQUAL_UINT8(26, cache_bits.tag);
}


/**     allocate cache      **/
void test_cache_init(void)
{
    int err;
    cache_t cache_small;
    cache_t cache_large;
    uint8_t length;

    length = 1;
    err = cache_init(&cache_small, length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "return unexpected");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache_small.length, "unexpected length of cache");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_small.block->byte[0], "unexpected byte offset value");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_small.block->tag, "unexpected tag value");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_small.block->valid, "unexpected valid value");

    length = 64;
    err = cache_init(&cache_large, length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "return unexpected");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(64, cache_large.length, "unexpected length of cache");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_large.block->byte[0], "unexpected byte offset value");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_large.block->tag, "unexpected tag value");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_large.block->valid, "unexpected valid value");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_large.block[20].tag, "unexpected tag value");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_large.block[20].valid, "unexpected valid value");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_large.block[63].tag, "unexpected tag value");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache_large.block[63].valid, "unexpected valid value");
}

/**     set_access_identifiers      **/
void test_set_access_identifiers(void)
{
    mem_access_t access;

    access.accesstype = data;
    access.address = 0x8cda3fa8; // 10001100110110100011111110101000

    /* dm uc, 4096 size */
    cache_bits.offset = 6;
    cache_bits.index = 6;
    cache_bits.tag = 32 - cache_bits.offset - cache_bits.index;

    set_access_identifiers(&access, cache_bits);
    // offset: 101000, 0x28
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x28, access.offset, "Unexpected offset");
    // index: 111110, 0x3E
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x3E, access.index, "Unexpected index");
    // tag: 10001100110110100011, 0x8CDA3
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x8CDA3, access.tag, "Unexpected tag");

    /* fa uc, 4096 size */
    cache_bits.offset = 6;
    cache_bits.index = 0;
    cache_bits.tag = 32 - cache_bits.offset - cache_bits.index;

    set_access_identifiers(&access, cache_bits);
    // offset: 101000, 0x28
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x28, access.offset, "Unexpected offset");
    // index: 0
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x0, access.index, "Unexpected index");
    // tag: 0x23368FE
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x23368FE, access.tag, "Unexpected tag");
}

/**     access_cache_dm      **/
void test_access_cache_dm_128B(void)
{
    int err;
    mem_access_t access;
    cache_t cache;

    cache_size = 128;
    cache_org = uc;
    cache_mapping = dm;
    cache_bits.index = 0;
    cache_bits.offset = 0;
    cache_bits.tag = 0;
    access.accesstype = data;   /* This doesn't matter */

    cache_length = get_cache_length(cache_size, cache_org);
    err = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "cache_init() unexpected return");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    printf("Cache bits: offset: %d, idx: %d, tag: %d\n", cache_bits.offset, cache_bits.index, cache_bits.tag);

    /* First in a series of accesses, should be compulsory miss */
    access.address = 0x00000000;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");
    /* Verify offset here */

    /* Same address, expect hit */
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /* New address within first index, same tag, expect cache hit (no offset!) */
    access.address = 0x0000003f;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /* Same index, new tag, expect miss */
    access.address = 0xff00001f;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /* Same index, first tag, expect miss */
    access.address = 0x0000001f;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /* Second index, expect miss */
    access.address = 0x00000040;
    set_access_identifiers(&access, cache_bits);
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == false, "Valid bit should not be set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, 0, "Tag should be zero");
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /* Second index, new tag, expect miss */
    access.address = 0xff000040;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /* Second index, first tag, expect miss */
    access.address = 0x0000004a;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /* Second index, first tag, new offset, expect hit */
    access.address = 0x0000004a;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");
}

void test_access_cache_dm_128B_mem_trace_1(void)
{
    int err;
    mem_access_t access;
    cache_t cache;

    cache_size = 128;
    cache_org = uc;
    cache_mapping = dm;
    cache_bits.index = 0;
    cache_bits.offset = 0;
    cache_bits.tag = 0;
    access.accesstype = data;   /* This doesn't matter */

    cache_length = get_cache_length(cache_size, cache_org);
    err = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "cache_init() unexpected return");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    printf("Cache bits: offset: %d, idx: %d, tag: %d\n", cache_bits.offset, cache_bits.index, cache_bits.tag);

    /*  0x8cda3fa8
        10001100110110100011111110101000
        tag:    1000110011011010001111111 -> 119B47F
        idx:    0
        offset: 101000 -> 40

        Compulsory miss to index 0
    */
    access.address = 0x8cda3fa8;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /*  0x8158bf94
        10000001010110001011111110010100
        tag:    10000001010110001011111110 -> 20562FE
        idx:    0
        offset: 101000 -> 40

        Same index, different tag, expect miss
    */
    access.address = 0x8158bf94;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");

    /*  0x8cd94c50
        10001100110110010100110001010000
        tag:    1000110011011001010011000 -> 119B298
        idx:    1
        offset: 010000 -> 16

        Compulsory miss to index 1
    */
    access.address = 0x8cd94c50;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");

    /*  0x8cd94d64
        10001100110110010100110101100100
        tag:    1000110011011001010011010 -> 119B29A
        idx:    1

        Index 1, different tag, miss
    */
    access.address = 0x8cd94d64;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");

    /*  0x8cd94c54
        10001100110110010100110001010100
        tag:    1000110011011001010011000 -> 119B298
        idx:    1

        Index 1, different tag, miss
    */
    access.address = 0x8cd94c54;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
}

void test_access_cache_dm_4096B_mem_trace_1(void)
{
    int err;
    mem_access_t access;
    cache_t cache;

    cache_size = 4096;
    cache_org = uc;
    cache_mapping = dm;
    cache_bits.index = 0;
    cache_bits.offset = 0;
    cache_bits.tag = 0;
    access.accesstype = data;   /* This doesn't matter */

    cache_length = get_cache_length(cache_size, cache_org);
    err = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "cache_init() unexpected return");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    printf("Cache bits: offset: %d, idx: %d, tag: %d\n", cache_bits.offset, cache_bits.index, cache_bits.tag);

    /*  0x8cda3fa8
        10001100110110100011111110101000
        tag:    10001100110110100011 -> 8CDA3
        idx:    111110 -> 3e
        offset: 101000

        Compulsory miss to index
    */
    access.address = 0x8cda3fa8;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");
    TEST_ASSERT_TRUE_MESSAGE(cache.block[access.index].valid == true, "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(cache.block[access.index].tag, access.tag, "Unexpected tag");

    /*  0x8158bf94
        10000001010110001011111110010100
        tag:    10000001010110001011 -> 8158B
        idx:    111110 -> 3e

        Same index, different tag, expect miss
    */
    access.address = 0x8158bf94;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");

    /*  0x8cd94c50
        10001100110110010100110001010000
        tag:    10001100110110010100 -> 8CD94
        idx:    110001 -> 31

        Compulsory miss to new index
    */
    access.address = 0x8cd94c50;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");

    /*  0x8cd94d64
        10001100110110010100110101100100
        tag:    10001100110110010100 -> 8CD94
        idx:    110101 -> 35

        Compulsory miss to new index
    */
    access.address = 0x8cd94d64;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_dm() unexpected return");

    /*  0x8cd94c54
        10001100110110010100110001010100
        tag:    10001100110110010100 -> 8CD94
        idx:    110001 -> 31

        A hit!
    */
    access.address = 0x8cd94c54;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_dm(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, err, "access_cache_dm() unexpected return");
}

void test_access_cache_fa_128B_mem_trace_1(void)
{
    /**
     * Such a small cache has only 2 rows. Only two successive calls with the same
     * tag will be a hit
    */

    int err;
    mem_access_t access;
    cache_t cache;

    cache_size = 128;
    cache_org = uc;
    cache_mapping = fa;
    cache_bits.index = 0;
    cache_bits.offset = 0;
    cache_bits.tag = 0;
    access.accesstype = data;   /* This doesn't matter for uc */

    cache_length = get_cache_length(cache_size, cache_org);
    err = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "cache_init() unexpected return");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    printf("Cache bits: offset: %d, idx: %d, tag: %d\n", cache_bits.offset, cache_bits.index, cache_bits.tag);

    /*  0x8cda3fa8
        10001100110110100011111110101000
        tag:    10001100110110100011111110 -> 23368FE
        offset: 101000 -> 40

        1st entry, miss
    */
    access.address = 0x8cda3fa8;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");

    /*  0x8158bf94
        10000001010110001011111110010100
        tag:    10000001010110001011111110 -> 20562FE
        offset: 101000 -> 40

        Miss
    */
    access.address = 0x8158bf94;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");

    /*  0x8cd94c50
        10001100110110010100110001010000
        tag:    10001100110110010100110001 -> 2336531
        offset: 010000 -> 16

        Miss
    */
    access.address = 0x8cd94c50;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");

    /*  0x8cd94d64
        10001100110110010100110101100100
        tag:    10001100110110010100110101 -> 2336535

        Miss
    */
    access.address = 0x8cd94d64;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");

    /*  0x8cd94c54
        10001100110110010100110001010100
        tag:    10001100110110010100110001 -> 2336531

        Same tag as two rounds ago, but this one has been evicted. Miss
    */
    access.address = 0x8cd94c54;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");
}

void test_access_cache_fa_4096B_mem_trace_1(void)
{
    int err;
    mem_access_t access;
    cache_t cache;

    cache_size = 128;
    cache_org = uc;
    cache_mapping = fa;
    cache_bits.index = 0;
    cache_bits.offset = 0;
    cache_bits.tag = 0;
    access.accesstype = data;   /* This doesn't matter for uc */

    cache_length = get_cache_length(cache_size, cache_org);
    err = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "cache_init() unexpected return");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);
    printf("Cache bits: offset: %d, idx: %d, tag: %d\n", cache_bits.offset, cache_bits.index, cache_bits.tag);

    /*  0x8cda3fa8
        10001100110110100011111110101000
        tag:    10001100110110100011111110 -> 23368FE
        offset: 101000 -> 40

        1st entry, miss
    */
    access.address = 0x8cda3fa8;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");

    /*  0x8158bf94
        10000001010110001011111110010100
        tag:    10000001010110001011111110 -> 20562FE
        offset: 101000 -> 40

        Miss
    */
    access.address = 0x8158bf94;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");

    /*  0x8cd94c50
        10001100110110010100110001010000
        tag:    10001100110110010100110001 -> 2336531
        offset: 010000 -> 16

        Miss
    */
    access.address = 0x8cd94c50;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");

    /*  0x8cd94d64
        10001100110110010100110101100100
        tag:    10001100110110010100110101 -> 2336535

        Miss
    */
    access.address = 0x8cd94d64;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, err, "access_cache_fa() unexpected return");

    /*  0x8cd94c54
        10001100110110010100110001010100
        tag:    10001100110110010100110001 -> 2336531

        Hit
    */
    access.address = 0x8cd94c54;
    set_access_identifiers(&access, cache_bits);
    err = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, err, "access_cache_fa() unexpected return");
}

void test_transfer_address_to_cache(void)
{
    int ret;
    cache_t cache;
    mem_access_t access;

    cache_mapping = uc;
    cache_org = fa;
    cache_size = 1024;

    cache_length = get_cache_length(cache_size, cache_org);
    ret = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "cache_init() failed");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);

    /*  0x8cda3fa8
        10001100110110100011111110101000
        tag:    10001100110110100011111110 -> 23368FE
    */
    access.address = 0x8cda3fa8;
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache.start,
        "Start pointer incorrect");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.end,
        "End pointer incorrect");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache.is_full,
        "is_full member should not be set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.block[0].valid,
        "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(access.tag, cache.block[0].tag,
        "Unexpected tag bits");

    /*  0x8158bf94
        10000001010110001011111110010100
        tag:    10000001010110001011111110 -> 8158BF94
    */
    access.address = 0x8158bf94;
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache.start,
        "Start pointer incorrect");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, cache.end,
        "End pointer incorrect");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache.is_full,
        "is_full member should not be set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.block[1].valid,
        "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(access.tag, cache.block[1].tag,
        "Unexpected tag bits");
}

void test_transfer_address_to_cache_is_full(void)
{
    int ret;
    cache_t cache;
    mem_access_t access;

    cache_org = uc;
    cache_mapping = fa;
    cache_size = 128;

    cache_length = get_cache_length(cache_size, cache_org);
    ret = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "cache_init() failed");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);

    access.address = 0x00000000;
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    access.address = 0x8158bf94; /* tag: 0x20562FE */
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    /* This should overwrite first entry */
    access.address = 0x8cd94c50; /* tag: 0x2336531 */
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.start,
        "Start pointer incorrect");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.end,
        "End pointer incorrect");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.is_full,
        "is_full member should not be set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.block[0].valid,
        "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.block[1].valid,
        "Valid bit not set");
    TEST_ASSERT_EQUAL_HEX_MESSAGE(0x2336531, cache.block[0].tag,
        "Unexpected tag bits");
    TEST_ASSERT_EQUAL_HEX_MESSAGE(0x20562FE, cache.block[1].tag,
        "Unexpected tag bits");

    /* Should overwrite 2nd entry */
    access.address = 0x00000000;
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache.start,
        "Start pointer incorrect");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, cache.end,
        "End pointer incorrect");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.is_full,
        "is_full member should not be set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.block[0].valid,
        "Valid bit not set");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cache.block[1].valid,
        "Valid bit not set");
    TEST_ASSERT_EQUAL_HEX_MESSAGE(0x2336531, cache.block[0].tag,
        "Unexpected tag bits");
    TEST_ASSERT_EQUAL_HEX_MESSAGE(0x0, cache.block[1].tag,
        "Unexpected tag bits");
}

void test_is_address_in_fa_cache(void)
{
    int ret;
    cache_t cache;
    mem_access_t access;

    cache_org = uc;
    cache_mapping = fa;
    cache_size = 256;

    cache_length = get_cache_length(cache_size, cache_org);
    ret = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "cache_init() failed");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);

    /* Cache is empty */
    ret = is_address_in_fa_cache(&cache, &access);
    TEST_ASSERT_EQUAL(0, ret);

    access.address = 0x00000000;
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    access.address = 0x8158bf94; /* tag: 0x20562FE */
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    /* Same as above, should return 1 */
    ret = is_address_in_fa_cache(&cache, &access);
    TEST_ASSERT_EQUAL(1, ret);

    access.address = 0xff000000;
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    access.address = 0xcc000000;
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    access.address = 0xaa000000;
    set_access_identifiers(&access, cache_bits);
    transfer_address_to_cache(&cache, &access);

    /* Not in cache */
    access.address = 0x00ff0000;
    set_access_identifiers(&access, cache_bits);
    ret = is_address_in_fa_cache(&cache, &access);
    TEST_ASSERT_EQUAL(0, ret);

    /* Is in cache */
    access.address = 0xaa000000;
    set_access_identifiers(&access, cache_bits);
    ret = is_address_in_fa_cache(&cache, &access);
    TEST_ASSERT_EQUAL(1, ret);
}

void test_access_cache_fa(void)
{
    int ret;
    cache_t cache;
    mem_access_t access;

    cache_org = uc;
    cache_mapping = fa;
    cache_size = 256;

    cache_length = get_cache_length(cache_size, cache_org);
    ret = cache_init(&cache, cache_length);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "cache_init() failed");

    set_cache_bits(&cache_bits, cache_length, cache_mapping, cache_org);

    /* Item 1/4 */
    access.address = 0x00000000;
    set_access_identifiers(&access, cache_bits);
    ret = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL(0, ret);

    /* Hit */
    access.address = 0x00000001;
    set_access_identifiers(&access, cache_bits);
    ret = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL(1, ret);

    /* Item 2/4 */
    access.address = 0x00000100;
    set_access_identifiers(&access, cache_bits);
    ret = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL(0, ret);

    /* Item 3/4 */
    access.address = 0x00010000;
    set_access_identifiers(&access, cache_bits);
    ret = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL(0, ret);

    /* Hit */
    access.address = 0x00000000;
    set_access_identifiers(&access, cache_bits);
    ret = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL(1, ret);

    /* Item 4/4 */
    access.address = 0x01000000;
    set_access_identifiers(&access, cache_bits);
    ret = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL(0, ret);

    /* Item 5/4 -> overwrite item 1: 0x0000000 */
    access.address = 0xff000000;
    set_access_identifiers(&access, cache_bits);
    ret = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL(0, ret);

    /* Rewrite 0x00000000 , expect miss */
    access.address = 0x00000000;
    set_access_identifiers(&access, cache_bits);
    ret = access_cache_fa(&cache, &access);
    TEST_ASSERT_EQUAL(0, ret);
}

/**     Test main      **/
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_set_cache_bits_dm_uc);
    RUN_TEST(test_set_cache_bits_dm_sc);
    RUN_TEST(test_set_cache_bits_fa_uc);
    RUN_TEST(test_set_cache_bits_fa_sc);

    RUN_TEST(test_cache_init);

    RUN_TEST(test_set_access_identifiers);

    RUN_TEST(test_access_cache_dm_128B);
    RUN_TEST(test_access_cache_dm_128B_mem_trace_1);
    RUN_TEST(test_access_cache_dm_4096B_mem_trace_1);

    RUN_TEST(test_transfer_address_to_cache);
    RUN_TEST(test_transfer_address_to_cache_is_full);
    RUN_TEST(test_is_address_in_fa_cache);
    RUN_TEST(test_access_cache_fa);

    return UNITY_END();
}
