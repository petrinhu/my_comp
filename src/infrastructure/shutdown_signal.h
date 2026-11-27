#pragma once

namespace my_comp::infrastructure {

// Sinal global de "o app está fechando". O main liga no aboutToQuit; quem espera processo
// externo (QProcessShellExecutor) checa e desiste cedo, para o waitForDone do thread pool
// no encerramento durar menos de um segundo em vez de esperar um dnf de até 180 s.
void request_shutdown() noexcept;
[[nodiscard]] bool shutdown_requested() noexcept;

// Só para testes: volta ao estado inicial.
void reset_shutdown_for_tests() noexcept;

} // namespace my_comp::infrastructure
