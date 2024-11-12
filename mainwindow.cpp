#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidget>

QString hexToQString(int number, int width) {
    return QString::number(number, 16).toUpper().rightJustified(width, '0');
}

std::vector <uint8_t> packet = {0x00, 0x00, 0x01, 0xba, 0x44, 0x00, 0x04, 0x00, 0x04, 0x01, 0x00, 0x3a, 0x9b, 0xf8, 0x00, 0x00, 0x01, 0xbb, 0x00, 0x0c, 0x80, 0x1e, 0xff, 0xfd, 0xe1, 0x7f, 0xe0, 0xe8, 0x00, 0xc0, 0xc2, 0x00, 0x00, 0x00, 0x01, 0xbc, 0x00, 0x12, 0xe1, 0xff, 0x00, 0x00, 0x00, 0x08, 0x1b, 0xe0, 0x00, 0x00, 0x90, 0xc0, 0x00, 0x00, 0x45, 0xbd, 0xdc, 0xf4, 0x00, 0x00, 0x01, 0xe0, 0x2e, 0x3c, 0x80, 0xc0, 0x0a, 0x31, 0x00, 0x01, 0x00, 0x01, 0x11, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x67, 0x4d, 0x00, 0x1f, 0x9d, 0xa8, 0x14, 0x01, 0x6e, 0x9b, 0x80, 0x80, 0x80, 0xa0, 0x00, 0x00, 0x03, 0x00, 0x20, 0x00, 0x00, 0x06, 0x50, 0x80, 0x00, 0x00, 0x00, 0x01, 0x68, 0xee, 0x3c, 0x80, 0x00, 0x00, 0x00, 0x01, 0x65, 0xb8, 0x00, 0x00, 0x03, 0x03, 0x83, 0x30, 0x3f, 0xeb, 0x08, 0x6a, 0xea, 0xff, 0x03, 0xcd, 0xe0, 0x7d, 0xda, 0xe6, 0x1d, 0x50, 0xcd, 0x67, 0x46, 0x6b, 0x29, 0x2f, 0xd0, 0x39, 0x59, 0x14, 0xbf, 0x47, 0x1c};
void MainWindow::data_test()
{
    QByteArray byteArray(reinterpret_cast<const char*>(packet.data()), packet.size());
    emit sig_ps_packet(byteArray);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(sig_ps_packet(QByteArray&)), this, SLOT(slot_build_tree(QByteArray&)));
    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::onTableWidgetRowSelected);

    init_ui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_ui()
{
    this->setWindowTitle("MPEGPSParse");
    ui->centralWidget->setLayout(ui->horizontalLayout);
    init_menu();
    init_table();
    init_tree();

    this->resize(1200, 800);
    startFileReaderThread("F:\\share\\out-01.ps");
    //data_test();
}

void MainWindow::init_menu()
{
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu("File");

    QAction *openAction = new QAction(QIcon(":/icons/open.png"), "Open", this);
    fileMenu->addAction(openAction);

    QAction *closeAction = new QAction(QIcon(":/icons/close.png"), "Close", this);
    fileMenu->addAction(closeAction);

    QAction *exitAction = new QAction(QIcon(":/icons/quit.png"), "Exit", this);
    QObject::connect(exitAction, &QAction::triggered, this, &QApplication::quit);
    fileMenu->addAction(exitAction);

    QMenu* operationMenu = menuBar->addMenu("Operations");

    QAction* toPsAction = new QAction("To PS", this);
    operationMenu->addAction(toPsAction);

    QAction* toPesAction = new QAction("To PES", this);
    operationMenu->addAction(toPesAction);

    // 连接 "Open" 菜单项的点击信号到槽
    QObject::connect(openAction, &QAction::triggered, [&]() {
        QString file = QFileDialog::getOpenFileName(this, "Select File");
        if (!file.isEmpty()) {
            //QMessageBox::information(this, "Selected File", file);
            fileName = file;
            this->setWindowTitle(QString("MPEGPSParse - ")+ QString("(%1)").arg(fileName));
            startFileReaderThread(fileName);
        }
    });
}

void MainWindow::init_table()
{
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setColumnWidth(0, 120);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(2, 100);
    ui->tableWidget->setColumnWidth(3, 100);
    ui->tableWidget->setColumnWidth(4, 100);
    ui->tableWidget->setColumnWidth(5, 100);

    // 获取列宽度的总和，设置表格总宽度
    int totalWidth = 0;
    for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        totalWidth += ui->tableWidget->columnWidth(i);
    }
    ui->tableWidget->setFixedWidth(totalWidth+30);
    ui->textBrowser->setFixedWidth(totalWidth+30);
    //ui->tableWidget->resizeColumnToContents(5);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::init_tree()
{
    ui->treeWidget->setHeaderHidden(true);
    //buildTree(ui->treeWidget);
}

void MainWindow::buildTree(QTreeWidget* treeWidget)
{
    // 清空树
    treeWidget->clear();

    // 创建根节点
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(treeWidget);
    rootItem->setText(0, "Root Node");

    // 创建子节点
    QTreeWidgetItem *childItem1 = new QTreeWidgetItem(rootItem);
    childItem1->setText(0, "Child Node 1");

    QTreeWidgetItem *childItem2 = new QTreeWidgetItem(rootItem);
    childItem2->setText(0, "Child Node 2");

    // 创建子节点的子节点
    QTreeWidgetItem *grandChildItem = new QTreeWidgetItem(childItem1);
    grandChildItem->setText(0, "Grandchild Node 1");

    // 展开根节点
    treeWidget->expandAll();
}

void MainWindow::showHexData(QTextBrowser* textBrowser, const QByteArray& array)
{
    textBrowser->clear();
    QString header;
    QString hex;
    QString show_text;
    int size = array.size();
    if(size>200) size=200;

    for(int i=0; i<=size/16; i++)
    {
        show_text += "$";
        show_text += QString::number(i, 16).rightJustified(8, '0') + " ";
        for(int j=0; j<16; j++)
        {
            show_text += QString::asprintf("%02X", (unsigned char)array[i*16+j])+QString(" ");
        }
        show_text += "\n";
    }
    textBrowser->setPlainText(show_text);
}

void MainWindow::startFileReaderThread(const QString &filePath) {
    FileReaderThread *fileReader = new FileReaderThread(filePath);
    connect(fileReader, &FileReaderThread::dataReady, this, &MainWindow::onDataReady);
    fileReader->start();
}

void MainWindow::onDataReady(const QList <QByteArray> &segments) {
    packets = segments;
    getPacketsInfo(packets);
}

void MainWindow::getPacketsInfo(const QList<QByteArray>&packets)
{
    int offset = 0;
    int num = packets.size();
    for(int i=0; i<num; i++)
    {
        const QByteArray & packet = packets.at(i);
        int len = packet.size();

        PSHeader header_ps;
        SYSHeader header_sys;
        PSMHeader header_psm;
        PESHeader header_pes;

        PSStream psStream(packet);

        header_ps = psStream.readPSHeader();
        if(psStream.ps_header_len)
            header_ps.print();

        if(psStream.ps_header_len){
            header_sys = psStream.readSYSHeader();
            if(psStream.sys_header_len)
                header_sys.print();
        }

        if(psStream.sys_header_len){
            header_psm = psStream.readPSMHeader();
            if(psStream.psm_header_len)
                header_psm.print();
        }

        if(psStream.ps_header_len){
            header_pes = psStream.readPESHeader();
            if(psStream.pes_header_len)
                header_pes.print();
        }

        addItemToTableWidget(offset, len, psStream.ps_header_len, psStream.sys_header_len, psStream.psm_header_len, psStream.pes_header_len);
        //printf("%d %d %d %d %d\n", offset, len, header_len, pcr, mux_rate);
        offset += packet.size();
    }
}

void MainWindow::addItemToTableWidget(int offset, int len, int header_ps_len, int header_sys_len, int header_psm_len, int header_pes_len)
{
    int rowCount = ui->tableWidget->rowCount(); // 获取当前行数
    ui->tableWidget->insertRow(rowCount); // 在最后一行插入新行
    ui->tableWidget->setRowHeight(rowCount, 30);
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString("$%1").arg(offset, 8, 16, QChar('0'))));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(len)));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(header_ps_len)));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(header_sys_len)));
    ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(header_psm_len)));
    ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(header_pes_len)));
    if(rowCount==0)
        ui->tableWidget->selectRow(rowCount);
}

void MainWindow::slot_build_tree(QByteArray& packet)
{
    printf("==================== \n");
    PSHeader header_ps;
    SYSHeader header_sys;
    PSMHeader header_psm;
    PESHeader header_pes;

    PSStream psStream(packet);

    header_ps = psStream.readPSHeader();
    if(psStream.ps_header_len)
        header_ps.print();

    if(psStream.ps_header_len){
        header_sys = psStream.readSYSHeader();
        if(psStream.sys_header_len)
            header_sys.print();
    }

    if(psStream.sys_header_len){
        header_psm = psStream.readPSMHeader();
        if(psStream.psm_header_len)
            header_psm.print();
    }

    if(psStream.ps_header_len){
        header_pes = psStream.readPESHeader();
        if(psStream.pes_header_len)
            header_pes.print();
    }

    printf("header ps len %d\n", psStream.ps_header_len);
    printf("header sys len %d\n", psStream.sys_header_len);
    printf("header psm len %d\n", psStream.psm_header_len);
    printf("header pes len %d\n", psStream.pes_header_len);
    printf("header len all %d\n", psStream.ps_header_len+psStream.sys_header_len+psStream.psm_header_len+psStream.pes_header_len);

    ui->treeWidget->clear();
    // 创建根节点
    QTreeWidgetItem *item_root = new QTreeWidgetItem(ui->treeWidget);
    item_root->setText(0, "Pack");

    //创建PSHeader
    if(psStream.ps_header_len)
    tree_add_ps_header(item_root, header_ps);

    //创建SYSHeader
    if(psStream.sys_header_len)
    tree_add_sys_header(item_root, header_sys);

    //创建PSMHeader
    if(psStream.psm_header_len)
    tree_add_psm_header(item_root, header_psm);

    //创建PESHeader
    if(psStream.ps_header_len)
    tree_add_pes_header(item_root, header_pes);

    ui->treeWidget->expandAll();
}

void MainWindow::tree_add_ps_header(QTreeWidgetItem *item_root, PSHeader &header_ps)
{
    QTreeWidgetItem *item_ps_header = new QTreeWidgetItem(item_root);
    item_ps_header->setText(0, "ps header");

    QTreeWidgetItem *item_ps_pack_start_code = new QTreeWidgetItem(item_ps_header);
    QString str_ps_pack_start_code = QString("pcak_start_code=$")+hexToQString(header_ps.pack_start_code, 8)+" (32bits)";
    item_ps_pack_start_code->setText(0, str_ps_pack_start_code);

    QTreeWidgetItem *item_reserved_01 = new QTreeWidgetItem(item_ps_header);
    QString str_reserved_01 = QString("reserved_01=")+QString::number(header_ps.reserved_01)+" (2bits)";
    item_reserved_01->setText(0, str_reserved_01);

    QTreeWidgetItem *item_system_clock_reference_base_32_30 = new QTreeWidgetItem(item_ps_header);
    QString str_system_clock_reference_base_32_30 = QString("system_clock_reference_base_32_30=")+QString::number(header_ps.system_clock_reference_base_32_30)+" (3bits)";
    item_system_clock_reference_base_32_30->setText(0, str_system_clock_reference_base_32_30);

    QTreeWidgetItem *item_marker_bit_01 = new QTreeWidgetItem(item_ps_header);
    QString str_marker_bit_01 = QString("marker_bit=")+QString::number(header_ps.marker_bit_01)+" (1bits)";
    item_marker_bit_01->setText(0, str_marker_bit_01);

    QTreeWidgetItem *item_system_clock_reference_base_29_15 = new QTreeWidgetItem(item_ps_header);
    QString str_system_clock_reference_base_29_15 = QString("system_clock_reference_base_29_15=")+QString::number(header_ps.system_clock_reference_base_29_15)+" (15bits)";
    item_system_clock_reference_base_29_15->setText(0, str_system_clock_reference_base_29_15);

    QTreeWidgetItem *item_marker_bit_02 = new QTreeWidgetItem(item_ps_header);
    QString str_marker_bit_02 = QString("marker_bit=")+QString::number(header_ps.marker_bit_02)+" (1bits)";
    item_marker_bit_02->setText(0, str_marker_bit_02);

    QTreeWidgetItem *item_system_clock_reference_base_14_0 = new QTreeWidgetItem(item_ps_header);
    QString str_system_clock_reference_base_14_0 = QString("system_clock_reference_base_14_0=")+QString::number(header_ps.system_clock_reference_base_14_0)+" (15bits)";
    item_system_clock_reference_base_14_0->setText(0, str_system_clock_reference_base_14_0);

    QTreeWidgetItem *item_marker_bit_03 = new QTreeWidgetItem(item_ps_header);
    QString str_marker_bit_03 = QString("marker_bit=")+QString::number(header_ps.marker_bit_03)+" (1bits)";
    item_marker_bit_03->setText(0, str_marker_bit_03);

    QTreeWidgetItem *item_system_clock_reference_extension = new QTreeWidgetItem(item_ps_header);
    QString str_system_clock_reference_extension = QString("system_clock_reference_extension=")+QString::number(header_ps.system_clock_reference_extension)+" (2bits)";
    item_system_clock_reference_extension->setText(0, str_system_clock_reference_extension);

    QTreeWidgetItem *item_marker_bit_04 = new QTreeWidgetItem(item_ps_header);
    QString str_marker_bit_04 = QString("marker_bit=")+QString::number(header_ps.marker_bit_04)+" (1bits)";
    item_marker_bit_04->setText(0, str_marker_bit_04);

    QTreeWidgetItem *item_program_mux_rate = new QTreeWidgetItem(item_ps_header);
    QString str_program_mux_rate = QString("program_mux_rate=")+QString::number(header_ps.program_mux_rate)+" (2bits)";
    item_program_mux_rate->setText(0, str_program_mux_rate);

    QTreeWidgetItem *item_marker_bit_05 = new QTreeWidgetItem(item_ps_header);
    QString str_marker_bit_05 = QString("marker_bit=")+QString::number(header_ps.marker_bit_05)+" (1bits)";
    item_marker_bit_05->setText(0, str_marker_bit_05);

    QTreeWidgetItem *item_marker_bit_06 = new QTreeWidgetItem(item_ps_header);
    QString str_marker_bit_06 = QString("marker_bit=")+QString::number(header_ps.marker_bit_06)+" (1bits)";
    item_marker_bit_06->setText(0, str_marker_bit_06);

    QTreeWidgetItem *item_reserved_02 = new QTreeWidgetItem(item_ps_header);
    QString str_reserved_02 = QString("reserved_02=")+QString::number(header_ps.reserved_02)+" (2bits)";
    item_reserved_02->setText(0, str_reserved_02);

    QTreeWidgetItem *item_pack_stuffing_length = new QTreeWidgetItem(item_ps_header);
    QString str_pack_stuffing_length = QString("pack_stuffing_length=")+QString::number(header_ps.pack_stuffing_length)+" (2bits)";
    item_pack_stuffing_length->setText(0, str_pack_stuffing_length);
}

void MainWindow::tree_add_sys_header(QTreeWidgetItem *item_root, SYSHeader& header_sys)
{
    QTreeWidgetItem *item_sys_header = new QTreeWidgetItem(item_root);
    item_sys_header->setText(0, "sys header");

    QTreeWidgetItem *item_sys_pack_start_code = new QTreeWidgetItem(item_sys_header);
    QString str_sys_pack_start_code = QString("system_header_start_code=$")+hexToQString(header_sys.system_header_start_code, 8)+" (32bits)";
    item_sys_pack_start_code->setText(0, str_sys_pack_start_code);

    QTreeWidgetItem *item_header_length = new QTreeWidgetItem(item_sys_header);
    QString str_header_length = QString("header_length=")+QString::number(header_sys.header_length)+" (16bits)";
    item_header_length->setText(0, str_header_length);

    QTreeWidgetItem *item_marker_bit_01 = new QTreeWidgetItem(item_sys_header);
    QString str_marker_bit_01 = QString("marker_bit_01=")+QString::number(header_sys.marker_bit_01)+" (1bits)";
    item_marker_bit_01->setText(0, str_marker_bit_01);

    QTreeWidgetItem *item_rate_bound = new QTreeWidgetItem(item_sys_header);
    QString str_rate_bound = QString("rate_bound=")+QString::number(header_sys.rate_bound)+" (22bits)";
    item_rate_bound->setText(0, str_rate_bound);

    QTreeWidgetItem *item_marker_bit_02 = new QTreeWidgetItem(item_sys_header);
    QString str_marker_bit_02 = QString("marker_bit=")+QString::number(header_sys.marker_bit_02)+" (1bits)";
    item_marker_bit_02->setText(0, str_marker_bit_02);

    QTreeWidgetItem *item_audio_bound = new QTreeWidgetItem(item_sys_header);
    QString str_audio_bound = QString("audio_bound=")+QString::number(header_sys.audio_bound)+" (6bits)";
    item_audio_bound->setText(0, str_audio_bound);

    QTreeWidgetItem *item_fixed_flag = new QTreeWidgetItem(item_sys_header);
    QString str_fixed_flag = QString("fixed_flag=")+QString::number(header_sys.fixed_flag)+" (1bits)";
    item_fixed_flag->setText(0, str_fixed_flag);

    QTreeWidgetItem *item_CSPS_flag = new QTreeWidgetItem(item_sys_header);
    QString str_CSPS_flag = QString("CSPS_flag=")+QString::number(header_sys.CSPS_flag)+" (1bits)";
    item_CSPS_flag->setText(0, str_CSPS_flag);

    QTreeWidgetItem *item_system_audio_lock_flag = new QTreeWidgetItem(item_sys_header);
    QString str_system_audio_lock_flag = QString("system_audio_lock_flag=")+QString::number(header_sys.system_audio_lock_flag)+" (1bits)";
    item_system_audio_lock_flag->setText(0, str_system_audio_lock_flag);

    QTreeWidgetItem *item_system_video_lock_flag = new QTreeWidgetItem(item_sys_header);
    QString str_system_video_lock_flag = QString("system_video_lock_flag=")+QString::number(header_sys.system_video_lock_flag)+" (1bits)";
    item_system_video_lock_flag->setText(0, str_system_video_lock_flag);

    QTreeWidgetItem *item_marker_bit_03 = new QTreeWidgetItem(item_sys_header);
    QString str_marker_bit_03 = QString("marker_bit=")+QString::number(header_sys.marker_bit_03)+" (1bits)";
    item_marker_bit_03->setText(0, str_marker_bit_03);

    QTreeWidgetItem *item_video_bound = new QTreeWidgetItem(item_sys_header);
    QString str_video_bound = QString("video_bound=")+QString::number(header_sys.video_bound)+" (5bits)";
    item_video_bound->setText(0, str_video_bound);

    QTreeWidgetItem *item_packet_rate_restriction_flag = new QTreeWidgetItem(item_sys_header);
    QString str_packet_rate_restriction_flag = QString("packet_rate_restriction_flag=")+QString::number(header_sys.packet_rate_restriction_flag)+" (1bits)";
    item_packet_rate_restriction_flag->setText(0, str_packet_rate_restriction_flag);

    QTreeWidgetItem *item_reserved_bits = new QTreeWidgetItem(item_sys_header);
    QString str_reserved_bits = QString("reserved_bits=")+QString::number(header_sys.reserved_bits)+" (7bits)";
    item_reserved_bits->setText(0, str_reserved_bits);

    QTreeWidgetItem *item_PSTD_buffer_params_count = new QTreeWidgetItem(item_sys_header);
    QString str_PSTD_buffer_params_count = QString("PSTD_buffer_params_count=")+QString::number(header_sys.streams.size())+" (1bits)";
    item_PSTD_buffer_params_count->setText(0, str_PSTD_buffer_params_count);

    QTreeWidgetItem *item_PSTD_buffer_params = new QTreeWidgetItem(item_sys_header);
    QString str_PSTD_buffer_params = QString("PSTD_buffer_params");
    item_PSTD_buffer_params->setText(0, str_PSTD_buffer_params);

    for(int i=0; i<header_sys.streams.size(); i++)
    {
        QTreeWidgetItem *item_PSTD_buffer_params_index = new QTreeWidgetItem(item_PSTD_buffer_params);
        QString str_PSTD_buffer_params_index = QString::number(i);
        item_PSTD_buffer_params_index->setText(0, str_PSTD_buffer_params_index);

        QTreeWidgetItem *item_PSTD_buffer_params_index_stream_id = new QTreeWidgetItem(item_PSTD_buffer_params_index);
        QString str_PSTD_buffer_params_index_stream_id = QString("stream_id=$")+hexToQString(header_sys.streams[i].stream_id, 2)+" (8bits)";
        item_PSTD_buffer_params_index_stream_id->setText(0, str_PSTD_buffer_params_index_stream_id);

        QTreeWidgetItem *item_PSTD_buffer_params_index_reserved_bits = new QTreeWidgetItem(item_PSTD_buffer_params_index);
        QString str_PSTD_buffer_params_index_reserved_bits = QString("reserved_bits=")+QString::number(header_sys.streams[i].reserved)+" (2bits)";
        item_PSTD_buffer_params_index_reserved_bits->setText(0, str_PSTD_buffer_params_index_reserved_bits);

        QTreeWidgetItem *item_PSTD_buffer_params_index_P_STD_buffer_bound_scale = new QTreeWidgetItem(item_PSTD_buffer_params_index);
        QString str_PSTD_buffer_params_index_P_STD_buffer_bound_scale = QString("P_STD_buffer_bound_scale=")+QString::number(header_sys.streams[i].P_STD_buffer_bound_scale)+" (1bits)";
        item_PSTD_buffer_params_index_P_STD_buffer_bound_scale->setText(0, str_PSTD_buffer_params_index_P_STD_buffer_bound_scale);

        QTreeWidgetItem *item_PSTD_buffer_params_index_P_STD_buffer_size_bound = new QTreeWidgetItem(item_PSTD_buffer_params_index);
        QString str_PSTD_buffer_params_index_P_STD_buffer_size_bound = QString("P_STD_buffer_size_bound=")+QString::number(header_sys.streams[i].P_STD_buffer_size_bound)+" (13bits)";
        item_PSTD_buffer_params_index_P_STD_buffer_size_bound->setText(0, str_PSTD_buffer_params_index_P_STD_buffer_size_bound);
    }
}

void MainWindow::tree_add_psm_header(QTreeWidgetItem *item_root, PSMHeader& header_psm)
{
    QTreeWidgetItem *item_psm_header = new QTreeWidgetItem(item_root);
    item_psm_header->setText(0, "psm header");

    QTreeWidgetItem *item_psm_pack_start_code = new QTreeWidgetItem(item_psm_header);
    QString str_psm_pack_start_code = QString("pcaket_start_code_prefix=$")+hexToQString(header_psm.packet_start_code_prefix, 6)+" (24bits)";
    item_psm_pack_start_code->setText(0, str_psm_pack_start_code);

    QTreeWidgetItem *item_psm_map_stream_id = new QTreeWidgetItem(item_psm_header);
    QString str_psm_map_stream_id = QString("map_stream_id=$")+hexToQString(header_psm.map_stream_id, 2)+" (8bits)";
    item_psm_map_stream_id->setText(0, str_psm_map_stream_id);

    QTreeWidgetItem *item_psm_program_stream_map_length = new QTreeWidgetItem(item_psm_header);
    QString str_psm_program_stream_map_length = QString("program_stream_map_length=")+QString::number(header_psm.elementary_stream_map_length)+" (16bits)";
    item_psm_program_stream_map_length->setText(0, str_psm_program_stream_map_length);

    QTreeWidgetItem *item_psm_current_next_indicator = new QTreeWidgetItem(item_psm_header);
    QString str_psm_current_next_indicator = QString("current_next_indicator=")+QString::number(header_psm.current_next_indicator)+" (1bits)";
    item_psm_current_next_indicator->setText(0, str_psm_current_next_indicator);

    QTreeWidgetItem *item_psm_reserved_01 = new QTreeWidgetItem(item_psm_header);
    QString str_psm_reserved_01 = QString("reserved=")+QString::number(header_psm.reserved_01)+" (2bits)";
    item_psm_reserved_01->setText(0, str_psm_reserved_01);

    QTreeWidgetItem *item_psm_program_stream_map_version = new QTreeWidgetItem(item_psm_header);
    QString str_psm_program_stream_map_version = QString("program_stream_map_version=")+QString::number(header_psm.program_stream_map_version)+" (5bits)";
    item_psm_program_stream_map_version->setText(0, str_psm_program_stream_map_version);

    QTreeWidgetItem *item_psm_reserved_02 = new QTreeWidgetItem(item_psm_header);
    QString str_psm_reserved_02 = QString("reserved=")+QString::number(header_psm.reserved_02)+" (7bits)";
    item_psm_reserved_02->setText(0, str_psm_reserved_02);

    QTreeWidgetItem *item_psm_marker_bit = new QTreeWidgetItem(item_psm_header);
    QString str_psm_marker_bit = QString("marker_bit=")+QString::number(header_psm.marker_bit)+" (1bits)";
    item_psm_marker_bit->setText(0, str_psm_marker_bit);

    QTreeWidgetItem *item_psm_program_stream_info_length = new QTreeWidgetItem(item_psm_header);
    QString str_psm_program_stream_info_length = QString("program_stream_info_length=")+QString::number(header_psm.program_stream_info_length)+" (16bits)";
    item_psm_program_stream_info_length->setText(0, str_psm_program_stream_info_length);

    QTreeWidgetItem *item_psm_program_stream_descriptors = new QTreeWidgetItem(item_psm_header);
    QString str_psm_program_stream_descriptors = QString("program_stream_descriptors (%1)").arg(header_psm.program_stream_info_length/sizeof(Descriptor));
    item_psm_program_stream_descriptors->setText(0, str_psm_program_stream_descriptors);

    for (int i = 0; i < header_psm.program_stream_info_length/sizeof(Descriptor); ++i)
    {
        QTreeWidgetItem *item_psm_program_stream_descriptors_index = new QTreeWidgetItem(item_psm_program_stream_descriptors);
        QString str_psm_program_stream_descriptors_index = QString::number(i);
        item_psm_program_stream_descriptors_index->setText(0, str_psm_program_stream_descriptors_index);

        QTreeWidgetItem *item_psm_program_stream_descriptors_index_descriptor_tag = new QTreeWidgetItem(item_psm_program_stream_descriptors);
        QString str_psm_program_stream_descriptors_index_descriptor_tag = QString("descriptor_tag=")+QString::number(header_psm.program_descriptors.at(i).descriptor_tag)+" (8bits)";
        item_psm_program_stream_descriptors_index_descriptor_tag->setText(0, str_psm_program_stream_descriptors_index_descriptor_tag);

        QTreeWidgetItem *item_psm_program_stream_descriptors_index_descriptor_length = new QTreeWidgetItem(item_psm_program_stream_descriptors);
        QString str_psm_program_stream_descriptors_index_descriptor_length = QString("descriptor_length=")+QString::number(header_psm.program_descriptors.at(i).descriptor_length)+" (8bits)";
        item_psm_program_stream_descriptors_index_descriptor_length->setText(0, str_psm_program_stream_descriptors_index_descriptor_length);
    }

    QTreeWidgetItem *item_psm_elementary_stream_map_length = new QTreeWidgetItem(item_psm_header);
    QString str_psm_elementary_stream_map_length = QString("elementary_stream_map_length=")+QString::number(header_psm.elementary_stream_map_length)+" (16bits)";
    item_psm_elementary_stream_map_length->setText(0, str_psm_elementary_stream_map_length);

    QTreeWidgetItem *item_psm_elementary_stream_map = new QTreeWidgetItem(item_psm_header);
    QString str_psm_elementary_stream_map = QString("elementary_stream_map");
    item_psm_elementary_stream_map->setText(0, str_psm_elementary_stream_map);

    for(int i=0; i<header_psm.elementary_streams.size(); i++)
    {
        QTreeWidgetItem *item_psm_elementary_stream_map_index = new QTreeWidgetItem(item_psm_elementary_stream_map);
        QString str_psm_elementary_stream_map_index = QString::number(i);
        item_psm_elementary_stream_map_index->setText(0, str_psm_elementary_stream_map_index);

        QTreeWidgetItem *item_psm_elementary_stream_map_index_stream_type = new QTreeWidgetItem(item_psm_elementary_stream_map_index);
        QString str_psm_elementary_stream_map_index_stream_type = QString("stream_type=")+QString::number(header_psm.elementary_streams.at(i).stream_type)+" (8bits)";
        item_psm_elementary_stream_map_index_stream_type->setText(0, str_psm_elementary_stream_map_index_stream_type);

        QTreeWidgetItem *item_psm_elementary_stream_map_index_elementary_stream_id = new QTreeWidgetItem(item_psm_elementary_stream_map_index);
        QString str_psm_elementary_stream_map_index_elementary_stream_id = QString("elementary_stream_id=")+QString::number(header_psm.elementary_streams.at(i).elementary_stream_id)+" (8bits)";
        item_psm_elementary_stream_map_index_elementary_stream_id->setText(0, str_psm_elementary_stream_map_index_elementary_stream_id);

        QTreeWidgetItem *item_psm_elementary_stream_map_index_elementary_stream_info_length = new QTreeWidgetItem(item_psm_elementary_stream_map_index);
        QString str_psm_elementary_stream_map_index_elementary_stream_info_length = QString("elementary_stream_info_length=")+QString::number(header_psm.elementary_streams.at(i).elementary_stream_info_length)+" (8bits)";
        item_psm_elementary_stream_map_index_elementary_stream_info_length->setText(0, str_psm_elementary_stream_map_index_elementary_stream_info_length);
    }

    QTreeWidgetItem *item_psm_crc = new QTreeWidgetItem(item_psm_header);
    QString str_psm_crc = QString("crc=$")+hexToQString(header_psm.crc, 8)+" (32bits)";
    item_psm_crc->setText(0, str_psm_crc);
}

void MainWindow::tree_add_pes_header(QTreeWidgetItem *item_root, PESHeader& header_pes)
{
    QTreeWidgetItem *item_pes_header = new QTreeWidgetItem(item_root);
    item_pes_header->setText(0, "pes header");

    QTreeWidgetItem *item_pes_pack_start_code = new QTreeWidgetItem(item_pes_header);
    QString str_pes_pack_start_code = QString("pcaket_start_code_prefix=$")+hexToQString(header_pes.packet_start_code_prefix, 6)+" (24bits)";
    item_pes_pack_start_code->setText(0, str_pes_pack_start_code);

    QTreeWidgetItem *item_pes_stream_id = new QTreeWidgetItem(item_pes_header);
    QString str_pes_stream_id = QString("stream_id=$")+hexToQString(header_pes.stream_id, 2)+" (8bits)";
    item_pes_stream_id->setText(0, str_pes_stream_id);

    QTreeWidgetItem *item_pes_PES_packet_length = new QTreeWidgetItem(item_pes_header);
    QString str_pes_PES_packet_length = QString("PES_packet_length=")+QString::number(header_pes.PES_packet_length)+" (16bits)";
    item_pes_PES_packet_length->setText(0, str_pes_PES_packet_length);

    QTreeWidgetItem *item_pes_PES_scrambling_control = new QTreeWidgetItem(item_pes_header);
    QString str_pes_PES_scrambling_control = QString("PES_scrambling_control=")+QString::number(header_pes.PES_scrambling_control)+" (2bits)";
    item_pes_PES_scrambling_control->setText(0, str_pes_PES_scrambling_control);

    QTreeWidgetItem *item_pes_PES_priority = new QTreeWidgetItem(item_pes_header);
    QString str_pes_PES_priority = QString("PES_priority=")+QString::number(header_pes.PES_priority)+" (1bits)";
    item_pes_PES_priority->setText(0, str_pes_PES_priority);

    QTreeWidgetItem *item_pes_data_alignment_indicator = new QTreeWidgetItem(item_pes_header);
    QString str_pes_data_alignment_indicator = QString("data_alignment_indicator=")+QString::number(header_pes.data_alignment_indicator)+" (1bits)";
    item_pes_data_alignment_indicator->setText(0, str_pes_data_alignment_indicator);

    QTreeWidgetItem *item_pes_copyright = new QTreeWidgetItem(item_pes_header);
    QString str_pes_copyright = QString("copyright=")+QString::number(header_pes.copyright)+" (1bits)";
    item_pes_copyright->setText(0, str_pes_copyright);

    QTreeWidgetItem *item_pes_original_or_copy = new QTreeWidgetItem(item_pes_header);
    QString str_pes_original_or_copy = QString("original_or_copy=")+QString::number(header_pes.original_or_copy)+" (1bits)";
    item_pes_original_or_copy->setText(0, str_pes_original_or_copy);

    QTreeWidgetItem *item_pes_PTS_DTS_flags = new QTreeWidgetItem(item_pes_header);
    QString str_pes_PTS_DTS_flags = QString("PTS_DTS_flags=")+QString::number(header_pes.PTS_DTS_flags)+" (2bits)";
    item_pes_PTS_DTS_flags->setText(0, str_pes_PTS_DTS_flags);

    QTreeWidgetItem *item_pes_ESCR_flag = new QTreeWidgetItem(item_pes_header);
    QString str_pes_ESCR_flag = QString("ESCR_flag=")+QString::number(header_pes.ESCR_flag)+" (1bits)";
    item_pes_ESCR_flag->setText(0, str_pes_ESCR_flag);

    QTreeWidgetItem *item_pes_ES_rate_flag = new QTreeWidgetItem(item_pes_header);
    QString str_pes_ES_rate_flag = QString("ES_rate_flag=")+QString::number(header_pes.ES_rate_flag)+" (1bits)";
    item_pes_ES_rate_flag->setText(0, str_pes_ES_rate_flag);

    QTreeWidgetItem *item_pes_DSM_trick_mode_flag = new QTreeWidgetItem(item_pes_header);
    QString str_pes_DSM_trick_mode_flag = QString("DSM_trick_mode_flag=")+QString::number(header_pes.DSM_trick_mode_flag)+" (1bits)";
    item_pes_DSM_trick_mode_flag->setText(0, str_pes_DSM_trick_mode_flag);

    QTreeWidgetItem *item_pes_additional_copy_info_flag = new QTreeWidgetItem(item_pes_header);
    QString str_pes_additional_copy_info_flag = QString("additional_copy_info_flag=")+QString::number(header_pes.additional_copy_info_flag)+" (1bits)";
    item_pes_additional_copy_info_flag->setText(0, str_pes_additional_copy_info_flag);

    QTreeWidgetItem *item_pes_PES_CRC_flag = new QTreeWidgetItem(item_pes_header);
    QString str_pes_PES_CRC_flag = QString("PES_CRC_flag=")+QString::number(header_pes.PES_CRC_flag)+" (1bits)";
    item_pes_PES_CRC_flag->setText(0, str_pes_PES_CRC_flag);

    QTreeWidgetItem *item_pes_PES_extension_flag = new QTreeWidgetItem(item_pes_header);
    QString str_pes_PES_extension_flag = QString("PES_extension_flag=")+QString::number(header_pes.PES_extension_flag)+" (1bits)";
    item_pes_PES_extension_flag->setText(0, str_pes_PES_extension_flag);

    QTreeWidgetItem *item_pes_PES_header_data_length = new QTreeWidgetItem(item_pes_header);
    QString str_pes_PES_header_data_length = QString("PES_header_data_length=")+QString::number(header_pes.PES_header_data_length)+" (8bits)";
    item_pes_PES_header_data_length->setText(0, str_pes_PES_header_data_length);

    if (header_pes.PTS_DTS_flags == 2)
    {
        QTreeWidgetItem *item_pes_reserved_01 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_reserved_01 = QString("reserved=0b0010 (4bits)");
        item_pes_reserved_01->setText(0, str_pes_reserved_01);

        QTreeWidgetItem *item_pes_PTS_32_30 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_PTS_32_30 = QString("PTS_32_30=")+QString::number(header_pes.PTS_32_30)+" (3bits)";
        item_pes_PTS_32_30->setText(0, str_pes_PTS_32_30);

        QTreeWidgetItem *item_pes_marker_bit_01 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_01 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_01->setText(0, str_pes_marker_bit_01);

        QTreeWidgetItem *item_pes_PTS_29_15 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_PTS_29_15 = QString("PTS_29_15=")+QString::number(header_pes.PTS_29_15)+" (15bits)";
        item_pes_PTS_29_15->setText(0, str_pes_PTS_29_15);

        QTreeWidgetItem *item_pes_marker_bit_02 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_02 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_02->setText(0, str_pes_marker_bit_02);

        QTreeWidgetItem *item_pes_PTS_14_0 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_PTS_14_0 = QString("PTS_14_0=")+QString::number(header_pes.PTS_14_0)+" (15bits)";
        item_pes_PTS_14_0->setText(0, str_pes_PTS_14_0);

        QTreeWidgetItem *item_pes_marker_bit_03 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_03 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_03->setText(0, str_pes_marker_bit_03);
    }

    if (header_pes.PTS_DTS_flags == 3)
    {
        QTreeWidgetItem *item_pes_reserved_01 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_reserved_01 = QString("reserved=0b0011 (4bits)");
        item_pes_reserved_01->setText(0, str_pes_reserved_01);

        QTreeWidgetItem *item_pes_PTS_32_30 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_PTS_32_30 = QString("PTS_32_30=")+QString::number(header_pes.PTS_32_30)+" (3bits)";
        item_pes_PTS_32_30->setText(0, str_pes_PTS_32_30);

        QTreeWidgetItem *item_pes_marker_bit_01 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_01 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_01->setText(0, str_pes_marker_bit_01);

        QTreeWidgetItem *item_pes_PTS_29_15 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_PTS_29_15 = QString("PTS_29_15=")+QString::number(header_pes.PTS_29_15)+" (15bits)";
        item_pes_PTS_29_15->setText(0, str_pes_PTS_29_15);

        QTreeWidgetItem *item_pes_marker_bit_02 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_02 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_02->setText(0, str_pes_marker_bit_02);

        QTreeWidgetItem *item_pes_PTS_14_0 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_PTS_14_0 = QString("PTS_14_0=")+QString::number(header_pes.PTS_14_0)+" (15bits)";
        item_pes_PTS_14_0->setText(0, str_pes_PTS_14_0);

        QTreeWidgetItem *item_pes_marker_bit_03 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_03 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_03->setText(0, str_pes_marker_bit_03);

        QTreeWidgetItem *item_pes_reserved_02 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_reserved_02 = QString("reserved=0b0001 (4bits)");
        item_pes_reserved_02->setText(0, str_pes_reserved_02);

        QTreeWidgetItem *item_pes_DTS_32_30 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_DTS_32_30 = QString("DTS_32_30=")+QString::number(header_pes.DTS_32_30)+" (3bits)";
        item_pes_DTS_32_30->setText(0, str_pes_DTS_32_30);

        QTreeWidgetItem *item_pes_marker_bit_04 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_04 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_04->setText(0, str_pes_marker_bit_04);

        QTreeWidgetItem *item_pes_DTS_29_15 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_DTS_29_15 = QString("DTS_29_15=")+QString::number(header_pes.DTS_29_15)+" (15bits)";
        item_pes_DTS_29_15->setText(0, str_pes_DTS_29_15);

        QTreeWidgetItem *item_pes_marker_bit_05 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_05 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_05->setText(0, str_pes_marker_bit_05);

        QTreeWidgetItem *item_pes_DTS_14_0 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_DTS_14_0 = QString("DTS_14_0=")+QString::number(header_pes.DTS_14_0)+" (15bits)";
        item_pes_DTS_14_0->setText(0, str_pes_DTS_14_0);

        QTreeWidgetItem *item_pes_marker_bit_06 = new QTreeWidgetItem(item_pes_header);
        QString str_pes_marker_bit_06 = QString("marker_bit=1 (1bits)");
        item_pes_marker_bit_06->setText(0, str_pes_marker_bit_06);
    }

    QTreeWidgetItem* item_addition;
    if (header_pes.ESCR_flag == 1)
    {
        item_addition = new QTreeWidgetItem(item_pes_header);
        item_addition->setText(0, "need to add ESCR_flag");
    }

    if (header_pes.ES_rate_flag == 1)
    {
        item_addition = new QTreeWidgetItem(item_pes_header);
        item_addition->setText(0, "need to add ES_rate_flag");
    }

    if (header_pes.DSM_trick_mode_flag == 1)
    {
        item_addition = new QTreeWidgetItem(item_pes_header);
        item_addition->setText(0, "need to add DSM_trick_mode_flag");
    }

    if (header_pes.additional_copy_info_flag == 1)
    {
        item_addition = new QTreeWidgetItem(item_pes_header);
        item_addition->setText(0, "need to add additional_copy_info_flag");
    }

    if (header_pes.PES_CRC_flag == 1)
    {
        item_addition = new QTreeWidgetItem(item_pes_header);
        item_addition->setText(0, "need to add PES_CRC_flag");
    }

    if (header_pes.PES_extension_flag == 1)
    {
        item_addition = new QTreeWidgetItem(item_pes_header);
        item_addition->setText(0, "need to add PES_extension_flag");
    }
}

void MainWindow::onTableWidgetRowSelected()
{
    // 获取当前选择的行
    QItemSelectionModel *selectionModel = ui->tableWidget->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedRows();
    if (!selectedIndexes.isEmpty()) {
        // 获取选择的第一行的索引
        int row = selectedIndexes.first().row();
        qDebug() << "select row " << row;
        QByteArray packet = packets.at(row);
        emit sig_ps_packet(packet);
        showHexData(ui->textBrowser, packet);
    }
}
