#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <vector>
#include "../../src/domain/models/gpu_stats.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/nvidia_gpu_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(NvidiaGpuProviderTest, ShouldParseNvidiaSmiOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    // nvidia-smi --format=csv,noheader,nounits output:
    // NVIDIA GeForce RTX 3050, 45, 62, 8192, 2048
    QString mock_output = "NVIDIA GeForce RTX 3050, 45, 62, 8192, 2048\n";

    EXPECT_CALL(*shell, execute(QString("nvidia-smi"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));

    NvidiaGpuProvider provider(shell);
    auto results = provider.collect_stats();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].model_name, "NVIDIA GeForce RTX 3050");
    EXPECT_DOUBLE_EQ(results[0].load_percent, 45.0);
    EXPECT_DOUBLE_EQ(results[0].temperature, 62.0);
    EXPECT_EQ(results[0].vram_total_mb, 8192);
    EXPECT_EQ(results[0].vram_used_mb, 2048);
    EXPECT_DOUBLE_EQ(results[0].vram_usage_percent, 25.0);
}
