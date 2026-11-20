#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdbool.h>
#include "disk.h"

/* Aloca e remove arquivos usando Alocacao Contigua (First-Fit) */
bool alloc_contiguous(Disk *disk, char file_id, int size);
bool remove_contiguous(Disk *disk, char file_id);

/* Aloca e remove arquivos usando Alocacao Encadeada */
bool alloc_linked(Disk *disk, char file_id, int size);
bool remove_linked(Disk *disk, char file_id);

/* Aloca e remove arquivos usando Alocacao Indexada */
bool alloc_indexed(Disk *disk, char file_id, int size);
bool remove_indexed(Disk *disk, char file_id);

/* Funcoes gerais que direcionam para a tecnica configurada no disco */
bool disk_allocate(Disk *disk, char file_id, int size);
bool disk_remove(Disk *disk, char file_id);

#endif // ALLOCATOR_H
