#include "disk.h"
#include "allocator.h"

void disk_reset_block(Block *block) {
    block->type = BLOCK_FREE;
    block->file_id = '\0';
    block->next_block = INVALID_BLOCK;
    block->index_count = 0;

    for (int i = 0; i < DISK_SIZE; i++) {
        block->index_table[i] = INVALID_BLOCK;
    }
}

void disk_init(Disk *disk, AllocationMethod method) {
    disk->allocation_method = method;

    for (int i = 0; i < DISK_SIZE; i++) {
        disk_reset_block(&disk->blocks[i]);
    }

    for (int i = 0; i < MAX_FILES; i++) {
        disk->file_entries[i].id = '\0';
        disk->file_entries[i].size = 0;
        disk->file_entries[i].start_block = INVALID_BLOCK;
        disk->file_entries[i].active = false;
    }
}

int disk_find_file(const Disk *disk, char file_id) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (disk->file_entries[i].active && disk->file_entries[i].id == file_id) {
            return i;
        }
    }
    return -1;
}

int disk_find_free_file_slot(const Disk *disk) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!disk->file_entries[i].active) {
            return i;
        }
    }
    return -1;
}

int disk_count_free_blocks(const Disk *disk) {
    int count = 0;

    for (int i = 0; i < DISK_SIZE; i++) {
        if (disk->blocks[i].type == BLOCK_FREE) {
            count++;
        }
    }

    return count;
}

bool disk_allocate(Disk *disk, char file_id, int size) {
    switch (disk->allocation_method) {
        case ALLOC_CONTIGUOUS:
            return alloc_contiguous(disk, file_id, size);
        case ALLOC_LINKED:
            return alloc_linked(disk, file_id, size);
        case ALLOC_INDEXED:
            return alloc_indexed(disk, file_id, size);
        default:
            return false;
    }
}

bool disk_remove(Disk *disk, char file_id) {
    switch (disk->allocation_method) {
        case ALLOC_CONTIGUOUS:
            return remove_contiguous(disk, file_id);
        case ALLOC_LINKED:
            return remove_linked(disk, file_id);
        case ALLOC_INDEXED:
            return remove_indexed(disk, file_id);
        default:
            return false;
    }
}
