#include <gtest/gtest.h>
#include "../../src/infrastructure/persistence/qsettings_preferences_store.h"

#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>

using my_comp::infrastructure::persistence::QSettingsPreferencesStore;

namespace {
class QSettingsPreferencesStoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        QStandardPaths::setTestModeEnabled(true);
        QCoreApplication::setOrganizationName("my_comp");
        QCoreApplication::setApplicationName("my_comp_prefs_store_test");
        QSettings().clear();
    }
    void TearDown() override { QSettings().clear(); }
};
} // namespace

TEST_F(QSettingsPreferencesStoreTest, ValueRoundTrip) {
    QSettingsPreferencesStore store;
    EXPECT_FALSE(store.value("network/external_latency_probe", false).toBool());
    store.set_value("network/external_latency_probe", true);
    EXPECT_TRUE(QSettingsPreferencesStore().value("network/external_latency_probe").toBool());
}

// Compatibilidade: histórico gravado no formato de array antigo continua legível.
TEST_F(QSettingsPreferencesStoreTest, ReadsLegacyArrayFormat) {
    {
        QSettings s;
        s.beginWriteArray("export_history");
        s.setArrayIndex(0);
        s.setValue("timestamp", "2026-01-01T00:00:00");
        s.setValue("format", "JSON");
        s.setValue("path", "/tmp/a.json");
        s.endArray();
        s.sync();
    }
    QSettingsPreferencesStore store;
    const auto items = store.read_list("export_history");
    ASSERT_EQ(items.size(), 1);
    EXPECT_EQ(items[0].value("path").toString(), QString("/tmp/a.json"));
}

TEST_F(QSettingsPreferencesStoreTest, ListRoundTripAndRemove) {
    QSettingsPreferencesStore store;
    store.write_list("k", {QVariantMap{{"a", 1}}, QVariantMap{{"a", 2}}});
    ASSERT_EQ(store.read_list("k").size(), 2);
    EXPECT_EQ(store.read_list("k")[1].value("a").toInt(), 2);
    store.remove("k");
    EXPECT_TRUE(store.read_list("k").isEmpty());
}
