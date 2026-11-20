#include "allocator.h"

bool alloc_indexed(Disk *disk, char file_id, int size) {
    if (size <= 0 || size >= disk->total_blocks) {
        return false;
    }

    if (disk_find_file(disk, file_id) != -1) {
        return false;
    }

    int slot = disk_find_free_file_slot(disk);
    if (slot == -1) {
        return false;
    }

    // Precisa de 'size' blocos de dados + 1 bloco para o indice
    int total_needed = size + 1;
    if (disk_count_free_blocks(disk) < total_needed) {
        return false;
    }

    int index_block = INVALID_BLOCK;
    int data_allocated = 0;

    // 1. Primeiro bloco livre vira o bloco de indice
    for (int i = 0; i < disk->total_blocks; i++) {
        if (disk->blocks[i].type == BLOCK_FREE) {
            index_block = i;
            disk->blocks[i].type = BLOCK_INDEX;
            disk->blocks[i].file_id = file_id;
            disk->blocks[i].index_count = 0;
            break;
        }
    }

    // 2. Demais blocos livres guardam os dados e entram na tabela de indice
    for (int i = 0; i < disk->total_blocks && data_allocated < size; i++) {
        if (disk->blocks[i].type == BLOCK_FREE) {
            disk->blocks[i].type = BLOCK_DATA;
            disk->blocks[i].file_id = file_id;

            disk->blocks[index_block].index_table[data_allocated] = i;
            disk->blocks[index_block].index_count++;
            data_allocated++;
        }
    }

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

    for (int i = 0; i < count; i++) {
        int data_blk = disk->blocks[index_block].index_table[i];
        if (data_blk >= 0 && data_blk < disk->total_blocks) {
            disk_reset_block(&disk->blocks[data_blk]);
        }
    }

    disk_reset_block(&disk->blocks[index_block]);

    disk->file_entries[slot].id = '\0';
    disk->file_entries[slot].size = 0;
    disk->file_entries[slot].start_block = INVALID_BLOCK;
    disk->file_entries[slot].active = false;

    return true;
}
