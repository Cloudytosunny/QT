@[TOC](QT摄像头)

# 效果

调用摄像头，显示可使用的摄像头设备，显示可使用摄像头设备的分辨率，保存捕捉的图片
![在这里插入图片描述](https://img-blog.csdnimg.cn/6d009cbc29b84d279b72f3daea70744a.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBAQ2xvdWR5X3RvX3N1bm55,size_20,color_FFFFFF,t_70,g_se,x_16)
# 配置
QT5.9.8+MSVC2017 64bIt
# 代码
Camera.pro
```javascript
QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Camera
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

```
mainwindow.h
```javascript

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setfblComobox(QCamera *camera);

private slots:
    void on_screenshot_clicked();  //拍照按钮

    void on_cameraType_activated(int index); //摄像头设备选取

    void on_imageCaptured(int id, const QImage &preview);//拍照后显示

    void on_fbl_activated(int index);//分辨率选取

private:
    Ui::MainWindow *ui;
    QCamera *camera;//定义摄像头
    QCameraViewfinder *viewfind;//画面显示框
    QList<QCameraInfo> cameras;//摄像头设备列表
    QCameraImageCapture *imageCapture;
    QList<QSize> mResSize = {};//分辨率List 定义
    QComboBox *box;
};
#endif // MAINWINDOW_H
```

main.cpp
```javascript
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

```
mainwindow.cpp
```javascript
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
```

mainwindows.ui
![在这里插入图片描述](https://img-blog.csdnimg.cn/df7bc974d3414f1f945da073eae2482c.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBAQ2xvdWR5X3RvX3N1bm55,size_20,color_FFFFFF,t_70,g_se,x_16)
# 总结
这一摄像头的使用首先使用的是QT自带的QCamera类，来创建的摄像头的使用，QT自身对其进行了优化必然不会占用很多的资源。即使把这一过程放在主线程（即GUI线程）并且你编写的工程很大，摄像头几乎也不会卡，时延也很低。但是缺点是，QT自带的QCamera需要用到QCameraViewfinder而这个东西呢不能适配自定义的显示框的大小，即如果显示框的大小与目前摄像头的分辨率不成比例时，那么它就会自动按跟分辨率相比，短的长或者宽作为分辨率的长或者宽，这时就会多出一部分被卡掉，卡成上好几帧的画面，引起极度不适。比如：摄像头当前的分辨率为4：3，而我设置的显示框为5：3那么右边这一部分（黄色的这一部分）的1：3便会卡住，即它不会自动拉伸，只会按照分辨率的大小的比例显示，多余的部分不能很好的显示。因此，这不是一种完美的使用摄像头方式。除此之外，我们希望得到具体的视频流，任自己随便处理，因此可以看我的其他的文章来解决这个问题


![请添加图片描述](https://img-blog.csdnimg.cn/4c3cc21979dc4d49b1d5dfc77ec1bbd9.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBAQ2xvdWR5X3RvX3N1bm55,size_20,color_FFFFFF,t_70,g_se,x_16)
本文章的代码在[这里](https://gitee.com/cloudytosunny/qt/tree/master/Camera)或者[这里](https://github.com/Cloudytosunny/QT/tree/master/Camera).
