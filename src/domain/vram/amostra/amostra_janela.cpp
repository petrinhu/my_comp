#include "domain/vram/amostra/amostra_janela.h"

namespace vramopt::domain {

double AmostraJanela::razao_vram_sobre_total() const noexcept {
    return vram_media.razao_sobre(vram_total_gpu);
}

}
