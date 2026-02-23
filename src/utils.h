#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "blockchain.h"  

bool validate_student_id(const char *id);
bool validate_invoice_id(const char *id);
bool validate_amount(double amt);
bool validate_reference(const char *ref);
bool is_number(const char *str);
double get_invoice_balance(Blockchain *chain, const char *invoice_id);

#endif