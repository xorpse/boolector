#include "testhash.h"
#include "btorhash.h"
#include "testrunner.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static BtorMemMgr *mem;

void
init_hash_tests (void)
{
  mem = btor_new_mem_mgr ();
}

static void
test_new_delete_hash (void)
{
  size_t allocated     = mem->allocated;
  BtorPtrHashTable *ht = btor_new_ptr_hash_table (mem, 0, 0);
  btor_delete_ptr_hash_table (ht);
  assert (allocated == mem->allocated);
}

static void
test_hash_str2i (void)
{
  BtorPtrHashTable *ht = btor_new_ptr_hash_table (mem, 0, 0);

  btor_insert_in_ptr_hash_table (ht, "one")->data.asInt = 1;
  assert (btor_find_in_ptr_hash_table (ht, "one"));
  assert (btor_find_in_ptr_hash_table (ht, "one")->data.asInt == 1);

  btor_insert_in_ptr_hash_table (ht, "two")->data.asInt = 2;
  assert (btor_find_in_ptr_hash_table (ht, "two"));
  assert (btor_find_in_ptr_hash_table (ht, "two")->data.asInt == 2);

  btor_insert_in_ptr_hash_table (ht, "three")->data.asInt = 3;
  assert (btor_find_in_ptr_hash_table (ht, "three"));
  assert (btor_find_in_ptr_hash_table (ht, "three")->data.asInt == 3);

  btor_delete_ptr_hash_table (ht);
}

static void
test_traverse_hash_str2i (void)
{
  BtorPtrHashTable *ht;
  BtorPtrHashBucket *p;
  char buffer[20];
  FILE *file;
  int i;

  ht = btor_new_ptr_hash_table (mem, btor_hashstr, btor_cmpstr);

  for (i = 0; i < 40; i++)
  {
    sprintf (buffer, "%d", i);
    p             = btor_insert_in_ptr_hash_table (ht, buffer);
    p->data.asInt = i;
    p->key        = btor_strdup (mem, buffer);
  }

  for (i = 0; i < 40; i++)
  {
    sprintf (buffer, "%d", i);
    assert (btor_find_in_ptr_hash_table (ht, buffer));
    assert (btor_find_in_ptr_hash_table (ht, buffer)->data.asInt == i);
  }

  file = fopen ("log/traverse_hash_str2i.log", "w");
  for (p = ht->first; p; p = p->next)
  {
    fprintf (file, "%s %d\n", (const char *) p->key, p->data.asInt);
    btor_freestr (mem, p->key);
  }
  fclose (file);

  btor_delete_ptr_hash_table (ht);
}

static void
test_hash_str2str (void)
{
  BtorPtrHashTable *ht;
  BtorPtrHashBucket *p;
  char buffer[20];
  FILE *file;
  int i;

  ht = btor_new_ptr_hash_table (mem, btor_hashstr, btor_cmpstr);

  for (i = 0; i < 10; i++)
  {
    sprintf (buffer, "%d", i);
    p      = btor_insert_in_ptr_hash_table (ht, buffer);
    p->key = btor_strdup (mem, buffer);
    sprintf (buffer, "%d", 10 - i);
    p->data.asStr = btor_strdup (mem, buffer);
  }

  for (i = 0; i < 10; i++)
  {
    sprintf (buffer, "%d", i);
    p = btor_find_in_ptr_hash_table (ht, buffer);
    assert (p);
    assert (i == atoi (p->key));
    assert (10 - i == atoi (p->data.asStr));
  }

  file = fopen ("log/hash_str2str.log", "w");
  for (p = ht->first; p; p = p->next)
  {
    fprintf (file, "%s -> %s\n", (char *) p->key, p->data.asStr);
    btor_freestr (mem, p->key);
    btor_freestr (mem, p->data.asStr);
  }
  fclose (file);

  btor_delete_ptr_hash_table (ht);
}

void
run_hash_tests (int argc, char **argv)
{
  BTOR_RUN_TEST (new_delete_hash);
  BTOR_RUN_TEST (hash_str2i);
  BTOR_RUN_TEST_CHECK_LOG (traverse_hash_str2i);
  BTOR_RUN_TEST_CHECK_LOG (hash_str2str);
}

void
finish_hash_tests (void)
{
  btor_delete_mem_mgr (mem);
}
