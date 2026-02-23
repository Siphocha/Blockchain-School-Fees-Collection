#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "blockchain.h"
#include "mempool.h"
#include "persistence.h"
#include "cli.h"

#define DIFFICULTY 3  
#define BLOCKCHAIN_FILE "data/blockchain.dat"
#define MEMPOOL_FILE "data/mempool.dat"

int main() {
    srand(time(NULL));
    
    //Data directory creation
    system("mkdir -p data");
    
    Blockchain *chain = blockchain_create();
    Mempool *mempool = mempool_create(100);
    
    //Load it all up
    load_blockchain(chain, BLOCKCHAIN_FILE);
    load_mempool(mempool, MEMPOOL_FILE);
    
    printf("=== ALU Fee Blockchain System ===\n");
    printf("Difficulty: %d\n", DIFFICULTY);
    printf("Type 'help' for commands.\n");
    
    cli_loop(chain, mempool, DIFFICULTY, BLOCKCHAIN_FILE, MEMPOOL_FILE);
    
    blockchain_free(chain);
    mempool_free(mempool);
    return 0;
}