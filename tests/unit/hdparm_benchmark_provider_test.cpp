#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/infrastructure/collectors/hdparm_benchmark_provider.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(HdparmBenchmarkProviderTest, ShouldParseCachedAndBufferedReads) {
    auto shell = std::make_shared<MockShellExecutor>();
    const QString out =
        "/dev/sda:\n"
        " Timing cached reads:   24000 MB in  2.00 seconds = 12005.34 MB/sec\n"
        " Timing buffered disk reads: 500 MB in  3.01 seconds = 166.11 MB/sec\n";
    EXPECT_CALL(*shell, execute(QString("hdparm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(out), Return(0)));

    HdparmBenchmarkProvider provider(shell);
    auto r = provider.run("sda");

    EXPECT_EQ(r.device, "sda");
    EXPECT_TRUE(r.success);
    EXPECT_DOUBLE_EQ(r.cached_mb_s, 12005.34);
    EXPECT_DOUBLE_EQ(r.seq_read_mb_s, 166.11);
    EXPECT_TRUE(r.error_message.isEmpty());
}

TEST(HdparmBenchmarkProviderTest, ShouldReportErrorOnNonZeroExit) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("hdparm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("hdparm: need root")), Return(1)));

    HdparmBenchmarkProvider provider(shell);
    auto r = provider.run("sda");

    EXPECT_FALSE(r.success);
    EXPECT_EQ(r.error_message, "hdparm: need root");
}

TEST(HdparmBenchmarkProviderTest, ShouldReportErrorOnEmptyOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("hdparm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("")), Return(127)));

    HdparmBenchmarkProvider provider(shell);
    auto r = provider.run("sda");

    EXPECT_FALSE(r.success);
    EXPECT_TRUE(r.error_message.contains("hdparm"));
}

TEST(HdparmBenchmarkProviderTest, ShouldSucceedWithPartialMatchCachedOnly) {
    auto shell = std::make_shared<MockShellExecutor>();
    const QString out = " Timing cached reads:   1000 MB in  2.00 seconds = 500.00 MB/sec\n";
    EXPECT_CALL(*shell, execute(QString("hdparm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(out), Return(0)));

    HdparmBenchmarkProvider provider(shell);
    auto r = provider.run("sda");

    EXPECT_TRUE(r.success);
    EXPECT_DOUBLE_EQ(r.cached_mb_s, 500.00);
    EXPECT_DOUBLE_EQ(r.seq_read_mb_s, 0.0);
}

TEST(HdparmBenchmarkProviderTest, ShouldFailWhenRegexMatchesNothing) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("hdparm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("saída estranha sem números")), Return(0)));

    HdparmBenchmarkProvider provider(shell);
    auto r = provider.run("sda");

    EXPECT_FALSE(r.success);
    EXPECT_EQ(r.error_message, "hdparm não retornou valores parseáveis");
}
