#include "transaction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void transaction_generate_id(char *id) {
    sprintf(id, "%ld%03d", time(NULL), rand() % 1000);
}

void transaction_create(Transaction *tx, const char *type, const char *student_id,
                        const char *invoice_id, double amount, const char *reference) {
    memset(tx, 0, sizeof(Transaction));
    transaction_generate_id(tx->transaction_id);
    strncpy(tx->type, type, sizeof(tx->type)-1);
    strncpy(tx->student_id, student_id, sizeof(tx->student_id)-1);
    if (invoice_id)
        strncpy(tx->invoice_id, invoice_id, sizeof(tx->invoice_id)-1);
    tx->amount = amount;
    if (reference)
        strncpy(tx->reference, reference, sizeof(tx->reference)-1);
    tx->timestamp = time(NULL);
}