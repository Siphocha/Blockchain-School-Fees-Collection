#include "cli.h"
#include "transaction.h"
#include "utils.h"
#include "persistence.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static void print_help(void) {
    printf("Commands:\n");
    printf("  invoice create <student_id> <amount> <reference>          - create new invoice\n");
    printf("  invoice status <invoice_id>                               - show invoice balance\n");
    printf("  payment record <student_id> <invoice_id> <amount> <ref>   - record a payment\n");
    printf("  payment confirm <transaction_id>                          - confirm a payment\n");
    printf("  mine                                                      - mine pending transactions\n");
    printf("  chain view                                                - display blockchain\n");
    printf("  chain verify                                              - verify chain integrity\n");
    printf("  exit                                                      - save and quit\n");
}

static void cmd_invoice_create(Mempool *mp, char *student, char *amt_str, char *ref) {
    if (!validate_student_id(student)) {
        printf("Invalid student ID.\n"); return;
    }
    if (!is_number(amt_str)) {
        printf("Invalid amount.\n"); return;
    }
    double amount = atof(amt_str);
    if (!validate_amount(amount)) {
        printf("Amount out of range.\n"); return;
    }
    if (!validate_reference(ref)) {
        printf("Invalid reference.\n"); return;
    }
    
    //Have to generate that unique invoice ID
    char invoice_id[20];
    sprintf(invoice_id, "INV%ld", (long)time(NULL));
    
    Transaction tx;
    transaction_create(&tx, "invoice_create", student, invoice_id, amount, ref);
    if (mempool_add(mp, &tx)) {
        printf("Invoice created (pending). Transaction ID: %s, Invoice ID: %s\n", tx.transaction_id, invoice_id);
    } else {
        printf("Mempool full, cannot add.\n");
    }
}

static void cmd_payment_record(Mempool *mp, char *student, char *invoice, char *amt_str, char *ref) {
    if (!validate_student_id(student) || !validate_invoice_id(invoice)) {
        printf("Invalid ID.\n"); return;
    }
    if (!is_number(amt_str)) {
        printf("Invalid amount.\n"); return;
    }
    double amount = atof(amt_str);
    if (!validate_amount(amount)) {
        printf("Amount out of range.\n"); return;
    }
    if (!validate_reference(ref)) {
        printf("Invalid reference.\n"); return;
    }
    
    Transaction tx;
    transaction_create(&tx, "payment_record", student, invoice, amount, ref);
    if (mempool_add(mp, &tx)) {
        printf("Payment recorded (pending). Transaction ID: %s\n", tx.transaction_id);
    } else {
        printf("Mempool full.\n");
    }
}

static void cmd_payment_confirm(Mempool *mp, char *tx_id) {
    Transaction tx;
    transaction_create(&tx, "payment_confirm", "", tx_id, 0.0, "confirmed");
    if (mempool_add(mp, &tx)) {
        printf("Confirmation added to mempool.\n");
    } else {
        printf("Mempool full.\n");
    }
}

static void cmd_invoice_status(Blockchain *chain, char *invoice_id) {
    if (!validate_invoice_id(invoice_id)) {
        printf("Invalid invoice ID.\n"); return;
    }
    double balance = get_invoice_balance(chain, invoice_id);
    printf("Invoice %s outstanding balance: %.2f\n", invoice_id, balance);
    if (balance <= 0.0001) {
        printf("Invoice is fully paid.\n");
    } else {
        printf("Outstanding amount: %.2f\n", balance);
    }
}

static void cmd_mine(Blockchain *chain, Mempool *mp, int difficulty, const char *bc_file, const char *mp_file) {
    if (mp->count == 0) {
        printf("No pending transactions to mine.\n");
        return;
    }
    
    //Taking up max transaction from mempool
    int tx_count = mp->count;
    if (tx_count > MAX_TRANSACTIONS_PER_BLOCK)
        tx_count = MAX_TRANSACTIONS_PER_BLOCK;
    
    Transaction txs[MAX_TRANSACTIONS_PER_BLOCK];
    for (int i = 0; i < tx_count; i++) {
        txs[i] = mp->items[i];
    }
    
    printf("Mining block with %d transactions...\n", tx_count);
    Block *new_block = blockchain_add_block(chain, txs, tx_count, difficulty);
    printf("Block mined: %s (nonce %d)\n", new_block->hash, new_block->nonce);
    
    //Once mined remove them from mempool
    for (int i = 0; i < tx_count; i++) {
        mempool_remove(mp, txs[i].transaction_id);
    }
    
    //These changes are persistant, for the sake of history
    save_blockchain(chain, bc_file);
    save_mempool(mp, mp_file);
}

static void cmd_chain_view(Blockchain *chain) {
    blockchain_print(chain);
}

static void cmd_chain_verify(Blockchain *chain, int difficulty) {
    if (blockchain_verify(chain, difficulty)) {
        printf("Blockchain is valid.\n");
    } else {
        printf("Blockchain verification failed!\n");
    }
}

void cli_loop(Blockchain *chain, Mempool *mempool, int difficulty,
              const char *bc_file, const char *mp_file) {
    char input[256];
    while (1) {
        printf("\n> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        
        //Remove that trailing new line
        input[strcspn(input, "\n")] = '\0';
        
        char *cmd = strtok(input, " ");
        if (!cmd) continue;
        
        if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
            print_help();
        }
        else if (strcmp(cmd, "invoice") == 0) {
            char *sub = strtok(NULL, " ");
            if (!sub) {
                printf("Usage: invoice <create|status> ...\n");
            }
            else if (strcmp(sub, "create") == 0) {
                char *student = strtok(NULL, " ");
                char *amount = strtok(NULL, " ");
                char *ref = strtok(NULL, "");
                if (student && amount && ref) {
                    cmd_invoice_create(mempool, student, amount, ref);
                } else {
                    printf("Usage: invoice create <student_id> <amount> <reference>\n");
                }
            }
            else if (strcmp(sub, "status") == 0) {
                char *invoice = strtok(NULL, " ");
                if (invoice) {
                    cmd_invoice_status(chain, invoice);
                } else {
                    printf("Usage: invoice status <invoice_id>\n");
                }
            }
            else {
                printf("Unknown invoice subcommand: %s\n", sub);
            }
        }
        else if (strcmp(cmd, "payment") == 0) {
            char *sub = strtok(NULL, " ");
            if (sub && strcmp(sub, "record") == 0) {
                char *student = strtok(NULL, " ");
                char *invoice = strtok(NULL, " ");
                char *amount = strtok(NULL, " ");
                char *ref = strtok(NULL, "");
                if (student && invoice && amount && ref) {
                    cmd_payment_record(mempool, student, invoice, amount, ref);
                } else {
                    printf("Usage: payment record <student_id> <invoice_id> <amount> <reference>\n");
                }
            } else if (sub && strcmp(sub, "confirm") == 0) {
                char *tx_id = strtok(NULL, " ");
                if (tx_id) {
                    cmd_payment_confirm(mempool, tx_id);
                } else {
                    printf("Usage: payment confirm <transaction_id>\n");
                }
            } else {
                printf("Unknown payment subcommand.\n");
            }
        }
        else if (strcmp(cmd, "mine") == 0) {
            cmd_mine(chain, mempool, difficulty, bc_file, mp_file);
        }
        else if (strcmp(cmd, "chain") == 0) {
            char *sub = strtok(NULL, " ");
            if (sub && strcmp(sub, "view") == 0) {
                cmd_chain_view(chain);
            } else if (sub && strcmp(sub, "verify") == 0) {
                cmd_chain_verify(chain, difficulty);
            } else {
                printf("Unknown chain subcommand.\n");
            }
        }
        else if (strcmp(cmd, "exit") == 0) {
            save_blockchain(chain, bc_file);
            save_mempool(mempool, mp_file);
            printf("Goodbye!\n");
            break;
        }
        else {
            printf("Unknown command. Type 'help'.\n");
        }
    }
}