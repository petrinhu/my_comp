#pragma once

#include "application/vram/classificacao/sugestao_acao.h"
#include "domain/vram/classificacao/resultado_classificacao.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/gpu_info.h"
#include "domain/vram/entidades/processo.h"

#include <QVector>

#include <vector>

namespace my_comp::application::vram {

struct ItemAuditoriaVram {
    domain::vram::Processo               processo;
    domain::vram::ResultadoClassificacao classificacao{};
    SugestaoAcao                   sugestao{};
    QVector<domain::vram::OrigemConfig>  origens_config;
};

struct GrupoAuditoriaGpu {
    domain::vram::GpuInfo                gpu;
    std::vector<ItemAuditoriaVram> itens;
};

struct RelatorioAuditoriaVram {
    std::vector<GrupoAuditoriaGpu> grupos;
};

}
