#include "utils.h"
#include "blockchain.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>   //added this for strtod

bool validate_student_id(const char *id) {
    if (!id || strlen(id) == 0 || strlen(id) >= 20) return false;
    for (int i = 0; id[i]; i++) {
        if (!isalnum(id[i])) return false;
    }
    return true;
}

bool validate_invoice_id(const char *id) {
    //Same as student ID, just because why not
    return validate_student_id(id);
}

bool validate_amount(double amt) {
    //arbitrary max
    return amt >= 0.0 && amt < 1e6; 
}

bool validate_reference(const char *ref) {
    if (!ref || strlen(ref) >= 50) return false;
    for (int i = 0; ref[i]; i++) {
        if (!isprint(ref[i])) return false;
    }
    return true;
}

bool is_number(const char *str) {
    char *endptr;
    strtod(str, &endptr);
    return *endptr == '\0' && endptr != str;
}

double get_invoice_balance(Blockchain *chain, const char *invoice_id) {
    double total_invoiced = 0.0;
    double total_paid = 0.0;
    
    Block *current = chain->head;
    while (current) {
        for (int i = 0; i < current->tx_count; i++) {
            Transaction *tx = &current->transactions[i];
            if (strcmp(tx->invoice_id, invoice_id) != 0) continue;
            
            if (strcmp(tx->type, "invoice_create") == 0) {
                total_invoiced += tx->amount;
            } else if (strcmp(tx->type, "payment_record") == 0) {
                total_paid += tx->amount;
            } else if (strcmp(tx->type, "payment_confirm") == 0) {
            } else if (strcmp(tx->type, "invoice_settlement") == 0) {
            }
        }
        current = current->next;
    }
    return total_invoiced - total_paid;
}