#ifndef SUBNETS_MODEL_H
#define SUBNETS_MODEL_H

#include <QAbstractTableModel>

class SubnetsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SubnetsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
};

#endif // SUBNETS_MODEL_H
