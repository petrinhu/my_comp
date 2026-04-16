#pragma once
#include <QObject>
#include <QVariantList>
#include <QTimer>
#include <QtConcurrent>
#include "../../domain/interfaces/i_process_provider.h"
#include <memory>
#include <vector>

namespace my_comp::presentation::viewmodels {

class ProcessViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList processes READ processes NOTIFY processesChanged)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)

public:
    explicit ProcessViewModel(std::shared_ptr<domain::interfaces::IProcessProvider> provider, QObject* parent = nullptr);

    QVariantList processes() const;

    QString filter() const { return m_filter; }
    void setFilter(const QString& f);

    QString sortField() const { return m_sortField; }
    void setSortField(const QString& field);

    bool sortAscending() const { return m_sortAscending; }
    void setSortAscending(bool asc);

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void killProcess(int pid, int signal = 9);
    Q_INVOKABLE void setPriority(int pid, int nice);

signals:
    void processesChanged();
    void filterChanged();
    void sortFieldChanged();
    void sortAscendingChanged();

private:
    std::shared_ptr<domain::interfaces::IProcessProvider> m_provider;
    std::vector<domain::models::ProcessInfo> m_data;
    QString m_filter;
    QString m_sortField = "cpu";
    bool m_sortAscending = false;
    bool m_isBusy = false;
    QTimer* m_refreshTimer = nullptr;

    void sortData();
};

}
