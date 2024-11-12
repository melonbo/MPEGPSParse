#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QKeyEvent>
#include <QTextBrowser>
#include <file_reader.h>
#include "PSStream.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDataReady(const QList <QByteArray> &segments);
    void slot_build_tree(QByteArray& packet);
    void onTableWidgetRowSelected();

signals:
    void sig_ps_packet(QByteArray& packet);

private:
    Ui::MainWindow *ui;
    QString fileName;
    QList <QByteArray> packets;

    void init_ui();
    void init_menu();
    void init_table();
    void init_tree();
    void buildTree(QTreeWidget* tree);
    void tree_add_ps_header(QTreeWidgetItem *item_root, PSHeader& header_ps);
    void tree_add_sys_header(QTreeWidgetItem *item_root, SYSHeader& header_sys);
    void tree_add_psm_header(QTreeWidgetItem *item_root, PSMHeader& header_psm);
    void tree_add_pes_header(QTreeWidgetItem *item_root, PESHeader& header_pes);

    void showHexData(QTextBrowser* textBrowser, const QByteArray& array);
    void getPacketsInfo(const QList<QByteArray>&packets);
    QByteArray getPsHeader(const QByteArray &packet);
    void addItemToTableWidget(int offset, int len, int header_ps_len, int header_sys_len, int header_psm_len, int header_pes_len);
    void data_test();

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Escape) {
            close();
        }
    }

    void startFileReaderThread(const QString &filePath);
};

#endif // MAINWINDOW_H
