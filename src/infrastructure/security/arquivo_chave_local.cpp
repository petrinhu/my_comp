#include "infrastructure/security/arquivo_chave_local.h"

#include <QFile>
#include <QtGlobal>

#include <openssl/crypto.h>
#include <openssl/rand.h>

#include <cerrno>
#include <cstring>
#include <utility>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace my_comp::infrastructure::security {

namespace {

constexpr mode_t MODO_SO_DONO = S_IRUSR | S_IWUSR;          // 0600
constexpr mode_t BITS_GRUPO_OUTROS = S_IRWXG | S_IRWXO;      // 0077

// RAII do descritor POSIX (MC-04: nada de close() manual espalhado).
class Descritor {
public:
    explicit Descritor(int fd) noexcept : m_fd{fd} {}
    ~Descritor() {
        if (m_fd >= 0) {
            ::close(m_fd);
        }
    }
    Descritor(const Descritor&) = delete;
    Descritor& operator=(const Descritor&) = delete;
    Descritor(Descritor&&) = delete;
    Descritor& operator=(Descritor&&) = delete;

    [[nodiscard]] int get() const noexcept { return m_fd; }
    [[nodiscard]] bool valido() const noexcept { return m_fd >= 0; }

private:
    int m_fd;
};

bool ler_exato(int fd, std::uint8_t* destino, std::size_t tamanho) {
    std::size_t lidos = 0;
    while (lidos < tamanho) {
        const auto n = ::read(fd, destino + lidos, tamanho - lidos);
        if (n < 0 && errno == EINTR) {
            continue;
        }
        if (n <= 0) {
            return false;
        }
        lidos += static_cast<std::size_t>(n);
    }
    return true;
}

bool escrever_exato(int fd, const std::uint8_t* origem, std::size_t tamanho) {
    std::size_t escritos = 0;
    while (escritos < tamanho) {
        const auto n = ::write(fd, origem + escritos, tamanho - escritos);
        if (n < 0 && errno == EINTR) {
            continue;
        }
        if (n <= 0) {
            return false;
        }
        escritos += static_cast<std::size_t>(n);
    }
    return true;
}

}  // namespace

ArquivoChaveLocal::ArquivoChaveLocal(QString caminho) : m_caminho{std::move(caminho)} {}

bool ArquivoChaveLocal::existe() const {
    struct stat st{};
    return ::lstat(QFile::encodeName(m_caminho).constData(), &st) == 0;
}

std::optional<std::vector<std::uint8_t>> ArquivoChaveLocal::ler() const {
    const QByteArray caminho = QFile::encodeName(m_caminho);
    const Descritor fd{::open(caminho.constData(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW)};
    if (!fd.valido()) {
        if (errno != ENOENT) {
            qWarning("chave local: não foi possível abrir %s (%s)", caminho.constData(), std::strerror(errno));
        }
        return std::nullopt;
    }
    struct stat st{};
    if (::fstat(fd.get(), &st) != 0 || !S_ISREG(st.st_mode)) {
        qWarning("chave local: %s não é arquivo regular; recusada", caminho.constData());
        return std::nullopt;
    }
    if (st.st_uid != ::geteuid() || (st.st_mode & BITS_GRUPO_OUTROS) != 0) {
        qWarning("chave local: %s precisa pertencer ao usuário e ter modo 0600; recusada",
                 caminho.constData());
        return std::nullopt;
    }
    if (st.st_size != static_cast<off_t>(TAMANHO_CHAVE)) {
        qWarning("chave local: %s não tem %zu bytes; recusada", caminho.constData(), TAMANHO_CHAVE);
        return std::nullopt;
    }
    std::vector<std::uint8_t> chave(TAMANHO_CHAVE);
    if (!ler_exato(fd.get(), chave.data(), chave.size())) {
        OPENSSL_cleanse(chave.data(), chave.size());
        qWarning("chave local: leitura incompleta de %s", caminho.constData());
        return std::nullopt;
    }
    return chave;
}

std::optional<std::vector<std::uint8_t>> ArquivoChaveLocal::ler_ou_criar() const {
    const QByteArray caminho = QFile::encodeName(m_caminho);
    // O_EXCL fecha a corrida "checa e cria" e, junto com O_NOFOLLOW, recusa link simbólico.
    const Descritor fd{::open(caminho.constData(),
                              O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW, MODO_SO_DONO)};
    if (!fd.valido()) {
        if (errno == EEXIST) {
            return ler();
        }
        qWarning("chave local: não foi possível criar %s (%s)", caminho.constData(), std::strerror(errno));
        return std::nullopt;
    }

    std::vector<std::uint8_t> chave(TAMANHO_CHAVE);
    const bool gerou = RAND_bytes(chave.data(), static_cast<int>(chave.size())) == 1;
    const bool gravou = gerou && escrever_exato(fd.get(), chave.data(), chave.size())
                        && ::fsync(fd.get()) == 0;
    if (!gravou) {
        OPENSSL_cleanse(chave.data(), chave.size());
        ::unlink(caminho.constData());  // não deixa chave parcial para trás
        qWarning("chave local: falha ao gerar/gravar %s", caminho.constData());
        return std::nullopt;
    }
    return chave;
}

}
