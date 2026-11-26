#include "domain/vram/amostra/amostra_janela.h"

namespace my_comp::domain::vram {

double AmostraJanela::razao_vram_sobre_total() const noexcept {
    return vram_media.razao_sobre(vram_total_gpu);
}

}
