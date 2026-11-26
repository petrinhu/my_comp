// Ponto de entrada: escolhe CLI ou GUI e delega a montagem ao composition root
// (src/composition/, MC-02: DI manual, um módulo compor_<feature>() por feature).

#include "composition/app_composition.h"
#include "composition/vram_composition.h"
#include "presentation/cli/cli_vram.h"

#include <QCoreApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QPalette>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStringList>
#include <QStyleHints>
#include <QTextStream>

using namespace Qt::StringLiterals;

namespace {

// Subcomando CLI: my_comp vram [--json|--all-processes|--no-config-origins]
int executar_cli_vram(int argc, char* argv[]) {
    QCoreApplication cli_app(argc, argv);
    // Mesmos nomes da GUI: o feedback VRAM vive em AppDataLocation, que deriva deles.
    QCoreApplication::setOrganizationName(QStringLiteral("my_comp"));
    QCoreApplication::setApplicationName(QStringLiteral("my_comp"));
    QStringList sub_args;
    for (int i = 2; i < argc; ++i) {
        sub_args << QString::fromUtf8(argv[i]);
    }
    QTextStream out{stdout};
    return vramopt::presentation::cli::executar_com_servico(
        vramopt::composition::compor_servico_auditar_vram(), sub_args, out);
}

bool sistema_em_tema_escuro(const QGuiApplication& app) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return app.styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    // Qt < 6.5 não expõe colorScheme(): infere pelo contraste da paleta do sistema.
    (void)app;
    const QPalette sys_palette = QGuiApplication::palette();
    return sys_palette.color(QPalette::Window).lightness()
           < sys_palette.color(QPalette::WindowText).lightness();
#endif
}

int executar_gui(int argc, char* argv[]) {
    // Wayland quando há sessão Wayland; em X11 o Qt escolhe xcb sozinho. Nunca sobrescreve
    // escolha explícita do usuário (ex.: QT_QPA_PLATFORM=xcb). AUDIT-04.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM") && !qEnvironmentVariableIsEmpty("WAYLAND_DISPLAY")) {
        qputenv("QT_QPA_PLATFORM", "wayland");
    }

    QGuiApplication app(argc, argv);
    app.setOrganizationName("my_comp");
    app.setApplicationName("my_comp");

    // Declarados antes do engine: o engine (e o QML) é destruído primeiro.
    const auto view_models = my_comp::composition::compor_app();

    QQmlApplicationEngine engine;
    auto* ctx = engine.rootContext();
    ctx->setContextProperty("isSystemDarkInitial", sistema_em_tema_escuro(app));
    my_comp::composition::registrar_no_qml(*ctx, view_models);

    const QUrl url(u"qrc:/Main.qml"_s);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& obj_url) {
            if (!obj && url == obj_url) {
                qCritical() << "[Main] Failed to load Main.qml";
                QCoreApplication::exit(-1);
            }
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc >= 2 && QString::fromUtf8(argv[1]) == QStringLiteral("vram")) {
            return executar_cli_vram(argc, argv);
        }
        return executar_gui(argc, argv);
    } catch (const std::exception& e) {
        qCritical() << "[Main] UNHANDLED EXCEPTION:" << e.what();
        return -1;
    } catch (...) {
        qCritical() << "[Main] UNKNOWN ERROR CAUSING CRASH";
        return -1;
    }
}
