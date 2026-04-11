#pragma once
#include <QString>
#include <QVariantMap>
#include <QDateTime>

namespace my_comp::domain::models {

struct ReportData {
    QString title;
    QString timestamp;
    QVariantMap sections; // Chave: Nome da Seção, Valor: Dados (pode ser QVariantList ou QVariantMap)
    
    // Metadados do relatório
    QString distro;
    QString kernel;
    QString user;
};

} // namespace my_comp::domain::models
