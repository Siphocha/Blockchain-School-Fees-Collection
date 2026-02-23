#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdint.h>

typedef struct {
    char transaction_id[36];   
    char type[20];             //Has to be this long for taking in all those invoice characters
    char student_id[20];
    char invoice_id[20];
    double amount;
    char reference[50];
    int64_t timestamp;
} Transaction;

void transaction_create(Transaction *tx, const char *type, const char *student_id,
                        const char *invoice_id, double amount, const char *reference);
void transaction_generate_id(char *id);

#endif