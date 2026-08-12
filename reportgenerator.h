#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include "schedulesystem.h"
#include "schedule.h"

// 报表生成器类，负责生成排课报表和统计表
class ReportGenerator : public QObject
{
    Q_OBJECT
    
public:
    //构造函数
    explicit ReportGenerator(QObject *parent = nullptr);
    
    // 设置排课系统
    void setScheduleSystem(ScheduleSystem *system);
    
    // 根据课程、教师和班级生成排课报表
    QString generateScheduleReport(const QString &course, const QString &teacher, const QString &studentClass);
    
    // 生成机房使用统计表
    QString generateStatisticsReport();
    
private:
    ScheduleSystem *m_system;//指向 ScheduleSystem 类的指针。它用于保存排课系统对象的引用

};

#endif // REPORTGENERATOR_H 
