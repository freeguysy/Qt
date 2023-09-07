#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QContextMenuEvent>
#include <QLabel>
#include <QMenu>
#include <QPoint>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include "myweatherData.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //右键退出菜单、退出菜单项
    QMenu* mExitMenu = nullptr;
    QAction* mExitAct = nullptr;

    QPoint mOffset;   //窗口移动时，鼠标与窗口左上角的距离

    //处理HTTP请求
    QNetworkAccessManager* mNetworkManager;

    Today mToday;
    Day mDay[6];

    //星期和日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDatekList;

    //天气和图标
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;

    //污染指数
    QList<QLabel*> mAqiList;

    //风力和风向
    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;

    QMap<QString, QString> mTypeMap;
private slots:
    //HTTP请求槽函数
    void onReplied(QNetworkReply* replay);

    void on_btnSearch_clicked();

    void on_leCity_returnPressed();

protected:
    //处理上下文菜单事件。上下文菜单是在用户右键单击控件或其他可接收上下文菜单事件的区域时显示的菜单。
    void contextMenuEvent(QContextMenuEvent* event);

    //鼠标移动窗口
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    //获取天气信息、解析JSON、更新UI
    void getWeatherInfo(QString cityName);
    void parseJson(QByteArray& byteArray);
    void updateUi();

    //重写父类的eventfilter方法
    bool eventFilter(QObject* watched,QEvent* event);

    //绘制高低温曲线
    void paintHighCurve();
    void paintLowCurve();
};
#endif // MAINWINDOW_H
