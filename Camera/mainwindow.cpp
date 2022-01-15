//********************************************************//
//*******************Gitee:多云转晴************************//
//*******************Github:CloudyToSunny****************//
//*******************CSDN:Cloudy_to_sunny****************//
//*******************************************************//
//*******************************************************//
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QCameraInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QComboBox *cameraType = new QComboBox();
    box = ui->fbl;
    cameraType = ui->cameraType;
    cameraType->clear();
    //获取可用的摄像头设备并添加到选择框中
    cameras = QCameraInfo::availableCameras();
    foreach(const QCameraInfo &cameraInfo, cameras) {
        qDebug() << "CameraInfo:" << cameraInfo;
        cameraType->addItem(cameraInfo.description());
    }
    //定义一个摄像头
    camera = new QCamera(this);
    viewfind = new QCameraViewfinder(ui->viewCAM);//设置显示框
    ui->viewCAM->setFixedSize(int(1280),int(720));//设置显示框的大小
    //注意：显示框的大小一般设置成与该摄像头设备的分辨率一致，若不一致则会出现有一段区域卡住
    viewfind->resize(ui->viewCAM->size());

    viewfind->show();//开启显示
    camera->setViewfinder(viewfind);
    imageCapture = new QCameraImageCapture(camera);
    camera->start();//开启摄像头

    connect(imageCapture, SIGNAL(imageCaptured(int, QImage)), this, SLOT(on_imageCaptured(int, QImage)));

    setfblComobox(camera);//获取该设备能够使用的分辨率
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cameraType_activated(int index)
{
    index = ui->cameraType->currentIndex();//获取现在索引的哪个摄像头设备
    qDebug()<<"Index"<< index <<": "<< ui->cameraType->currentText();
    camera->stop();//先把目前的摄像头关闭掉
    camera = new QCamera(cameras[index]);//再定义新选择的摄像头设备
    camera->setCaptureMode(QCamera::CaptureVideo);//设置捕捉模式
    camera->setViewfinder(viewfind);//设置显示框
    imageCapture = new QCameraImageCapture(camera);//定义拍照对象
    camera->start();//开启摄像头
    connect(imageCapture, SIGNAL(imageCaptured(int, QImage)), this, SLOT(on_imageCaptured(int, QImage)));
    setfblComobox(camera);//获取该设备能够使用的分辨率
}

void MainWindow::on_imageCaptured(int id, const QImage &preview){
    //将拍好的照片适应于label的尺寸显示，这样显示的大小就可以改变label的尺寸而改变照片显示的尺寸了
    ui->imageview->setPixmap(QPixmap::fromImage(preview.scaled(ui->imageview->width(),ui->imageview->height())));
}

void MainWindow::on_screenshot_clicked()
{
    qDebug()<<"ScreenShot";

    imageCapture->capture();//拍照
}

void MainWindow::on_fbl_activated(int index){
    index = ui->fbl->currentIndex();
    qDebug()<<"Index"<< index <<": "<< ui->fbl->currentText();
    qDebug()<<"mResSize:"<<mResSize[index];

    //因为分辨率改变了所以要重新设定一下显示框的大小
    ui->viewCAM->setFixedSize(mResSize[index].width(),mResSize[index].height());
    viewfind->resize(ui->viewCAM->size());
    camera->setViewfinder(viewfind);

    //设置摄像头参数
    QCameraViewfinderSettings set;
    set.setResolution(mResSize[index]);
    camera->setViewfinderSettings(set);
}

void MainWindow::setfblComobox(QCamera *camera){

    mResSize.clear();//清楚设备可用分辨率
    mResSize = camera->supportedViewfinderResolutions();//获取设备可用分辨率
    box->clear();//清楚box中已有的
    int i=0;
    foreach (QSize msize, mResSize) {
       qDebug()<<msize;
       box->addItem(QString::number(msize.width(),10)+"*"+QString::number(msize.height(),10), i++);//将每个可用的分辨率添加到box中
    }  //摄像头支持分辨率打印
    box->setCurrentIndex(i-1);//选择最后一个
}

