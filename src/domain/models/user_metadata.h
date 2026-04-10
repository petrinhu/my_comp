#pragma once
#include <QString>
#include <QStringList>
#include <QMap>
#include <vector>

namespace my_comp::domain::models {

struct UserMetadata {
    QString currentUser;
    QString currentUid;
    QString currentGid;
    QStringList groups;
    QString shell;
    QString home;
    
    QMap<QString, QString> envVars;
    QStringList systemLimits;
    QStringList loginHistory;
    QStringList systemUsers;
};

} // namespace my_comp::domain::models
