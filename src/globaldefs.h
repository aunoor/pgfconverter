#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#include <QtCore>

#define VERSION "v1.0.7"

#define MIME_RAW_POINT_TYPE "application/x-rawpointlist"

typedef struct FavRecord {
    quint32 pntType;//home-office, 4 байта
    char head[4];
    quint32 lon; //4 байта
    quint32 lat; //4 байта
    char address[0x100];//256 байтов
    char name[0x100];//256 байтов
    char phone[0x100];//256 байтов
    quint32 iconNum; //4 байта - номер иконки
    char desc[0x100];//256 байтов
}favRecord_t;  //0x414 - длина записи.

enum PointType {ptHome=1, ptOffice=2};

typedef struct FavPoints{
   QString name;
   QString desc;
   double lat;
   double lon;
   quint32 pntType;
   quint32 iconNum;
   bool checked;
   QUuid uuid; //используется для точной идентификации точки при перемещениях
}favPoints_t;

Q_DECLARE_METATYPE(favPoints_t)

typedef QList<favPoints_t> FavPointsList;

void pntToRawPnt(favPoints_t &pnt, favRecord_t *rawPnt);
favPoints_t trRawPointToPoint(favRecord_t &favRawPoint);
void addRawPointToPointList(favRecord_t &favRawPoint, FavPointsList &list);

#endif // GLOBALDEFS_H
