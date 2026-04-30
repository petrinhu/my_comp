#pragma once

#include "application/vram/classificacao/sugestao_acao.h"
#include "domain/vram/classificacao/resultado_classificacao.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/gpu_info.h"
#include "domain/vram/entidades/processo.h"

#include <QVector>

#include <vector>

namespace vramopt::application {

struct ItemAuditoriaVram {
    domain::Processo               processo;
    domain::ResultadoClassificacao classificacao{};
    SugestaoAcao                   sugestao{};
    QVector<domain::OrigemConfig>  origens_config;
};

struct GrupoAuditoriaGpu {
    domain::GpuInfo                gpu;
    std::vector<ItemAuditoriaVram> itens;
};

struct RelatorioAuditoriaVram {
    std::vector<GrupoAuditoriaGpu> grupos;
};

}
