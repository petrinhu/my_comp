#include "gpu_viewmodel.h"
#include <QPointer>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

namespace my_comp::presentation::viewmodels {

GpuViewModel::GpuViewModel(std::vector<std::unique_ptr<domain::interfaces::IGpuProvider>> providers, QObject* parent)
    : QObject(parent) {
    m_providers.reserve(providers.size());
    for (auto& provider : providers) {
        m_providers.push_back(std::move(provider));
    }
    // MC-04: timer não arranca no ctor; initialize() dispara a primeira coleta.
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GpuViewModel::update_stats);
}

void GpuViewModel::initialize() {
    if (m_initialized) return;
    m_initialized = true;
    update_stats();
    m_timer->start(2000);
}

void GpuViewModel::update_stats() {
    if (m_busy || m_providers.empty()) return;
    m_busy = true;

    QPointer<GpuViewModel> self(this);
    auto providers = m_providers;
    (void)QtConcurrent::run([self, providers]() {
        std::vector<domain::models::GpuStats> all;
        for (const auto& provider : providers) {
            auto stats = provider->collect_stats();
            all.insert(all.end(), stats.begin(), stats.end());
        }
        QMetaObject::invokeMethod(self.data(), [self, all]() {
            if (!self) return;
            self->m_busy = false;
            self->apply_stats(all);
        }, Qt::QueuedConnection);
    });
}

void GpuViewModel::apply_stats(const std::vector<domain::models::GpuStats>& stats_list) {
    QVariantList new_list;
    for (const auto& stats : stats_list) {
        QVariantMap map;
        map["modelName"] = stats.model_name;
        map["vendor"] = stats.vendor;
        map["load"] = stats.load_percent;
        map["temp"] = stats.temperature;
        map["vramUsed"] = static_cast<double>(stats.vram_used_mb);
        map["vramTotal"] = static_cast<double>(stats.vram_total_mb);
        map["vramPercent"] = stats.vram_usage_percent;
        new_list.append(map);
    }

    // Primeira coleta sempre notifica (a UI sai do estado "carregando" mesmo sem GPU).
    if (m_gpus != new_list || !m_has_emitted) {
        m_gpus = new_list;
        m_has_emitted = true;
        emit gpusChanged();
    }
}

}
