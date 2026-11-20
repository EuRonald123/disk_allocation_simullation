#include <stdio.h>
#include <ctype.h>
#include "disk.h"
#include "allocator.h"
#include "display.h"

/* Le um inteiro usando fgets; retorna default_val se a entrada for vazia ou invalida */
static int read_int(int default_val) {
    char buf[64];
    int val;
    if (fgets(buf, sizeof(buf), stdin) != NULL && sscanf(buf, "%d", &val) == 1) {
        return val;
    }
    return default_val;
}

/* Le um caractere usando fgets e converte para maiusculo */
static char read_char(void) {
    char buf[64];
    char c = '\0';
    if (fgets(buf, sizeof(buf), stdin) != NULL && sscanf(buf, " %c", &c) == 1) {
        return (char)toupper((unsigned char)c);
    }
    return '\0';
}

/* Configura ou reconfigura o disco (tamanho e tecnica) */
static void setup_disk(Disk *disk) {
    printf("\nQuantos blocos o disco deve ter? (1 a %d, Enter = %d): ",
           MAX_DISK_SIZE, DEFAULT_DISK_SIZE);
    int disk_size = read_int(DEFAULT_DISK_SIZE);

    printf("Escolha a tecnica (1-Contigua | 2-Encadeada | 3-Indexada): ");
    int choice = read_int(1);

    AllocationMethod method = ALLOC_CONTIGUOUS;
    if (choice == 2) method = ALLOC_LINKED;
    else if (choice == 3) method = ALLOC_INDEXED;

    disk_init(disk, method, disk_size);
}

int main(void) {
    Disk disk;
    setup_disk(&disk);

    int op = -1;
    while (op != 0) {
        printf("\n=== DISCO %s (%d blocos) ===\n",
               display_method_name(disk.allocation_method), disk.total_blocks);
        printf("  1 - Ver estado do disco\n");
        printf("  2 - Criar arquivo\n");
        printf("  3 - Remover arquivo\n");
        printf("  4 - Reiniciar disco (trocar tamanho/tecnica)\n");
        printf("  0 - Sair\n");
        printf("Opcao: ");

        op = read_int(-1);

        if (op == 1) {
            display_disk_state(&disk, "ESTADO ATUAL");

        } else if (op == 2) {
            printf("Letra do arquivo (ex: A): ");
            char id = read_char();
            if (id == '\0') {
                printf(">> ERRO: ID invalido.\n");
                continue;
            }

            printf("Tamanho em blocos: ");
            int size = read_int(0);

            display_disk_state(&disk, "ANTES DE CRIAR");
            if (disk_allocate(&disk, id, size)) {
                printf(">> Arquivo '%c' (%d blocos) criado com sucesso!\n\n", id, size);
            } else {
                printf(">> ERRO: Nao foi possivel criar '%c' (sem espaco, fragmentado ou ID repetido).\n\n", id);
            }
            display_disk_state(&disk, "DEPOIS DE CRIAR");

        } else if (op == 3) {
            printf("Letra do arquivo para remover: ");
            char id = read_char();
            if (id == '\0') {
                printf(">> ERRO: ID invalido.\n");
                continue;
            }

            display_disk_state(&disk, "ANTES DE REMOVER");
            if (disk_remove(&disk, id)) {
                printf(">> Arquivo '%c' removido com sucesso!\n\n", id);
            } else {
                printf(">> ERRO: Arquivo '%c' nao encontrado.\n\n", id);
            }
            display_disk_state(&disk, "DEPOIS DE REMOVER");

        } else if (op == 4) {
            setup_disk(&disk);
        }
    }

    return 0;
}
