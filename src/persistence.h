#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "blockchain.h"
#include "mempool.h"

void save_blockchain(Blockchain *chain, const char *filename);
void load_blockchain(Blockchain *chain, const char *filename);
void save_mempool(Mempool *mempool, const char *filename);
void load_mempool(Mempool *mempool, const char *filename);

#endif