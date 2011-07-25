#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>


typedef struct FavRecord {
    char head[8];
    quint32 lon; //4 байта
    quint32 lat; //4 байта
    char address[0x100];//256 байтов
    char name[0x100];//256 байтов
    char phone[0x100];//256 байтов
    quint32 iconNum; //4 байта - номер иконки
    char desc[0x100];//256 байтов
}favRecord_t;  //0x414 - длина записи.

typedef struct FavPoints{
   QString name;
   QString desc;
   double lat;
   double lon;
}favPoints_t;

Q_DECLARE_METATYPE(favPoints_t)

typedef QList<favPoints_t> FavPointsList;


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_exit_triggered();

    void on_action_open_gpx_triggered();

    void on_action_import_fav_triggered();

    void on_action_export_fav_triggered();

    void on_action_save_gpx_triggered();

    void on_action_check_all_triggered();

    void on_action_uncheck_all_triggered();

private:
    Ui::MainWindow *ui;
    FavPointsList favList;
    QMenu treeDropMenu;
    bool loadFavRecords(QString fileName, FavPointsList &list);
    void trRawPointToPoint(favRecord_t &favRawPoint, FavPointsList &list);
    void showPointList(FavPointsList &list);
    bool loadGpx(QString fileName, FavPointsList &list);
    void chCheckItems(bool checked);
    bool storeInGpx(QString &fileName);
    bool storeInFavDat(QString &fileName);
    void pntToRawPnt(favPoints_t &pnt, favRecord_t &rawPnt);
    int countCheckedItems();
};

#endif // MAINWINDOW_H
