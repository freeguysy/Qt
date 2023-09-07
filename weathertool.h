#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include <QMap>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QDebug>

class WeatherTool{
private:
    //建立城市名字和城市代码的映射
    static QMap<QString,QString> mCityMap;

    static void initCityMap(){
        //1、读取文件
//        QString filePath = "D:/C/C++/C++Project1/citycode.json";
        QString filePath = ":/citycode.json";

        QFile file(filePath);
        //只读且以文本形式读取
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray json = file.readAll();

        file.close();

        //2、解析并写入mCityMap;
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument ::fromJson(json,&err);

        if(err.error != QJsonParseError::NoError){
            return;
        }
        if(!doc.isArray()){
            return;
        }
        QJsonArray citys =  doc.array();
        for(int i=0;i<citys.size();i++){
            QString city =  citys[i].toObject().value("city_name").toString();
            QString code =  citys[i].toObject().value("city_code").toString();
            if(code.size() > 0)
                mCityMap.insert(city,code);
        }
    }
public:
    static QString getCityCode(QString cityName){
        if(mCityMap.isEmpty()){
            initCityMap();
        }

        QMap<QString,QString>::iterator it = mCityMap.find(cityName);
        // 北京、北京市均可
        if(it == mCityMap.end()){
            it =mCityMap.find(cityName + "市");
        }

        if(it != mCityMap.end()){
            return it.value();
        }
        return "";
    }
};

QMap<QString,QString> WeatherTool::mCityMap = {};
#endif // WEATHERTOOL_H
