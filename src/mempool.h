#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "transaction.h"

typedef struct {
    Transaction *items;
    int count;
    int capacity;
} Mempool;

Mempool* mempool_create(int capacity);
void mempool_free(Mempool *mp);
int mempool_add(Mempool *mp, Transaction *tx);
int mempool_remove(Mempool *mp, const char *tx_id);
void mempool_clear(Mempool *mp);
Transaction* mempool_get_all(Mempool *mp, int *out_count);
void mempool_print(const Mempool *mp);

#endif