
#pragma once
#include "entity.h"

class Select : public Entity
{
    Q_OBJECT
public:
    explicit Select(QObject *parent = nullptr);

    void setOptions(const QStringList &opts);
    void setState(const QString &state);
    QString getState() const;
    QStringList getOptions() const;

protected:
    void init() override;

signals:
    void optionSelected(const QString &newOption);

private:
    void publishState();

    QString m_state;
    QStringList m_options;
};