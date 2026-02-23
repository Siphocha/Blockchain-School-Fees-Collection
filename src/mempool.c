#include "mempool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Mempool basics. Creating that Mempool.
Mempool* mempool_create(int capacity) {
    Mempool *mp = malloc(sizeof(Mempool));
    mp->items = malloc(sizeof(Transaction) * capacity);
    mp->count = 0;
    mp->capacity = capacity;
    return mp;
}

void mempool_free(Mempool *mp) {
    free(mp->items);
    free(mp);
}

//Adding the transactions of the payment to mempool
int mempool_add(Mempool *mp, Transaction *tx) {
    if (mp->count >= mp->capacity) return 0;
    mp->items[mp->count++] = *tx;
    return 1;
}

//once done they gotta go
int mempool_remove(Mempool *mp, const char *tx_id) {
    for (int i = 0; i < mp->count; i++) {
        if (strcmp(mp->items[i].transaction_id, tx_id) == 0) {
            // Shift remaining
            for (int j = i; j < mp->count-1; j++)
                mp->items[j] = mp->items[j+1];
            mp->count--;
            return 1;
        }
    }
    return 0;
}

void mempool_clear(Mempool *mp) {
    mp->count = 0;
}

Transaction* mempool_get_all(Mempool *mp, int *out_count) {
    *out_count = mp->count;
    return mp->items;
}

//Display that w accessibility
void mempool_print(const Mempool *mp) {
    printf("Mempool (%d transactions):\n", mp->count);
    for (int i = 0; i < mp->count; i++) {
        Transaction *tx = &mp->items[i];
        printf("  %s [%s] Student: %s Invoice: %s Amount: %.2f\n",
               tx->transaction_id, tx->type, tx->student_id, tx->invoice_id, tx->amount);
    }
}