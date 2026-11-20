#include <stdio.h>
#include <ctype.h>
#include "disk.h"
#include "allocator.h"
#include "display.h"

/* Limpa o buffer de entrada do teclado */
static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* Configura ou reconfigura o disco (tamanho e tecnica) */
static void setup_disk(Disk *disk) {
    int disk_size = DEFAULT_DISK_SIZE;
    int choice = 1;

    printf("\nQuantos blocos o disco deve ter? (1 a %d, padrao %d): ",
           MAX_DISK_SIZE, DEFAULT_DISK_SIZE);
    if (scanf("%d", &disk_size) != 1) {
        disk_size = DEFAULT_DISK_SIZE;
    }
    clear_input_buffer();

    printf("Escolha a tecnica (1-Contigua | 2-Encadeada | 3-Indexada): ");
    if (scanf("%d", &choice) != 1) {
        choice = 1;
    }
    clear_input_buffer();

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

        if (scanf("%d", &op) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        if (op == 1) {
            display_disk_state(&disk, "ESTADO ATUAL");

        } else if (op == 2) {
            char id;
            int size;

            printf("Letra do arquivo (ex: A): ");
            if (scanf(" %c", &id) != 1) {
                clear_input_buffer();
                continue;
            }
            id = (char)toupper((unsigned char)id);

            printf("Tamanho em blocos: ");
            if (scanf("%d", &size) != 1) {
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();

            display_disk_state(&disk, "ANTES DE CRIAR");
            if (disk_allocate(&disk, id, size)) {
                printf(">> Arquivo '%c' (%d blocos) criado com sucesso!\n\n", id, size);
            } else {
                printf(">> ERRO: Nao foi possivel criar '%c' (sem espaco, fragmentado ou ID repetido).\n\n", id);
            }
            display_disk_state(&disk, "DEPOIS DE CRIAR");

        } else if (op == 3) {
            char id;

            printf("Letra do arquivo para remover: ");
            if (scanf(" %c", &id) != 1) {
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();
            id = (char)toupper((unsigned char)id);

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
