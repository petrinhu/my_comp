#pragma once

#include <QCoreApplication>
#include <QMetaObject>
#include <QPointer>

#include <utility>

namespace my_comp::presentation::viewmodels {

// Publica `f(vm)` na thread da UI a partir de um worker (QtConcurrent).
//
// O evento é postado no QCoreApplication, que vive mais que os ViewModels, e o QPointer só
// é consultado já na thread da UI. Postar direto no ViewModel (invokeMethod(self.data(), ...))
// tem uma janela de use-after-free: o worker lê o ponteiro, a UI destrói o objeto e o
// postEvent usa memória liberada (achado da revisão do UX-05). Se o ViewModel já foi
// destruído quando o evento roda, `f` não é chamado.
template <typename T, typename F>
void post_to_ui(const QPointer<T>& self, F&& f) {
    QCoreApplication* const app = QCoreApplication::instance();
    if (app == nullptr) {
        return;
    }
    QMetaObject::invokeMethod(
        app,
        [self, f = std::forward<F>(f)]() mutable {
            if (T* const vm = self.data()) {
                f(vm);
            }
        },
        Qt::QueuedConnection);
}

} // namespace my_comp::presentation::viewmodels
