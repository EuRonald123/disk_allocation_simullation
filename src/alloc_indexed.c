#include "allocator.h"

bool alloc_indexed(Disk *disk, char file_id, int size) {
    if (size <= 0 || size >= DISK_SIZE) {
        return false;
    }

    // Verifica se o arquivo ja existe ou se o diretorio esta cheio
    if (disk_find_file(disk, file_id) != -1) {
        return false;
    }

    int slot = disk_find_free_file_slot(disk);
    if (slot == -1) {
        return false;
    }

    // Precisa de 'size' blocos de dados + 1 bloco exclusivo para o indice
    int total_needed = size + 1;
    if (disk_count_free_blocks(disk) < total_needed) {
        return false;
    }

    int index_block = INVALID_BLOCK;
    int data_allocated = 0;

    // 1. Encontra o primeiro bloco livre para ser o bloco de indice
    for (int i = 0; i < DISK_SIZE; i++) {
        if (disk->blocks[i].type == BLOCK_FREE) {
            index_block = i;
            disk->blocks[i].type = BLOCK_INDEX;
            disk->blocks[i].file_id = file_id;
            disk->blocks[i].index_count = 0;
            break;
        }
    }

    // 2. Encontra 'size' blocos livres para os dados e registra no bloco de indice
    for (int i = 0; i < DISK_SIZE && data_allocated < size; i++) {
        if (disk->blocks[i].type == BLOCK_FREE) {
            disk->blocks[i].type = BLOCK_DATA;
            disk->blocks[i].file_id = file_id;

            disk->blocks[index_block].index_table[data_allocated] = i;
            disk->blocks[index_block].index_count++;
            data_allocated++;
        }
    }

    // Registra o arquivo no diretorio (start_block aponta para o bloco de indice)
    disk->file_entries[slot].id = file_id;
    disk->file_entries[slot].size = size;
    disk->file_entries[slot].start_block = index_block;
    disk->file_entries[slot].active = true;

    return true;
}

bool remove_indexed(Disk *disk, char file_id) {
    int slot = disk_find_file(disk, file_id);
    if (slot == -1) {
        return false;
    }

    int index_block = disk->file_entries[slot].start_block;
    int count = disk->blocks[index_block].index_count;

    // Libera todos os blocos de dados apontados pela tabela de indice
    for (int i = 0; i < count; i++) {
        int data_blk = disk->blocks[index_block].index_table[i];
        if (data_blk >= 0 && data_blk < DISK_SIZE) {
            disk_reset_block(&disk->blocks[data_blk]);
        }
    }

    // Libera o proprio bloco de indice
    disk_reset_block(&disk->blocks[index_block]);

    // Remove a entrada do diretorio
    disk->file_entries[slot].id = '\0';
    disk->file_entries[slot].size = 0;
    disk->file_entries[slot].start_block = INVALID_BLOCK;
    disk->file_entries[slot].active = false;

    return true;
}
