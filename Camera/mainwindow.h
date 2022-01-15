
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

