#include "hdparm_benchmark_provider.h"
#include <QElapsedTimer>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

domain::models::BenchmarkResult HdparmBenchmarkProvider::run(const QString& device) {
    domain::models::BenchmarkResult r;
    r.device = device;

    QString out;
    QElapsedTimer t;
    t.start();
    const int rc = m_shell->execute("hdparm", {"-tT", "/dev/" + device}, out);
    r.duration_s = t.elapsed() / 1000.0;

    if (rc != 0 || out.isEmpty()) {
        r.error_message = out.isEmpty()
            ? "hdparm ausente ou sem permissão (requer root/pkexec para I/O bruto)"
            : out.trimmed();
        return r;
    }

    // Saída típica:
    //   Timing cached reads:   12345 MB in  2.00 seconds = 6172.50 MB/sec
    //   Timing buffered disk reads: 500 MB in  3.01 seconds = 166.11 MB/sec
    static const QRegularExpression re_cached(R"(cached reads:.*=\s*([\d.]+)\s*MB/sec)");
    static const QRegularExpression re_seq(R"(buffered disk reads:.*=\s*([\d.]+)\s*MB/sec)");

    const auto m_cached = re_cached.match(out);
    const auto m_seq = re_seq.match(out);

    if (m_cached.hasMatch()) r.cached_mb_s = m_cached.captured(1).toDouble();
    if (m_seq.hasMatch()) r.seq_read_mb_s = m_seq.captured(1).toDouble();

    r.success = (r.seq_read_mb_s > 0.0 || r.cached_mb_s > 0.0);
    if (!r.success) r.error_message = "hdparm não retornou valores parseáveis";
    return r;
}

} // namespace my_comp::infrastructure::collectors
