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

/**     Tests          **/
void test_set_cache_bits_dm_uc(void)
{
    cache_bits_t cache_bits;
    uint32_t cache_size;
    cache_map_t cache_mapping = dm;
    cache_org_t cache_org = uc;
    uint8_t tag;

    cache_size = 128;
    set_cache_bits(&cache_bits, cache_size, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(1, cache_bits.index);
    tag = 32-6-1;
    TEST_ASSERT_EQUAL_UINT8(tag, cache_bits.tag);

    cache_size = 4096;
    set_cache_bits(&cache_bits, cache_size, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.index);
    tag = 32-6-6;
    TEST_ASSERT_EQUAL_UINT8(tag, cache_bits.tag);
}

void test_set_cache_bits_dm_sc(void)
{
    cache_bits_t cache_bits;
    uint32_t cache_size;
    cache_map_t cache_mapping = dm;
    cache_org_t cache_org = sc;
    uint8_t tag;

    cache_size = 128;
    set_cache_bits(&cache_bits, cache_size, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    tag = 32-6;
    TEST_ASSERT_EQUAL_UINT8(tag, cache_bits.tag);

    cache_size = 4096;
    set_cache_bits(&cache_bits, cache_size, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(3, cache_bits.index);
    tag = 32-6-3;
    TEST_ASSERT_EQUAL_UINT8(tag, cache_bits.tag);
}

void test_set_cache_bits_fa_uc(void)
{
    cache_bits_t cache_bits;
    uint32_t cache_size;
    cache_map_t cache_mapping = fa;
    cache_org_t cache_org = uc;

    cache_size = 128;
    set_cache_bits(&cache_bits, cache_size, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    TEST_ASSERT_EQUAL_UINT8(26, cache_bits.tag);

    cache_size = 4096;
    set_cache_bits(&cache_bits, cache_size, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    TEST_ASSERT_EQUAL_UINT8(26, cache_bits.tag);
}

void test_set_cache_bits_fa_sc(void)
{
    cache_bits_t cache_bits;
    uint32_t cache_size;
    cache_map_t cache_mapping = fa;
    cache_org_t cache_org = sc;

    cache_size = 128;
    set_cache_bits(&cache_bits, cache_size, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    TEST_ASSERT_EQUAL_UINT8(26, cache_bits.tag);

    cache_size = 4096;
    set_cache_bits(&cache_bits, cache_size, cache_mapping, cache_org);
    TEST_ASSERT_EQUAL_UINT8(6, cache_bits.offset);
    TEST_ASSERT_EQUAL_UINT8(0, cache_bits.index);
    TEST_ASSERT_EQUAL_UINT8(26, cache_bits.tag);
}

/**     Test main      **/
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_set_cache_bits_dm_uc);
    RUN_TEST(test_set_cache_bits_dm_sc);
    RUN_TEST(test_set_cache_bits_fa_uc);
    RUN_TEST(test_set_cache_bits_fa_sc);
    
    return UNITY_END();
}
