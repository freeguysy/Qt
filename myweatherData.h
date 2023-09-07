#ifndef MYWEATHERDATA_H
#define MYWEATHERDATA_H

class Today
{
public:
    Today(){
        date = "2023-8-13";
        city="广州";

        ganmao="感冒指数";

        wendu = 0;
        shidu = "0%";
        pm25 = 0;
        quality = "无数据";

        type="多云";
        fl = "2级";
        fx = "南风";

        high = 30;
        low = 18;
    }
    QString date;
    QString city;

    QString ganmao;

    int wendu;
    QString shidu;
    int pm25;
    QString quality;

    QString type;

    QString fl;
    QString fx;

    int high;
    int low;

};

class Day
{
public:
    Day(){
        date = "2023-8-13";
        week = "周五";
        type = "多云";

        high = 0;
        low = 0;

        fx = "南风";
        fl = "2级";

        aqi = 0;
    }
    QString date;
    QString week;
    QString type;

    int high;
    int low;

    QString fl;
    QString fx;

    int aqi;
};

#endif // MYWEATHERDATA_H
