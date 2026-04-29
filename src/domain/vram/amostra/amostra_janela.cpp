#include "domain/vram/amostra/amostra_janela.h"

namespace vramopt::domain {

double AmostraJanela::razaoVramSobreTotal() const noexcept {
    return vram_media.razaoSobre(vram_total_gpu);
}

}
