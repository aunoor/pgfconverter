#include <QtXml>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editpointdialog.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->treeWidget->clear();
    this->ui->treeWidget->setHeaderLabels(QStringList() << tr("Наименование") << tr("Описание") << tr("Координаты"));

    this->ui->treeWidget->header()->resizeSection(0,200);
    this->ui->treeWidget->header()->resizeSection(1,200);

    listMenu.addAction(this->ui->action_check_all);
    listMenu.addAction(this->ui->action_uncheck_all);
    listMenu.addAction(this->ui->action_del_from_list);
    listMenu.insertSeparator(this->ui->action_del_from_list);

    this->ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setWindowTitle(tr("Путевые точки"));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_exit_triggered()
{
    this->close();
}

void MainWindow::on_action_export_fav_triggered()
{
    if (!countCheckedItems()) {
        QMessageBox::critical(this,QObject::tr("Ошибка"), tr("Не выбрано ни одной точки для сохранения."));
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,tr("Экспорт favorites.dat"),".","favorites.dat; *.dat");
    if (fileName.isEmpty()) return;
    if (QFileInfo(fileName).suffix().isEmpty()) fileName.append(".dat");
    storeInFavDat(fileName);
}

void MainWindow::on_action_save_gpx_triggered()
{
    if (!countCheckedItems()) {
        QMessageBox::critical(this,QObject::tr("Ошибка"), tr("Не выбрано ни одной точки для сохранения."));
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,tr("Экспорт gpx waypoints"),".","*.gpx");
    if (fileName.isEmpty()) return;
    if (QFileInfo(fileName).suffix().isEmpty()) fileName.append(".gpx");
    storeInGpx(fileName);
}


void MainWindow::trRawPointToPoint(favRecord_t &favRawPoint, FavPointsList &list) {
    favPoints_t point;
    point.lat = favRawPoint.lat*.00001;
    point.lon = favRawPoint.lon*.00001;
    QTextCodec *codec=QTextCodec::codecForName("UTF-16");
    point.desc=codec->toUnicode(favRawPoint.desc,0x100);
    point.desc.truncate(point.desc.indexOf(QChar('\0')));
    point.name=codec->toUnicode(favRawPoint.name,0x100);
    point.name.truncate(point.name.indexOf(QChar('\0')));
    list.append(point);
}

bool MainWindow::loadFavRecords(QString fileName, FavPointsList &list)
{
    QFile file(fileName);
    if (!file.exists()) {
        QMessageBox::critical(this,QObject::tr("Ошибка"), tr("Файл не найден."));
        return false;
    }

    bool res = file.open(QIODevice::ReadOnly);

    if (!res) {
        QMessageBox::critical(this,QObject::tr("Ошибка"), tr("Ошибка при открытии файла."));
        return false;
    }

    list.clear();
    file.seek(6); //пропускаем заголовок
    while (!file.atEnd()) {
        QByteArray ba = file.read(0x414);
        if (ba.size()!=0x414) {
            QMessageBox::critical(this,QObject::tr("Ошибка"), tr("Size of readed data != point data size!"));
            file.close();
            return false;
        }
        favRecord_t favRawPoint;
        qMemCopy(&favRawPoint,ba.data(),0x414);
        trRawPointToPoint(favRawPoint, list);
    }
    file.close();
    return true;
}

void MainWindow::showPointList(FavPointsList &list, bool append) {
    if (!append) this->ui->treeWidget->clear();
    for (int i=0;i<list.size();i++) {
        QStringList sList;
        sList << list.at(i).name;
        sList << list.at(i).desc;
        sList << QString(tr("N %2, E %1")).arg(QString::number(list.at(i).lon,'g',7).leftJustified(8,'0',true)).arg(QString::number(list.at(i).lat,'g',7).leftJustified(8,'0',true));
        QTreeWidgetItem *item = new QTreeWidgetItem(sList);
        item->setCheckState(0,Qt::Checked);
        item->setData(0,Qt::UserRole,qVariantFromValue(list.at(i)));
        ui->treeWidget->addTopLevelItem(item);
    }
}

bool MainWindow::loadGpx(QString fileName, FavPointsList &list) {
    QDomDocument doc("gpx");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    list.clear();
    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (e.tagName().toLower()=="wpt") {
                favPoints_t point;
                point.lon = e.attributeNode("lon").value().toDouble();
                point.lat = e.attributeNode("lat").value().toDouble();
                QDomNodeList nl=e.childNodes();
                for (int i=0;i<nl.size();i++) {
                    if (nl.at(i).nodeName().toLower()=="name") point.name=nl.at(i).toElement().text();
                    if (nl.at(i).nodeName().toLower()=="desc") point.desc=nl.at(i).toElement().text();
                }//for nl.size
                list.append(point);
            }//if =="wpt"
        }//!e.isNull()
        n = n.nextSibling();
    }//while(!n.isNull())
    return true;
}

void MainWindow::chCheckItems(bool checked) {
    int cnt=this->ui->treeWidget->topLevelItemCount();
    for (int i=0; i<cnt;i++) {
        ui->treeWidget->topLevelItem(i)->setCheckState(0,checked?Qt::Checked:Qt::Unchecked);
    }
}

void MainWindow::on_action_check_all_triggered()
{
    chCheckItems(true);
}

void MainWindow::on_action_uncheck_all_triggered()
{
    chCheckItems(false);
}

int MainWindow::countCheckedItems() {
    int cnt=0;
    for (int i=0; i<this->ui->treeWidget->topLevelItemCount();i++) {
        if (ui->treeWidget->topLevelItem(i)->checkState(0)==Qt::Checked) cnt++;
    }
    return cnt;
}

bool MainWindow::storeInGpx(QString &fileName){
    QFile file(fileName);
    bool res = file.open(QIODevice::WriteOnly);
    if (!res) {
        QMessageBox::critical(this,QObject::tr("Ошибка"), tr("Ошибка при открытии файла %1.").arg(fileName));
        return res;
    }

    file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    file.write("<gpx version=\"1.0\" >\n");
    int cnt=this->ui->treeWidget->topLevelItemCount();
    for (int i=0; i<cnt;i++) {
        if (ui->treeWidget->topLevelItem(i)->checkState(0)!=Qt::Checked) continue;

        favPoints_t point = ui->treeWidget->topLevelItem(i)->data(0,Qt::UserRole).value<favPoints_t>();
        file.write("<wpt lat=\"");
        file.write(QString::number(point.lat,'g',10).toLatin1());
        file.write("\" lon=\"");
        file.write(QString::number(point.lon,'g',10).toLatin1());
        file.write("\">\n");
        if (!point.name.isEmpty()){
            file.write("\t<name>");
            file.write(point.name.toUtf8());
            file.write("</name>\n");
        }
        if (!point.desc.isEmpty()){
            file.write("\t<desc>");
            file.write(point.desc.toUtf8());
            file.write("</desc>\n");
        }
        file.write("</wpt>\n");
    }//for
    file.write("</gpx>\n");
    return true;
}

void MainWindow::pntToRawPnt(favPoints_t &pnt, favRecord_t &rawPnt) {
    qMemSet((void*)&rawPnt,0,sizeof(rawPnt));
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    rawPnt.lat=pnt.lat*100000;
    rawPnt.lon=pnt.lon*100000;
    QByteArray ba = codec->fromUnicode(pnt.name);
    qMemCopy(&rawPnt.name[0], ba.data()+2, pnt.name.size()*2);
    ba =  codec->fromUnicode(pnt.desc);
    qMemCopy(&rawPnt.desc[0], ba.data()+2, pnt.desc.size()*2);
}

bool MainWindow::storeInFavDat(QString &fileName){
    QFile file(fileName);
    bool res = file.open(QIODevice::WriteOnly);
    if (!res) {
        QMessageBox::critical(this,QObject::tr("Ошибка"), tr("Ошибка при открытии файла %1.").arg(fileName));
        return res;
    }//if !res

    quint32 cnt=countCheckedItems();

    //пишем заголовок
    char a[2]={1,0};
    file.write(&a[0],2);
    file.write((char*)&cnt,4);
    file.seek(6);
    //пишем данные
    for (int i=0; i<this->ui->treeWidget->topLevelItemCount();i++) {
        if (ui->treeWidget->topLevelItem(i)->checkState(0)!=Qt::Checked) continue;
        cnt++;
        favPoints_t point = ui->treeWidget->topLevelItem(i)->data(0,Qt::UserRole).value<favPoints_t>();
        favRecord_t rawPnt;
        pntToRawPnt(point,rawPnt);
        file.write((char*)&rawPnt,sizeof(rawPnt));
    }//for
    file.close();
    return true;
}

void MainWindow::on_action_append_from_file_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Добавить точки в список"),".",tr("Файлы с путевыми точками (*.gpx; favorites.dat; *.dat)"));
    if (fileName.isEmpty()) return;
    FavPointsList favList;
    if (QFileInfo(fileName).suffix()=="gpx") {if (!loadGpx(fileName, favList)) return;}
    else {if (!loadFavRecords(fileName, favList)) return; }
    showPointList(favList, true);
}

void MainWindow::on_action_open_file_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Открыть список точек"),".",tr("Файлы с путевыми точками (*.gpx; favorites.dat; *.dat)"));
    if (fileName.isEmpty()) return;
    FavPointsList favList;
    if (QFileInfo(fileName).suffix()=="gpx") {if (!loadGpx(fileName, favList)) return;}
    else {if (!loadFavRecords(fileName, favList)) return; }
    showPointList(favList, false);
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    EditPointDialog ed;
    QString name, desc, coords;
    name = item->text(0);
    desc = item->text(1);
    coords = item->text(2);
    int res=ed.exec(name, desc, coords);
    if (res==QDialog::Rejected) return;
    favPoints_t point = item->data(0,Qt::UserRole).value<favPoints_t>();
    point.desc = desc;
    point.name = name;
    item->setData(0,Qt::UserRole,qVariantFromValue(point));
    item->setText(0,name);
    item->setText(1,desc);
}

void MainWindow::on_action_del_from_list_triggered()
{
    if (!ui->treeWidget->currentItem()) return;
    ui->treeWidget->model()->removeRow(ui->treeWidget->currentIndex().row());
}

void MainWindow::on_treeWidget_customContextMenuRequested(QPoint pos)
{
    ui->action_del_from_list->setEnabled(ui->treeWidget->itemAt(pos));
    QPoint locPos = mapToGlobal(pos);
    locPos.setY(locPos.y()+listMenu.sizeHint().height()-20);
    listMenu.popup(locPos,ui->action_check_all);
}
