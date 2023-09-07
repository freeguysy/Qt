#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "weathertool.h"
#include <QPainter>

#define INCREMENT 3 //温度每升高或降低1度，y坐标的增量为3
#define POINT_RADIUS 3  //曲线锚点的大小
#define TEXT_OFFSET_X 12
#define TEXT_OFFSET_Y 12

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);     //设置窗口无边框
    setFixedSize(width(),height());     //设置固定窗口大小

    //构建右键菜单
    mExitMenu = new QMenu(this);
    mExitAct = new QAction(this);


    mExitAct->setText("退出");
    mExitAct->setIcon(QIcon(":/res/close.png"));

    mExitMenu->addAction(mExitAct);

    //lambda表达式[=] { qApp->exit(0); } 定义了一个无参数的匿名函数，函数体中调用了qApp->exit(0)来退出应用程序。
    connect(mExitAct,&QAction::triggered,this,[=](){qApp->exit(0);});

    //将控件添加到控件数组
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDatekList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;
    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;
    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    //图标
    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu.png");
    mTypeMap.insert("暴雨到大暴雨",":/res/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到特大暴雨",":/res/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/res/type/DaDaoBaoXue.png");
    mTypeMap.insert("大到暴雨",":/res/type/DaDaoBaoYu.png");
    mTypeMap.insert("大雪",":/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮沉",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("undefined",":/res/type/undefined.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小到中雪",":/res/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小到中雨",":/res/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/res/type/Xue.png");
    mTypeMap.insert("扬沙",":/res/type/YangSha.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/Yu.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("中到大雪",":/res/type/ZhongDaoDaXue.png");
    mTypeMap.insert("中到大雨",":/res/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");


    //网络请求
    mNetworkManager = new QNetworkAccessManager(this);
    connect(mNetworkManager,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);

//    getWeatherInfo("101010100");    //北京城市编码
      getWeatherInfo("孝感");    //广州城市编码
//    getWeatherInfo("101200401");    //孝感城市编码

      //5、给标签添加事件过滤器
      ui->lblHighCurve->installEventFilter(this);
      ui->lblLowCurve->installEventFilter(this);

}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onReplied(QNetworkReply *replay)
{
    //获取 QNetworkReply 对象中 HTTP 响应状态码
    int status_code = replay->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//    qDebug() << "operation" << replay->operation();
//    qDebug() << "status_code" << status_code;
//    qDebug() << "url" << replay->url();
//    qDebug() << "raw header" << replay->rawHeaderList();

    //请求出错
    if(replay->error() != QNetworkReply::NoError || status_code !=200){
        qDebug() << replay->errorString().toLatin1().data();
        QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Ok);
    }else{
        QByteArray byteArray =  replay->readAll();
        qDebug() << " " << byteArray.data();
        parseJson(byteArray);
    }
    //释放堆上的资源
    replay->deleteLater();
}

//重写父类的虚函数，父类默认实现是忽略右键菜单事件
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    //弹出右键菜单,根据鼠标位置
    mExitMenu->exec(QCursor::pos());

    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //鼠标到窗口左上角的距离-子窗口左上角到父窗口左上角的距离
    mOffset =  event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //新左上角位置-偏移量
    this->move(event->globalPos() - mOffset);
}

void MainWindow::getWeatherInfo(QString cityName)
{
    QString cityCode =  WeatherTool::getCityCode(cityName);
//    qDebug() << cityCode;

    if(cityCode.isEmpty()){
        QMessageBox::warning(this,"天气","请检查输入是否正确",QMessageBox::Ok);
        return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    mNetworkManager->get(QNetworkRequest(url));
}

void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc =  QJsonDocument::fromJson(byteArray,&err);

    if(err.error != QJsonParseError::NoError){
        return ;
    }

    QJsonObject rootObj =  doc.object();
//    qDebug() << rootObj.value("message").toString();

    //1、解析日期和城市
    mToday.date =  rootObj.value("date").toString();
    mToday.city =  rootObj.value("cityInfo").toObject().value("city").toString();

    //2、解析yesterday
    QJsonObject objData = rootObj.value("data").toObject();

    QJsonObject objYesterday = objData.value("yesterday").toObject();

    mDay[0].week = objYesterday.value("week").toString();
    mDay[0].date = objYesterday.value("ymd").toString();
    mDay[0].type = objYesterday.value("type").toString();

    //高温低温
    QString s;
    //以空格分隔，获取索引为1的数据，然后去除℃字符
    s = objYesterday.value("high").toString().split(" ").at(1);
    s=s.left(s.length()-1);
    mDay[0].high = s.toInt();

    s = objYesterday.value("low").toString().split(" ").at(1);
    s=s.left(s.length()-1);
    mDay[0].low = s.toInt();
//    mDay[0].high = objYesterday.value("high").toString().toInt();
//    mDay[0].low = objYesterday.value("low").toString().toInt();


    //风向、风力
    mDay[0].fx =  objYesterday.value("fx").toString();
    mDay[0].fl = objYesterday.value("fl").toString();
    mDay[0].aqi = objYesterday.value("aqi").toDouble();

    //3、解析forcost中5天的数据
    QJsonArray forecastArray =  objData.value("forecast").toArray();

    for(int i=0;i<5;i++){
        QJsonObject objForecast =  forecastArray[i].toObject();
        mDay[i+1].week = objForecast.value("week").toString();
        mDay[i+1].date = objForecast.value("ymd").toString();
        mDay[i+1].type = objForecast.value("type").toString();

        //高温低温
        QString s;
        s = objForecast.value("high").toString().split(" ").at(1);
        s=s.left(s.length()-1);
        mDay[i+1].high = s.toInt();

        s = objForecast.value("low").toString().split(" ").at(1);
        s=s.left(s.length()-1);
        mDay[i+1].low = s.toInt();


        //风向、风力
        mDay[i+1].fx =  objForecast.value("fx").toString();
        mDay[i+1].fl = objForecast.value("fl").toString();
        mDay[i+1].aqi = objForecast.value("aqi").toDouble();

    }

    //4、解析今天的数据
    mToday.ganmao =  objData.value("ganmao").toString();
    mToday.wendu =  objData.value("wendu").toString().toInt();
    mToday.shidu =  objData.value("shidu").toString();
    mToday.pm25 =  objData.value("pm25").toDouble();
    mToday.quality =  objData.value("quality").toString();

    //5、forecast中第一个数组元素，也是今天的数据
    mToday.type = mDay[1].type;
    mToday.fl = mDay[1].fl;
    mToday.fx = mDay[1].fx;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    //获取数据完毕，更新UI
    updateUi();


    //更新温度曲线(相当于手动刷新，然后信号会被mainWindow拦截并处理，就实现了曲线的更新)
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}

void MainWindow::updateUi()
{
    //更新日期和城市
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyymmdd").toString("yyyy/mm/dd") + " " + mDay[1].week);
    ui->lblCity->setText(mToday.city);

    //更新今天
    ui->lblTypeIcon->setPixmap(mTypeMap[mToday.type]);
    ui->lblTemp->setText(QString::number(mToday.wendu)+"℃");
    ui->lblType->setText(mToday.type);
    ui->lblLowHigh->setText(QString::number(mToday.low) + "~" + QString::number(mToday.high) + "℃");
    ui->lblGanMao->setText("感冒指数" + mToday.ganmao);
    ui->lblWindFx->setText(mToday.fx);
    ui->lblWindFl->setText(mToday.fl);
    ui->lblPM25->setText(QString::number(mToday.pm25));
    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);

    //更新六天
    for(int i=0;i<6;i++){
        //更新日期和时间
        mWeekList[i]->setText("周" + mDay[i].week.right(1));
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");

        QStringList ymdList = mDay[i].date.split("-");
        mDatekList[i]->setText(ymdList[1] + "/" + ymdList[2]);

        //更新天气类型

        mTypeList[i]->setText(mDay[i].type);
        mTypeIconList[i]->setPixmap(mTypeMap[mDay[i].type]);

        //更新空气质量
        if(mDay[i].aqi >= 0 && mDay[i].aqi <= 50){
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(121,184,0);");
        }else if(mDay[i].aqi > 50 && mDay[i].aqi <= 100){
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,187,23);");
        }else if(mDay[i].aqi > 100 && mDay[i].aqi <= 150){
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,87,97);");
        }else if(mDay[i].aqi > 150 && mDay[i].aqi <= 200){
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(235,17,27);");
        }else if(mDay[i].aqi > 200 && mDay[i].aqi <= 250){
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170,0,0);");
        }else{
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110,0,0);");
        }
//        qDebug() << mDay[i].aqi;
        //更新风向，风力
        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint){
        paintHighCurve();
    }

    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint){
        paintLowCurve();
    }
    return QWidget::eventFilter(watched,event);
}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);
    //抗锯齿，美观
    painter.setRenderHint(QPainter::Antialiasing,true);

    //1、获取x坐标,x坐标为每个标签起始位置x加上宽度的一半=标签的中间位置
    int pointX[6] = {0};
    for(int i=0;i<6;i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }
    //2、获取y坐标,y坐标取5天温度的平均值，温度高了往平均值上面浮动，反之往下浮动
    int tempSum=0;
    int tempAverage=0;
    for(int i=0;i<6;i++){
        tempSum += mDay[i].high;
    }
    tempAverage = tempSum / 6;

    int pointY[6] = {0};
    int yCenter = ui->lblHighCurve->height()/2;
    for(int i=0; i<6; i++){
        if (mDay[i].high == 0) {
            pointY[i] = yCenter; // 如果温度为0，将Y坐标设置为yCenter
        } else {
            //比平均值高就提高n*3个像素，反之则降低
            pointY[i] = yCenter - ((mDay[i].high - tempAverage) * INCREMENT);
        }
    }
    //3、开始绘制
    QPen pen = painter.pen();   //画笔
    //画笔宽度和颜色
    pen.setWidth(1);
    pen.setColor(QColor(255,170,0));

    painter.setPen(pen);
    painter.setBrush(QColor(255,170,0));    //画刷--内部填充的颜色

    //画点写文本
    for(int i=0;i<6;i++){
        //绘点
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X,pointY[i]-TEXT_OFFSET_Y,QString::number(mDay[i].high)+"℃");
    }

    //画线
    for(int i=0;i<5;i++){
        if(i==0){
            pen.setStyle(Qt::DotLine);  //虚线
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);    //实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i+1],pointY[i+1]);
    }
}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);
    //抗锯齿，美观
    painter.setRenderHint(QPainter::Antialiasing,true);

    //1、获取x坐标,x坐标为每个标签起始位置x加上宽度的一半=标签的中间位置
    int pointX[6] = {0};
    for(int i=0;i<6;i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }
    //2、获取y坐标,y坐标取5天温度的平均值，温度高了往平均值上面浮动，反之往下浮动
    int tempSum=0;
    int tempAverage=0;
    for(int i=0;i<6;i++){
        tempSum += mDay[i].low;
    }
    tempAverage = tempSum / 6;

    int pointY[6] = {0};
    int yCenter = ui->lblLowCurve->height()/2;
    for(int i=0; i<6; i++){
        if (mDay[i].high == 0) {
            pointY[i] = yCenter; // 如果温度为0，将Y坐标设置为yCenter
        } else {
            pointY[i] = yCenter - ((mDay[i].low - tempAverage) * INCREMENT);
        }
    }
    //3、开始绘制
    QPen pen = painter.pen();   //画笔
    //画笔宽度和颜色
    pen.setWidth(1);
    pen.setColor(QColor(0,255,255));

    painter.setPen(pen);
    painter.setBrush(QColor(0,255,255));    //画刷--内部填充的颜色

    //画点写文本
    for(int i=0;i<6;i++){
        //绘点
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X,pointY[i]-TEXT_OFFSET_Y,QString::number(mDay[i].low)+"℃");
    }

    //画线
    for(int i=0;i<5;i++){
        if(i==0){
            pen.setStyle(Qt::DotLine);  //虚线
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);    //实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i+1],pointY[i+1]);
    }
}

void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    getWeatherInfo(cityName);
}


void MainWindow::on_leCity_returnPressed()
{
    QString cityName = ui->leCity->text();
    getWeatherInfo(cityName);
}

