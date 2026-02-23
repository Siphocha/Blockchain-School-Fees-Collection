#ifndef CLI_H
#define CLI_H

#include "blockchain.h"
#include "mempool.h"

void cli_loop(Blockchain *chain, Mempool *mempool, int difficulty,
              const char *bc_file, const char *mp_file);

#endif