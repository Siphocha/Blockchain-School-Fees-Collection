#include "persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Need our own persistence saves
void save_blockchain(Blockchain *chain, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Failed to save blockchain");
        return;
    }
    Block *current = chain->head;
    while (current) {
        fwrite(current, sizeof(Block), 1, fp);
        current = current->next;
    }
    fclose(fp);
}

//Load up saves to continue w the persistence 
void load_blockchain(Blockchain *chain, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return; 
    
    Block block;
    while (fread(&block, sizeof(Block), 1, fp) == 1) {
        Block *new_block = malloc(sizeof(Block));
        memcpy(new_block, &block, sizeof(Block));
        new_block->next = NULL;
        if (!chain->head) {
            chain->head = chain->tail = new_block;
        } else {
            chain->tail->next = new_block;
            chain->tail = new_block;
        }
        chain->length++;
    }
    fclose(fp);
}

void save_mempool(Mempool *mempool, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Failed to save mempool");
        return;
    }
    // Save count first, then transactions
    fwrite(&mempool->count, sizeof(int), 1, fp);
    fwrite(mempool->items, sizeof(Transaction), mempool->count, fp);
    fclose(fp);
}

void load_mempool(Mempool *mempool, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return;
    
    int count;
    fread(&count, sizeof(int), 1, fp);
    if (count > mempool->capacity) count = mempool->capacity;
    fread(mempool->items, sizeof(Transaction), count, fp);
    mempool->count = count;
    fclose(fp);
}