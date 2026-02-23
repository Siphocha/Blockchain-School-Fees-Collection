#include "blockchain.h"
#include "sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Lets carefully define this now
Blockchain* blockchain_create(void) {
    Blockchain *chain = malloc(sizeof(Blockchain));
    chain->head = chain->tail = NULL;
    chain->length = 0;
    return chain;
}

void blockchain_free(Blockchain *chain) {
    Block *current = chain->head;
    while (current) {
        Block *next = current->next;
        free(current);
        current = next;
    }
    free(chain);
}

//Yup need that hash first.
void calculate_block_hash(const Block *block, char output[HASH_LEN]) {
    char buffer[4096] = {0};
    // Serialize block header + transactions + nonce
    sprintf(buffer, "%d%ld%s", block->block_id, block->timestamp, block->prev_hash);
    for (int i = 0; i < block->tx_count; i++) {
        Transaction *tx = (Transaction*)&block->transactions[i];
        sprintf(buffer + strlen(buffer), "%s%s%s%f%s%ld",
                tx->type, tx->student_id, tx->invoice_id,
                tx->amount, tx->reference, tx->timestamp);
    }
    sprintf(buffer + strlen(buffer), "%d", block->nonce);
    sha256(buffer, output);
}

void mine_block(Block *block, int difficulty) {
    char target[HASH_LEN] = {0};
    memset(target, '0', difficulty);
    target[difficulty] = '\0';
    
    block->nonce = 0;
    do {
        block->nonce++;
        calculate_block_hash(block, block->hash);
    } while (strncmp(block->hash, target, difficulty) != 0);
}


//Adding sequential blocks needs to be defined iteravely
Block* blockchain_add_block(Blockchain *chain, Transaction *txs, int tx_count, int difficulty) {
    Block *new_block = malloc(sizeof(Block));
    memset(new_block, 0, sizeof(Block));
    
    new_block->block_id = chain->length + 1;
    new_block->timestamp = time(NULL);
    new_block->tx_count = tx_count;
    memcpy(new_block->transactions, txs, sizeof(Transaction) * tx_count);
    
    if (chain->tail) {
        strcpy(new_block->prev_hash, chain->tail->hash);
    } else {
        memset(new_block->prev_hash, '0', HASH_LEN-1);
        new_block->prev_hash[HASH_LEN-1] = '\0';
    }
    
    mine_block(new_block, difficulty);
    
    new_block->next = NULL;
    if (!chain->head) {
        chain->head = chain->tail = new_block;
    } else {
        chain->tail->next = new_block;
        chain->tail = new_block;
    }
    chain->length++;
    
    return new_block;
}

int blockchain_verify(Blockchain *chain, int difficulty) {
    if (!chain->head) return 1; 
    
    Block *current = chain->head;
    Block *prev = NULL;
    char computed_hash[HASH_LEN];
    int block_num = 1;
    char target[HASH_LEN];
    memset(target, '0', difficulty);
    target[difficulty] = '\0';
    
    while (current) {
        calculate_block_hash(current, computed_hash);
        if (strcmp(computed_hash, current->hash) != 0) {
            printf("Block %d hash mismatch!\n", block_num);
            return 0;
        }
        if (strncmp(current->hash, target, difficulty) != 0) {
            printf("Block %d PoW invalid!\n", block_num);
            return 0;
        }
        if (prev) {
            if (strcmp(current->prev_hash, prev->hash) != 0) {
                printf("Block %d prev_hash mismatch!\n", block_num);
                return 0;
            }
        }
        //Neeed to be careful with Genesis Block implementation here
        
        prev = current;
        current = current->next;
        block_num++;
    }
    return 1;
}

void blockchain_print(Blockchain *chain) {
    Block *current = chain->head;
    int i = 1;
    while (current) {
        printf("\n=== Block #%d (ID: %d) ===\n", i, current->block_id);
        printf("Timestamp : %ld\n", current->timestamp);
        printf("Prev Hash : %s\n", current->prev_hash);
        printf("Hash      : %s\n", current->hash);
        printf("Nonce     : %d\n", current->nonce);
        printf("Transactions (%d):\n", current->tx_count);
        for (int j = 0; j < current->tx_count; j++) {
            Transaction *tx = &current->transactions[j];
            printf("  %d. [%s] Student: %s Invoice: %s Amount: %.2f Ref: %s\n",
                   j+1, tx->type, tx->student_id, tx->invoice_id, tx->amount, tx->reference);
        }
        current = current->next;
        i++;
    }
}

Block* blockchain_get_latest_block(Blockchain *chain) {
    return chain->tail;
}