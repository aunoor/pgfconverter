#include <QtCore>

#define VERSION "0.0.1"
//0x414 - длина записи о точке.
//offset 0x02 - количество записей в файле, длина - 4 байта (unsigned long)

typedef struct FavPoints{
    QString name;
    QString desc;
    QString address;
    double lat;
    double lon;
}favPoints_t;

typedef struct PointRec{
    char head[8];
    quint32 lon; //4 байта
    quint32 lat; //4 байта
    char address[0x100];//256 байтов
    char name[0x100];//256 байтов
    char phone[0x100];//256 байтов
    quint32 iconNum; //4 байта - номер иконки
    char desc[0x100];//256 байтов
} pointRec_t; //общий размер 1044 байта

typedef QList<favPoints_t*> WayPointsList;

void dumpCH(char *ca, int size){
    for (int i=0;i<size;i++) {
        fprintf(stderr,"%#x ",*(ca+i));
    }
    fprintf(stderr,"\r\n");
}

favPoints_t* parseRecord(pointRec_t* pointRec)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF16");;
    favPoints_t* point = new favPoints_t;
    point->lat=(pointRec->lat *.00001);
    point->lon=(pointRec->lon *.00001);
    point->name=codec->toUnicode(pointRec->name,0x100);
    point->name.truncate(point->name.indexOf(QChar('\0'),0,Qt::CaseInsensitive));
    point->desc=codec->toUnicode(pointRec->desc,0x100);
    point->desc.truncate(point->desc.indexOf(QChar('\0'),0,Qt::CaseInsensitive));
    point->address=codec->toUnicode(pointRec->address,0x100).trimmed();
    point->address.truncate(point->address.indexOf(QChar('\0'),0,Qt::CaseInsensitive));
    return point;
}


bool storeInGpx(QString &fileName, WayPointsList &list){
    QFile file(fileName);
    bool res = file.open(QIODevice::WriteOnly);
    if (!res) {
        printf("Can't open %s for reading.\n",fileName.toLocal8Bit().data());
        return res;
    }
    file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    file.write("<gpx version=\"1.0\" >\n");
    for (int i=0;i<list.size();i++) {
        file.write("<wpt lat=\"");
        file.write(QString::number(list.at(i)->lat,'g',10).toLatin1());
        file.write("\" lon=\"");
        file.write(QString::number(list.at(i)->lon,'g',10).toLatin1());
        file.write("\">\n");
        if (!list.at(i)->name.isEmpty()){
            file.write("\t<name>");
            file.write(list.at(i)->name.toUtf8());
            file.write("</name>\n");
        }
        if (!list.at(i)->desc.isEmpty()){
            file.write("\t<desc>");
            file.write(list.at(i)->desc.toUtf8());
            file.write("</desc>\n");
        }
        file.write("</wpt>\n");
    }
    file.write("</gpx>\n");
return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString outFileName;

    printf("ProGorod to gpx converter v.%s\n",VERSION);

    if (a.arguments().indexOf("-h")>0) {
        printf("Usage:\n");
        printf("-o <filename> - output gpx file name\n");
        return 0;
    }

    QFile file("./favorites.dat");
    if (!file.exists()) {
        printf("favorites.dat not found.\n");
        return -1;
    }
    bool res = file.open(QIODevice::ReadOnly);

    if (!res) {
        printf("Can't open favorites.dat for reading.\n");
        return -1;
    }

    int idx=a.arguments().indexOf("-o");
    if (idx>0) outFileName = a.arguments().at(idx+1);
    if (outFileName.isEmpty()) outFileName="./waypoints.gpx";

    file.seek(6); //пропускаем заголовок

    WayPointsList wpts;

    while (!file.atEnd()) {
        QByteArray ba = file.read(0x414);
        favPoints_t* point = parseRecord((pointRec_t*)ba.data());
        wpts.append(point);
    }//while !file.atEnd()

    storeInGpx(outFileName,wpts);

    //return a.exec();
}

