#pragma once

#include <QObject>
#include <QString>
#include <QUrl>

namespace my_comp::presentation::viewmodels {

// Ponte QML → caminho local para os diálogos de arquivo (UX-02a). O tipo `url` do QML
// (Qt 6.4) não expõe toLocalFile(); converter aqui evita replace("file://") +
// decodeURIComponent() espalhados pelas views.
class UrlHelper : public QObject {
    Q_OBJECT
public:
    explicit UrlHelper(QObject* parent = nullptr) : QObject(parent) {}

    // Caminho local de uma URL file://; vazio para URL não-local ou inválida.
    Q_INVOKABLE QString toLocalPath(const QUrl& url) const {
        return url.isLocalFile() ? url.toLocalFile() : QString();
    }
};

} // namespace my_comp::presentation::viewmodels
