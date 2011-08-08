#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "pointmodel.h"
#include "globaldefs.h"

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
    void on_treeView_doubleClicked(QModelIndex index);
    void on_treeView_customContextMenuRequested(QPoint pos);
    void on_action_del_from_list_triggered();
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
    void on_action_about_prog_triggered();
    void pointModel_dataChanged_slot(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    Ui::MainWindow *ui;
    QMenu listMenu;
    bool changed;
    QString openedFileName;
    PointModel pointModel;
    bool loadFavRecords(QString fileName, FavPointsList &list);
    void showPointList(FavPointsList &list, bool append);
    bool loadGpx(QString fileName, FavPointsList &list);
    void chCheckItems(bool checked);
    bool storeInGpx(QString &fileName);
    bool storeInFavDat(QString &fileName);
    int  countCheckedItems();
    void setChanged(bool ch);

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *, QEvent *);
};

#endif // MAINWINDOW_H
