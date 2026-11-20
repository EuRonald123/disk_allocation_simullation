#ifndef DISK_H
#define DISK_H

#include <stdbool.h>

#define MAX_DISK_SIZE 64
#define DEFAULT_DISK_SIZE 10
#define MAX_FILES 26
#define INVALID_BLOCK -1

// Enums de metodos de alocacao dos arquivos
typedef enum {
    ALLOC_CONTIGUOUS,
    ALLOC_LINKED,
    ALLOC_INDEXED
} AllocationMethod;

// Enums do tipo do bloco
typedef enum {
    BLOCK_FREE,
    BLOCK_DATA,
    BLOCK_INDEX
} BlockType;

typedef struct {
    BlockType type;
    char file_id;
    int next_block;
    int index_table[MAX_DISK_SIZE];
    int index_count;
} Block;

typedef struct {
    char id;
    int size;
    int start_block;
    bool active;
} FileEntry;

typedef struct {
    AllocationMethod allocation_method;
    int total_blocks;
    Block blocks[MAX_DISK_SIZE];
    FileEntry file_entries[MAX_FILES];
} Disk;

/* Inicializa o disco com metodo e quantidade de blocos */
void disk_init(Disk *disk, AllocationMethod method, int total_blocks);

/* Reseta um unico bloco do disco para o estado de livre */
void disk_reset_block(Block *block);

/* Busca um arquivo ativo pelo id */
int disk_find_file(const Disk *disk, char file_id);

/* Busca um slot vazio no diretorio */
int disk_find_free_file_slot(const Disk *disk);

/* Conta a quantidade de blocos livres no disco */
int disk_count_free_blocks(const Disk *disk);

#endif // DISK_H
