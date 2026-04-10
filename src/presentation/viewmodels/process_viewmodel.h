#pragma once
#include <QObject>
#include <QVariantList>
#include <QTimer>
#include "../../domain/interfaces/i_process_provider.h"
#include <memory>

namespace my_comp::presentation::viewmodels {

class ProcessViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList processes READ processes NOTIFY processesChanged)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    explicit ProcessViewModel(std::shared_ptr<domain::interfaces::IProcessProvider> provider, QObject* parent = nullptr);

    QVariantList processes() const;
    
    QString filter() const { return m_filter; }
    void setFilter(const QString& f);
    
    QString sortField() const { return m_sortField; }
    void setSortField(const QString& field);
    
    bool sortAscending() const { return m_sortAscending; }
    void setSortAscending(bool asc);

    bool isBusy() const { return m_isBusy; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void killProcess(int pid, int signal = 15);
    Q_INVOKABLE void setPriority(int pid, int nice);

signals:
    void processesChanged();
    void filterChanged();
    void sortFieldChanged();
    void sortAscendingChanged();
    void isBusyChanged();

private:
    std::shared_ptr<domain::interfaces::IProcessProvider> m_provider;
    std::vector<domain::models::ProcessInfo> m_data;
    QString m_filter;
    QString m_sortField = "cpu";
    bool m_sortAscending = false;
    bool m_isBusy = false;
    QTimer* m_refreshTimer;
    
    void sortData();
};

}
