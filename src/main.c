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

/* Executa uma operacao de alocacao exibindo o resultado */
static void test_alloc(Disk *disk, char id, int size) {
    printf(">> Tentando CRIAR arquivo '%c' (%d blocos)...\n", id, size);
    if (disk_allocate(disk, id, size)) {
        printf("   [OK] Arquivo '%c' alocado com sucesso!\n\n", id);
    } else {
        printf("   [ERRO] Falha ao alocar '%c' (sem espaco, fragmentacao ou ID repetido).\n\n", id);
    }
}

/* Executa uma operacao de remocao exibindo o resultado */
static void test_remove(Disk *disk, char id) {
    printf(">> Tentando REMOVER arquivo '%c'...\n", id);
    if (disk_remove(disk, id)) {
        printf("   [OK] Arquivo '%c' removido com sucesso!\n\n", id);
    } else {
        printf("   [ERRO] Arquivo '%c' nao encontrado.\n\n", id);
    }
}

/* Roda um cenario completo em uma tecnica especifica mostrando ANTES e DEPOIS */
static void run_preset_for_method(AllocationMethod method) {
    Disk disk;
    disk_init(&disk, method);

    printf("\n####################################################################\n");
    printf("   SIMULACAO AUTOMATICA - TECNICA: %s\n", display_method_name(method));
    printf("####################################################################\n\n");

    // 1. Estado inicial
    display_disk_state(&disk, "1. ESTADO INICIAL DO DISCO (VAZIO)");

    // 2. Alocacao inicial de 4 arquivos
    test_alloc(&disk, 'A', 6);
    test_alloc(&disk, 'B', 5);
    test_alloc(&disk, 'C', 8);
    test_alloc(&disk, 'D', 6);
    display_disk_state(&disk, "2. APOS ALOCAR ARQUIVOS A(6), B(5), C(8) e D(6)");

    // 3. Remocao de B e D (cria "buracos" no disco para testar fragmentacao)
    test_remove(&disk, 'B');
    test_remove(&disk, 'D');
    display_disk_state(&disk, "3. APOS REMOVER ARQUIVOS B(5) e D(6)");

    // 4. Tentativa de alocar arquivo E(9):
    // Ha blocos livres suficientes no total, mas espalhados (5 no buraco de B + livres no fim)
    test_alloc(&disk, 'E', 9);
    display_disk_state(&disk, "4. ESTADO FINAL APOS TENTAR ALOCAR ARQUIVO E(9)");
}

/* Menu interativo para manipular o disco manualmente */
static void run_interactive_mode(void) {
    Disk disk;
    int choice = 0;

    printf("\nEscolha a tecnica inicial:\n");
    printf("  1 - Contigua\n");
    printf("  2 - Encadeada\n");
    printf("  3 - Indexada\n");
    printf("Opcao: ");
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    AllocationMethod method = ALLOC_CONTIGUOUS;
    if (choice == 2) method = ALLOC_LINKED;
    else if (choice == 3) method = ALLOC_INDEXED;

    disk_init(&disk, method);

    int op = -1;
    while (op != 0) {
        printf("\n--- MENU INTERATIVO (%s) ---\n", display_method_name(disk.allocation_method));
        printf("  1 - Exibir estado do disco\n");
        printf("  2 - Criar arquivo (mostra Antes e Depois)\n");
        printf("  3 - Remover arquivo (mostra Antes e Depois)\n");
        printf("  4 - Resetar / Trocar tecnica do disco\n");
        printf("  0 - Voltar ao menu principal\n");
        printf("Escolha: ");

        if (scanf("%d", &op) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        if (op == 1) {
            display_disk_state(&disk, "ESTADO ATUAL DO DISCO");

        } else if (op == 2) {
            char id;
            int size;
            printf("ID do arquivo (1 letra, ex: A): ");
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

            display_disk_state(&disk, "ANTES DA ALOCACAO");
            test_alloc(&disk, id, size);
            display_disk_state(&disk, "DEPOIS DA ALOCACAO");

        } else if (op == 3) {
            char id;
            printf("ID do arquivo para remover: ");
            if (scanf(" %c", &id) != 1) {
                clear_input_buffer();
                continue;
            }
            clear_input_buffer();
            id = (char)toupper((unsigned char)id);

            display_disk_state(&disk, "ANTES DA REMOCAO");
            test_remove(&disk, id);
            display_disk_state(&disk, "DEPOIS DA REMOCAO");

        } else if (op == 4) {
            return;
        }
    }
}

int main(void) {
    int option = -1;

    while (option != 0) {
        printf("\n====================================================================\n");
        printf("     SIMULADOR DE ALOCACAO DE ARQUIVOS EM DISCO (SO I)\n");
        printf("====================================================================\n");
        printf("  1 - Rodar Preset Comparativo nas 3 Tecnicas (Ideal para o Relatorio)\n");
        printf("  2 - Rodar Preset apenas na Alocacao CONTIGUA\n");
        printf("  3 - Rodar Preset apenas na Alocacao ENCADEADA\n");
        printf("  4 - Rodar Preset apenas na Alocacao INDEXADA\n");
        printf("  5 - Modo Interativo (Criar/Remover manualmente)\n");
        printf("  0 - Sair\n");
        printf("--------------------------------------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &option) != 1) {
            clear_input_buffer();
            printf("Entrada invalida! Digite um numero.\n");
            continue;
        }
        clear_input_buffer();

        switch (option) {
            case 1:
                run_preset_for_method(ALLOC_CONTIGUOUS);
                run_preset_for_method(ALLOC_LINKED);
                run_preset_for_method(ALLOC_INDEXED);
                break;
            case 2:
                run_preset_for_method(ALLOC_CONTIGUOUS);
                break;
            case 3:
                run_preset_for_method(ALLOC_LINKED);
                break;
            case 4:
                run_preset_for_method(ALLOC_INDEXED);
                break;
            case 5:
                run_interactive_mode();
                break;
            case 0:
                printf("Encerrando simulador.\n");
                break;
            default:
                printf("Opcao invalida!\n");
                break;
        }
    }

    return 0;
}
