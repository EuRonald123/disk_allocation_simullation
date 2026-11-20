#ifndef DISPLAY_H
#define DISPLAY_H

#include "disk.h"

/*
 Retorna o nome legível do método de alocação configurado no disco
 */
const char* display_method_name(AllocationMethod method);

/*
 Desenha apenas o mapa visual dos blocos do disco (ex: [ . ], [ A ], [ A* ])
 */
void display_disk_map(const Disk *disk);

/*
 Exibe a tabela de diretório com os arquivos ativos e a estrutura de blocos
 correspondente à técnica atual (intervalo contíguo, lista encadeada ou tabela de índice)
 */
void display_file_table(const Disk *disk);

/*
 Exibe um resumo completo do estado atual do disco:
 cabeçalho, estatísticas de ocupação/fragmentação, mapa visual e tabela de arquivos
 */
void display_disk_state(const Disk *disk, const char *title);

#endif // DISPLAY_H
