#include "allocator.h"

/* Procura o primeiro espaco contiguo livre que comporte 'size' blocos (First-Fit) */
static int find_contiguous_space(const Disk *disk, int size) {
    int count = 0;
    int start = INVALID_BLOCK;

    for (int i = 0; i < disk->total_blocks; i++) {
        if (disk->blocks[i].type == BLOCK_FREE) {
            if (count == 0) {
                start = i;
            }
            count++;
            if (count == size) {
                return start;
            }
        } else {
            count = 0;
            start = INVALID_BLOCK;
        }
    }

    return INVALID_BLOCK;
}

bool alloc_contiguous(Disk *disk, char file_id, int size) {
    if (size <= 0 || size > disk->total_blocks) {
        return false;
    }

    if (disk_find_file(disk, file_id) != -1) {
        return false;
    }

    int slot = disk_find_free_file_slot(disk);
    if (slot == -1) {
        return false;
    }

    int start_block = find_contiguous_space(disk, size);
    if (start_block == INVALID_BLOCK) {
        return false;
    }

    for (int i = start_block; i < start_block + size; i++) {
        disk->blocks[i].type = BLOCK_DATA;
        disk->blocks[i].file_id = file_id;
    }

    disk->file_entries[slot].id = file_id;
    disk->file_entries[slot].size = size;
    disk->file_entries[slot].start_block = start_block;
    disk->file_entries[slot].active = true;

    return true;
}

bool remove_contiguous(Disk *disk, char file_id) {
    int slot = disk_find_file(disk, file_id);
    if (slot == -1) {
        return false;
    }

    int start = disk->file_entries[slot].start_block;
    int size = disk->file_entries[slot].size;

    for (int i = start; i < start + size; i++) {
        disk_reset_block(&disk->blocks[i]);
    }

    disk->file_entries[slot].id = '\0';
    disk->file_entries[slot].size = 0;
    disk->file_entries[slot].start_block = INVALID_BLOCK;
    disk->file_entries[slot].active = false;

    return true;
}
