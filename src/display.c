#include <stdio.h>
#include "display.h"

#define BLOCKS_PER_ROW 8

/* Calcula o maior segmento de blocos livres consecutivos no disco */
static int calculate_max_contiguous_free(const Disk *disk) {
    int max_run = 0;
    int current_run = 0;

    for (int i = 0; i < DISK_SIZE; i++) {
        if (disk->blocks[i].type == BLOCK_FREE) {
            current_run++;
            if (current_run > max_run) {
                max_run = current_run;
            }
        } else {
            current_run = 0;
        }
    }

    return max_run;
}

const char* display_method_name(AllocationMethod method) {
    switch (method) {
        case ALLOC_CONTIGUOUS:
            return "CONTIGUA";
        case ALLOC_LINKED:
            return "ENCADEADA";
        case ALLOC_INDEXED:
            return "INDEXADA";
        default:
            return "DESCONHECIDA";
    }
}

void display_disk_map(const Disk *disk) {
    printf("\nMapa de Blocos (Legenda: [ . ] = Livre | [ X ] = Dados | [ X* ] = Indice):\n");

    for (int row_start = 0; row_start < DISK_SIZE; row_start += BLOCKS_PER_ROW) {
        int row_end = row_start + BLOCKS_PER_ROW;
        if (row_end > DISK_SIZE) {
            row_end = DISK_SIZE;
        }

        // Linha 1: Índices dos blocos
        printf("  Idx: ");
        for (int i = row_start; i < row_end; i++) {
            printf("  %02d   ", i);
        }
        printf("\n");

        // Linha 2: Conteúdo de cada bloco
        printf("  Blk: ");
        for (int i = row_start; i < row_end; i++) {
            const Block *b = &disk->blocks[i];
            if (b->type == BLOCK_FREE) {
                printf("[  . ] ");
            } else if (b->type == BLOCK_INDEX) {
                printf("[ %c* ] ", b->file_id);
            } else {
                printf("[  %c ] ", b->file_id);
            }
        }
        printf("\n\n");
    }
}

void display_file_table(const Disk *disk) {
    printf("Tabela de Diretorio (Arquivos Ativos):\n");

    int active_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        const FileEntry *entry = &disk->file_entries[i];
        if (!entry->active) {
            continue;
        }
        active_count++;

        printf("  - Arquivo '%c' | Tam. Dados: %2d bloco(s) | ", entry->id, entry->size);

        if (disk->allocation_method == ALLOC_CONTIGUOUS) {
            int end_block = entry->start_block + entry->size - 1;
            printf("Inicio: %02d | Fim: %02d | Blocos: [%02d .. %02d]\n",
                   entry->start_block, end_block, entry->start_block, end_block);

        } else if (disk->allocation_method == ALLOC_LINKED) {
            printf("Inicio: %02d | Cadeia: ", entry->start_block);
            int curr = entry->start_block;
            int safety_steps = 0; // Proteção contra loop infinito caso haja bug nos ponteiros

            while (curr != INVALID_BLOCK && safety_steps < DISK_SIZE) {
                printf("%02d -> ", curr);
                curr = disk->blocks[curr].next_block;
                safety_steps++;
            }
            printf("FIM(-1)\n");

        } else if (disk->allocation_method == ALLOC_INDEXED) {
            int idx_blk = entry->start_block;
            const Block *index_block = &disk->blocks[idx_blk];

            printf("Bloco Indice: %02d (Total no disco: %d blocos) | Dados: { ",
                   idx_blk, entry->size + 1);

            for (int j = 0; j < index_block->index_count; j++) {
                printf("%02d%s",
                       index_block->index_table[j],
                       (j < index_block->index_count - 1) ? ", " : " ");
            }
            printf("}\n");
        }
    }

    if (active_count == 0) {
        printf("  (Nenhum arquivo alocado no momento)\n");
    }
}

void display_disk_state(const Disk *disk, const char *title) {
    int free_blocks = disk_count_free_blocks(disk);
    int used_blocks = DISK_SIZE - free_blocks;
    int max_contiguous = calculate_max_contiguous_free(disk);

    printf("====================================================================\n");
    if (title != NULL) {
        printf(" %s\n", title);
        printf("--------------------------------------------------------------------\n");
    }
    printf(" Tecnica: %-10s | Total: %2d | Ocupados: %2d | Livres: %2d (Maior contiguo: %2d)\n",
           display_method_name(disk->allocation_method),
           DISK_SIZE,
           used_blocks,
           free_blocks,
           max_contiguous);
    printf("====================================================================\n");

    display_disk_map(disk);
    display_file_table(disk);
    printf("====================================================================\n\n");
}
