#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include <QCoreApplication>
#include <QThread>
#include <QThreadPool>
#include <stdexcept>
#include "../../src/presentation/viewmodels/power_viewmodel.h"
#include "../../src/domain/interfaces/i_power_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockPowerProvider : public IPowerProvider {
public:
    MOCK_METHOD(PowerStats, collect_stats, (), (override));
};

TEST(PowerViewModelTest, ShouldRefreshStatsFromProvider) {
    auto provider = std::make_shared<MockPowerProvider>();
    PowerStats stats;
    stats.consumption_watts = 15.5;
    stats.battery_percent = 85.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(stats));

    PowerViewModel vm(provider);
    // AR-07: ctor sem I/O, initialize() dispara update() e timer.
    QSignalSpy spy(&vm, &PowerViewModel::dataChanged);
    vm.initialize();
    ASSERT_TRUE(spy.wait(2000));  // UX-05: a coleta é assíncrona

    EXPECT_DOUBLE_EQ(vm.powerWatts(), 15.5);
    EXPECT_DOUBLE_EQ(vm.batteryPercent(), 85.0);
}

// UX-05: collect_stats lê ~9 arquivos em /sys/class/power_supply a cada 2 s pelo timer.
TEST(PowerViewModelTest, ColetaForaDaThreadDaUi) {
    auto provider = std::make_shared<MockPowerProvider>();
    QThread* thread_da_coleta = nullptr;
    PowerStats stats;
    stats.battery_percent = 50.0;
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Invoke([&thread_da_coleta, stats]() {
        thread_da_coleta = QThread::currentThread();
        return stats;
    }));
    PowerViewModel vm(provider);
    QSignalSpy spy(&vm, &PowerViewModel::dataChanged);
    vm.initialize();
    ASSERT_TRUE(spy.wait(2000));
    EXPECT_NE(thread_da_coleta, QThread::currentThread());
    QThreadPool::globalInstance()->waitForDone();
}

// Falha só perde aquele tique: o próximo update() coleta de novo. Chama o slot direto,
// sem depender do timer real de 2 s.
TEST(PowerViewModelTest, FalhaNaColetaNaoTravaAsProximas) {
    auto provider = std::make_shared<MockPowerProvider>();
    PowerStats stats;
    stats.battery_percent = 42.0;
    EXPECT_CALL(*provider, collect_stats())
        .WillOnce(Invoke([]() -> PowerStats { throw std::runtime_error("sysfs sumiu"); }))
        .WillRepeatedly(Return(stats));
    PowerViewModel vm(provider);
    QSignalSpy spy(&vm, &PowerViewModel::dataChanged);
    ASSERT_TRUE(QMetaObject::invokeMethod(&vm, "update", Qt::DirectConnection));  // falha
    QThreadPool::globalInstance()->waitForDone();
    QCoreApplication::processEvents();
    EXPECT_EQ(spy.count(), 0);
    ASSERT_TRUE(QMetaObject::invokeMethod(&vm, "update", Qt::DirectConnection));
    ASSERT_TRUE(spy.wait(2000));
    EXPECT_DOUBLE_EQ(vm.batteryPercent(), 42.0);
}
