#include "allocator.h"

bool alloc_linked(Disk *disk, char file_id, int size) {
    if (size <= 0 || size > DISK_SIZE) {
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

    // Verifica se ha blocos livres suficientes no total (nao precisam ser contiguos)
    if (disk_count_free_blocks(disk) < size) {
        return false;
    }

    int start_block = INVALID_BLOCK;
    int prev_block = INVALID_BLOCK;
    int allocated = 0;

    // Percorre o disco pegando os primeiros blocos livres e encadeando-os
    for (int i = 0; i < DISK_SIZE && allocated < size; i++) {
        if (disk->blocks[i].type == BLOCK_FREE) {
            disk->blocks[i].type = BLOCK_DATA;
            disk->blocks[i].file_id = file_id;
            disk->blocks[i].next_block = INVALID_BLOCK;

            if (start_block == INVALID_BLOCK) {
                start_block = i;
            } else {
                disk->blocks[prev_block].next_block = i;
            }

            prev_block = i;
            allocated++;
        }
    }

    // Registra o arquivo no diretorio
    disk->file_entries[slot].id = file_id;
    disk->file_entries[slot].size = size;
    disk->file_entries[slot].start_block = start_block;
    disk->file_entries[slot].active = true;

    return true;
}

bool remove_linked(Disk *disk, char file_id) {
    int slot = disk_find_file(disk, file_id);
    if (slot == -1) {
        return false;
    }

    int curr = disk->file_entries[slot].start_block;
    int steps = 0;

    // Percorre a cadeia de ponteiros liberando cada bloco
    while (curr != INVALID_BLOCK && steps < DISK_SIZE) {
        int next = disk->blocks[curr].next_block;
        disk_reset_block(&disk->blocks[curr]);
        curr = next;
        steps++;
    }

    // Remove a entrada do diretorio
    disk->file_entries[slot].id = '\0';
    disk->file_entries[slot].size = 0;
    disk->file_entries[slot].start_block = INVALID_BLOCK;
    disk->file_entries[slot].active = false;

    return true;
}
