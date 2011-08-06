#ifndef POINTMODEL_H
#define POINTMODEL_H

#include <QtGui>
#include "globaldefs.h"

class PointModel : public QAbstractItemModel
{
private:
    FavPointsList pointList;
public:
    PointModel();
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags (const QModelIndex & index) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex & index = QModelIndex()) const;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
    Qt::DropActions supportedDropActions() const;
    bool dropMimeData (const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
    QStringList mimeTypes() const;
    QMimeData * mimeData ( const QModelIndexList & indexes ) const;
    bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    void appendPoint(const favPoints_t &point); //добавляет точку в список
    void clearModel(); //очищает данные модели
    void setPointChecked(int row, bool checked); //выбирает или не выбирает точку
    int getCheckedCount();//возвращает количество выбранных записей
    favPoints_t getPoint(int row); //возвращает данные точки. Если row>количества точек, все упадет. намеренно!
    int getPointsCount(); //возвращает количество точек в модели
    void setPoint(int row, favPoints_t &point); //записываем данные точки
};

#endif // POINTMODEL_H
