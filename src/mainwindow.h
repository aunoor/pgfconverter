#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>


typedef struct FavRecord {
    //char head[8];
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
    void on_treeWidget_customContextMenuRequested(QPoint pos);
    void on_action_del_from_list_triggered();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column);
    void on_action_open_file_triggered();
    void on_action_append_from_file_triggered();
    void on_action_exit_triggered();
    void on_action_export_fav_triggered();
    void on_action_save_gpx_triggered();
    void on_action_check_all_triggered();
    void on_action_uncheck_all_triggered();
    void on_action_about_Qt_triggered();

    void on_action_save_as_triggered();

    void on_action_save_triggered();

private:
    Ui::MainWindow *ui;
    QMenu listMenu;
    bool changed;
    QString openedFileName;
    bool loadFavRecords(QString fileName, FavPointsList &list);
    void trRawPointToPoint(favRecord_t &favRawPoint, FavPointsList &list);
    void showPointList(FavPointsList &list, bool append);
    bool loadGpx(QString fileName, FavPointsList &list);
    void chCheckItems(bool checked);
    bool storeInGpx(QString &fileName);
    bool storeInFavDat(QString &fileName);
    void pntToRawPnt(favPoints_t &pnt, favRecord_t *rawPnt);
    int  countCheckedItems();
    void setChanged(bool ch);
};

#endif // MAINWINDOW_H
