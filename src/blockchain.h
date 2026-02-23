#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "transaction.h"
#include <stdint.h>

#define HASH_LEN 65
#define MAX_TRANSACTIONS_PER_BLOCK 10

typedef struct Block {
    int block_id;
    int64_t timestamp;
    Transaction transactions[MAX_TRANSACTIONS_PER_BLOCK];
    int tx_count;
    char prev_hash[HASH_LEN];
    char hash[HASH_LEN];
    int nonce;
    struct Block *next;
} Block;

typedef struct {
    Block *head;
    Block *tail;
    int length;
} Blockchain;

Blockchain* blockchain_create(void);
void blockchain_free(Blockchain *chain);
Block* blockchain_add_block(Blockchain *chain, Transaction *txs, int tx_count, int difficulty);
int blockchain_verify(Blockchain *chain, int difficulty);
void blockchain_print(Blockchain *chain);
Block* blockchain_get_latest_block(Blockchain *chain);
void calculate_block_hash(const Block *block, char output[HASH_LEN]);
void mine_block(Block *block, int difficulty);

#endif