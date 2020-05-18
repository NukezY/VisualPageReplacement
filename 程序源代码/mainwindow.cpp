#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextCodec>
#include <iostream>
#include <QTextStream>
#include <QVector>
#include <QEventLoop>
#include <QTimer>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* 内存块滑块设计*/
    ui->Slider_kuaishu->setMinimum(1);  // 最小值
    ui->Slider_kuaishu->setMaximum(9);  // 最大值
    ui->Slider_kuaishu->setSingleStep(1); // 步长
    ui->Slider_kuaishu->setTickInterval(1);  // 设置刻度间隔
    ui->Slider_kuaishu->setTickPosition(QSlider::TicksAbove);  //刻度在上方



    /* 按钮、滑块点击相应关联（信号与槽相连）*/
    QObject::connect(ui->PB_openfile,SIGNAL(clicked()),this,SLOT(openfile())); // 打开文件
    QObject::connect(ui->PB_qufilecreate,SIGNAL(clicked()),this,SLOT(filecreat()));  // 创建文件
    QObject::connect(ui->PB_allclear,SIGNAL(clicked()),this,SLOT(allclear()));
    QObject::connect(ui->Slider_kuaishu,SIGNAL(valueChanged(int)),this,SLOT(getslider())); // 内存块 滑块响应


    QObject::connect(ui->PB_opt,SIGNAL(clicked()),this,SLOT(switchPage()));
    QObject::connect(ui->PB_fifo,SIGNAL(clicked()),this,SLOT(switchPage()));
    QObject::connect(ui->PB_lru,SIGNAL(clicked()),this,SLOT(switchPage()));
    // 算法页面切换响应

    QObject::connect(ui->PB_onerun,SIGNAL(clicked()),this,SLOT(onerun()));  // 单步模拟响应
    QObject::connect(ui->PB_run,SIGNAL(clicked()),this,SLOT(run()));        // 单步模拟响应
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openfile()
{
    QString filename=QFileDialog::getOpenFileName(this,"open file",QDir::homePath());
    // 文件打开窗口对象声明

    if(filename.isEmpty())
    {
        QMessageBox::information(this,"Error Message","Please Select a File");
        //未选文件 弹窗提醒

        QTextCodec* codec=QTextCodec::codecForName("UTF-8");
        QString str=codec->toUnicode("打开文件失败！");
        ui->textEdit->append(str);
        //流程框显示 文件打开失败

        return;
    }
    else
    {
        ui->lineEdit_qufile->setText(filename); // 显示文件路径

        QTextCodec* codec=QTextCodec::codecForName("UTF-8");
        QString str=codec->toUnicode("打开文件成功！");
        ui->textEdit->append(str);
        //  流程框显示 文件打开失成功

        QFile fp(filename);
        QVector<int> array;
        if(fp.open(fp.ReadOnly))
        {
            QTextStream ts(&fp);
            while(!ts.atEnd())
            {
                static int buf;
                ts>>buf;
                array.append(buf);
            }
            fp.close();
        }
        taskSize=array.length();
        task = new int[taskSize];
        page = new int[taskSize*1000];      // 如果不对page初始化，则会变为野指针
        pgLack = new char[taskSize];
        for(int i=0;i<array.length();i++)
        {
            task[i]=array[i];
        }
        // 将文件中的访问串记录
    }
}
void MainWindow::filecreat(){
    QFile::remove("D:/taskCreat.txt");
    QFile file("D:/taskCreat.txt");
    QString str;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i =0; i< 10 ; i++){
        str += QString::number(qrand()%10,10);
        str += " ";
    }
    str = str.left(str.length()-1);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    //写入文件需要字符串为QByteArray格式
    file.write(str.toUtf8());
    file.close();
    QMessageBox::information(this,"Error Message","已创建文件");

    QTextCodec* codec=QTextCodec::codecForName("UTF-8");
    str=codec->toUnicode("文件路径：D:/taskCreat.txt");
    ui->textEdit->append(str);
}
void MainWindow::allclear(){
    lackpage= 0;
}
void MainWindow::getslider(){
    QTextCodec* codec=QTextCodec::codecForName("UTF-8");
    QString str=codec->toUnicode("当前内存块选择值：");
    QString s = QString::number(ui->Slider_kuaishu->value(), 10);
    ui->textEdit->append(str+s);

    pageSize = ui->Slider_kuaishu->value(); // 对内存块数量保存。
}
void MainWindow::switchPage(){
    QPushButton *button = qobject_cast<QPushButton*>(sender());//得到按下的按钮的指针
    QTextCodec* codec=QTextCodec::codecForName("UTF-8");
    QString str;
    if(button==ui->PB_opt){
        ui->stackWid->setCurrentIndex(0);//根据按下的button按索引显示相应的页面
        str=codec->toUnicode("已切换到opt算法");
        pageAl = "opt";
    }
    else if(button==ui->PB_fifo){
        ui->stackWid->setCurrentIndex(1);
        str=codec->toUnicode("已切换到fifo算法");
        pageAl = "fifo";
    }
    else if(button==ui->PB_lru){
        ui->stackWid->setCurrentIndex(2);
        str=codec->toUnicode("已切换到lru算法");
        pageAl = "lru";
    }
    ui->textEdit->append(str);
}
void MainWindow::run(){
    pageIndx= 0;
    if(lackpage > 0 ) QMessageBox::information(this,"ERROR Message","已模拟，不再模拟");
    else{
        if(pageAl == "opt"){
            QVector<QVector<int> > fin(taskSize,QVector<int> (pageSize));   // page数据数组。
            for(int i=0;i<taskSize;i++)
            {
                int lastlack = lackpage;    // 上次的缺页数
                al_opt->work(task,i, page,taskSize, pageSize,pageIndx,lackpage);   // 进行当前序列值的计算。
                if(lastlack != lackpage) pgLack[i] = 'F';
                else pgLack[i] =' ';    // 缺页数改变的话 对当前进行标记。
                for(int j = 0 ; j < pageSize; j++){
                    if( j < pageIndx)   fin[i][j] = page[j];
                    else    fin[i][j] = -1;
                }   //  对page数据保存
            }

            QStandardItemModel *model = new QStandardItemModel();   // 表模式声明。
            QStringList labels;     //   表头字符串声明。
            for(int i =0 ;i<taskSize;i++){
                labels << QString::number(task[i],10);      // 把访问串写入字符串。
            }
            model->setHorizontalHeaderLabels(labels);
            //  把访问串设置为表头
            QStandardItem   *item = 0;
            for(int i = 0; i<taskSize;i++){
                for(int j = 0; j<pageSize;j++){
                    item = new QStandardItem(QString("%1").arg(fin[i][j]));
                    model->setItem(j,i,item);   //  j,i,item（行，列，值）；
                }
                item = new QStandardItem(QString("%1").arg(pgLack[i]));
                model->setItem(pageSize,i,item);    // 最后一行 缺页标志显示。
            }
            ui->taVi_opt->setModel(model);     // 加载模式。
            ui->taVi_opt->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents );
            //  把数据表格化显示。
            vilack(); // 缺页率计算
        }
        else if(pageAl == "fifo" ){
            QVector<QVector<int> > fin(taskSize,QVector<int> (pageSize));   // page数据数组。
            for(int i=0;i<taskSize;i++)
            {
                int lastlack = lackpage;    // 上次的缺页数
                al_fifo->work(task[i], page, pageSize,pageIndx,lackpage);   // 进行当前序列值的计算。
                if(lastlack != lackpage) pgLack[i] = 'F';
                else pgLack[i] =' ';    // 缺页数改变的话 对当前进行标记。
                for(int j = 0 ; j < pageSize; j++){
                    if( j < pageIndx)   fin[i][j] = page[j];
                    else    fin[i][j] = -1;
                }   //  对page数据保存
            }

            QStandardItemModel *model = new QStandardItemModel();   // 表模式声明。
            QStringList labels;     //   表头字符串声明。
            for(int i =0 ;i<taskSize;i++){
                labels << QString::number(task[i],10);      // 把访问串写入字符串。
            }
            model->setHorizontalHeaderLabels(labels);
            //  把访问串设置为表头
            QStandardItem   *item = 0;
            for(int i = 0; i<taskSize;i++){
                for(int j = 0; j<pageSize;j++){
                    item = new QStandardItem(QString("%1").arg(fin[i][j]));
                    model->setItem(j,i,item);   //  j,i,item（行，列，值）；
                }
                item = new QStandardItem(QString("%1").arg(pgLack[i]));
                model->setItem(pageSize,i,item);    // 最后一行 缺页标志显示。
            }
            ui->taVi_fifo->setModel(model);     // 加载模式。
            ui->taVi_fifo->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents );
            //  把数据表格化显示。

            vilack(); // 缺页率计算
        }
        else if(pageAl == "lru" ){
            QVector<QVector<int> > fin(taskSize,QVector<int> (pageSize));   // page数据数组。
            for(int i=0;i<taskSize;i++)
            {
                int lastlack = lackpage;    // 上次的缺页数
                al_lru->work(task[i], page, pageSize,pageIndx,lackpage);   // 进行当前序列值的计算。
                if(lastlack != lackpage) pgLack[i] = 'F';
                else pgLack[i] =' ';    // 缺页数改变的话 对当前进行标记。
                for(int j = 0 ; j < pageSize; j++){
                    if( j < pageIndx)   fin[i][j] = page[j];
                    else    fin[i][j] = -1;
                }   //  对page数据保存
            }

            QStandardItemModel *model = new QStandardItemModel();   // 表模式声明。
            QStringList labels;     //   表头字符串声明。
            for(int i =0 ;i<taskSize;i++){
                labels << QString::number(task[i],10);      // 把访问串写入字符串。
            }
            model->setHorizontalHeaderLabels(labels);
            //  把访问串设置为表头
            QStandardItem   *item = 0;
            for(int i = 0; i<taskSize;i++){
                for(int j = 0; j<pageSize;j++){
                    item = new QStandardItem(QString("%1").arg(fin[i][j]));
                    model->setItem(j,i,item);   //  j,i,item（行，列，值）；
                }
                item = new QStandardItem(QString("%1").arg(pgLack[i]));
                model->setItem(pageSize,i,item);    // 最后一行 缺页标志显示。
            }
            ui->taVi_lru->setModel(model);     // 加载模式。
            ui->taVi_lru->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents );
            //  把数据表格化显示。

            vilack(); // 缺页率计算
        }
        else{
            QTextCodec* codec1=QTextCodec::codecForName("UTF-8");
            QString str=codec1->toUnicode("请先选择置换算法。");
            ui->textEdit->append(str);
        }
    }
}
void MainWindow::onerun(){
    pageIndx= 0;
    if(lackpage > 0 ) QMessageBox::information(this,"ERROR Message","已模拟，不再模拟");
    else{
        if(pageAl == "opt"){
            QVector<QVector<int> > fin(taskSize,QVector<int> (pageSize));   // page数据数组。
            for(int i=0;i<taskSize;i++)
            {
                int lastlack = lackpage;    // 上次的缺页数
                 al_opt->work(task,i, page,taskSize, pageSize,pageIndx,lackpage);   // 进行当前序列值的计算。
                if(lastlack != lackpage) pgLack[i] = 'F';
                else pgLack[i] =' ';    // 缺页数改变的话 对当前进行标记。
                for(int j = 0 ; j < pageSize; j++){
                    if( j < pageIndx)   fin[i][j] = page[j];
                    else    fin[i][j] = -1;
                }   //  对page数据保存
            }
            QStandardItemModel *model = new QStandardItemModel();   // 表模式声明。
            QStringList labels;     //   表头字符串声明。
            for(int i =0 ;i<taskSize;i++){
                labels << QString::number(task[i],10);      // 把访问串写入字符串。
            }
            model->setHorizontalHeaderLabels(labels);
            //  把访问串设置为表头
            QStandardItem   *item = 0;
            for(int i = 0; i<taskSize;i++){
                /*
                  在一次性把数据全显示的基础上，在增加一层循环k，
                  实现依次显示：
                  1列
                  2列
                  3列
                  。。。。
                  再加上休眠函数，可以大致简单的体现一步一步计算的结果。
                */
                for(int k =0 ; k<=i;k++){
                    for(int j = 0; j<pageSize;j++){
                        item = new QStandardItem(QString("%1").arg(fin[k][j]));
                        model->setItem(j,k,item);   //  j,i,item（行，列，值）；
                    }
                    item = new QStandardItem(QString("%1").arg(pgLack[k]));
                    model->setItem(pageSize,k,item);    // 最后一行 缺页标志显示。
                    ui->taVi_opt->setModel(model);     // 加载模式。
                    ui->taVi_opt->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents );
                }
                mwsleep(2000); // 间隔2s
            }

            //  把数据表格化显示。

            vilack(); // 缺页率计算
        }
        else if(pageAl == "fifo"){
            QVector<QVector<int> > fin(taskSize,QVector<int> (pageSize));   // page数据数组。
            for(int i=0;i<taskSize;i++)
            {
                int lastlack = lackpage;    // 上次的缺页数
                al_fifo->work(task[i], page, pageSize,pageIndx,lackpage);   // 进行当前序列值的计算。
                if(lastlack != lackpage) pgLack[i] = 'F';
                else pgLack[i] =' ';    // 缺页数改变的话 对当前进行标记。
                for(int j = 0 ; j < pageSize; j++){
                    if( j < pageIndx)   fin[i][j] = page[j];
                    else    fin[i][j] = -1;
                }   //  对page数据保存
            }
            QStandardItemModel *model = new QStandardItemModel();   // 表模式声明。
            QStringList labels;     //   表头字符串声明。
            for(int i =0 ;i<taskSize;i++){
                labels << QString::number(task[i],10);      // 把访问串写入字符串。
            }
            model->setHorizontalHeaderLabels(labels);
            //  把访问串设置为表头
            QStandardItem   *item = 0;
            for(int i = 0; i<taskSize;i++){
                /*
                  在一次性把数据全显示的基础上，在增加一层循环k，
                  实现依次显示：
                  1列
                  2列
                  3列
                  。。。。
                  再加上休眠函数，可以大致简单的体现一步一步计算的结果。
                */
                for(int k =0 ; k<=i;k++){
                    for(int j = 0; j<pageSize;j++){
                        item = new QStandardItem(QString("%1").arg(fin[k][j]));
                        model->setItem(j,k,item);   //  j,i,item（行，列，值）；
                    }
                    item = new QStandardItem(QString("%1").arg(pgLack[k]));
                    model->setItem(pageSize,k,item);    // 最后一行 缺页标志显示。
                    ui->taVi_fifo->setModel(model);     // 加载模式。
                    ui->taVi_fifo->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents );
                }
                mwsleep(2000); // 间隔2s
            }

            //  把数据表格化显示。

            vilack(); // 缺页率计算
        }
        else if(pageAl == "lru"){
            QVector<QVector<int> > fin(taskSize,QVector<int> (pageSize));   // page数据数组。
            for(int i=0;i<taskSize;i++)
            {
                int lastlack = lackpage;    // 上次的缺页数
                al_lru->work(task[i], page, pageSize,pageIndx,lackpage);   // 进行当前序列值的计算。
                if(lastlack != lackpage) pgLack[i] = 'F';
                else pgLack[i] =' ';    // 缺页数改变的话 对当前进行标记。
                for(int j = 0 ; j < pageSize; j++){
                    if( j < pageIndx)   fin[i][j] = page[j];
                    else    fin[i][j] = -1;
                }   //  对page数据保存
            }
            QStandardItemModel *model = new QStandardItemModel();   // 表模式声明。
            QStringList labels;     //   表头字符串声明。
            for(int i =0 ;i<taskSize;i++){
                labels << QString::number(task[i],10);      // 把访问串写入字符串。
            }
            model->setHorizontalHeaderLabels(labels);
            //  把访问串设置为表头
            QStandardItem   *item = 0;
            for(int i = 0; i<taskSize;i++){
                /*
                  在一次性把数据全显示的基础上，在增加一层循环k，
                  实现依次显示：
                  1列
                  2列
                  3列
                  。。。。
                  再加上休眠函数，可以大致简单的体现一步一步计算的结果。
                */
                for(int k =0 ; k<=i;k++){
                    for(int j = 0; j<pageSize;j++){
                        item = new QStandardItem(QString("%1").arg(fin[k][j]));
                        model->setItem(j,k,item);   //  j,i,item（行，列，值）；
                    }
                    item = new QStandardItem(QString("%1").arg(pgLack[k]));
                    model->setItem(pageSize,k,item);    // 最后一行 缺页标志显示。
                    ui->taVi_lru->setModel(model);     // 加载模式。
                    ui->taVi_lru->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents );
                }
                mwsleep(2000); // 间隔2s
            }

            //  把数据表格化显示。

            vilack(); // 缺页率计算
        }
        else{
            QTextCodec* codec1=QTextCodec::codecForName("UTF-8");
            QString str=codec1->toUnicode("请先选择置换算法。");
            ui->textEdit->append(str);
        }
    }
}
void MainWindow::vilack(){
    QTextCodec* codec1=QTextCodec::codecForName("UTF-8");   // 数据格式
    QString str1=codec1->toUnicode("总页数");
    QString s1 = QString::number(taskSize, 10);
    ui->textEdit->append(str1+s1);  // 消息框显示总页数
    str1=codec1->toUnicode("缺页数");
    s1 = QString::number(lackpage, 10);
    ui->textEdit->append(str1+s1);  // 消息框显示缺页数
    str1=codec1->toUnicode("缺页率");
    double d=(double)lackpage/taskSize*100;
    s1 = QString::number(d, 10,2);
    ui->textEdit->append(str1+s1+"%"); // 消息框显示缺页率
}
void MainWindow::mwsleep(int m)
{

    QEventLoop eventloop;
    QTimer::singleShot(m, &eventloop, SLOT(quit()));
    eventloop.exec();
}
