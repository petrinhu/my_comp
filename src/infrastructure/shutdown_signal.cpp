#include "shutdown_signal.h"

#include <atomic>

namespace my_comp::infrastructure {

namespace {
std::atomic<bool>& flag() noexcept {
    static std::atomic<bool> encerrando{false};
    return encerrando;
}
}  // namespace

void request_shutdown() noexcept { flag().store(true, std::memory_order_release); }

bool shutdown_requested() noexcept { return flag().load(std::memory_order_acquire); }

void reset_shutdown_for_tests() noexcept { flag().store(false, std::memory_order_release); }

} // namespace my_comp::infrastructure
