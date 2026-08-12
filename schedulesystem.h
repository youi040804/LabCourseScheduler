#ifndef SCHEDULESYSTEM_H
#define SCHEDULESYSTEM_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QDate>
#include "classroom.h"
#include "schedule.h"

// 排课系统类，负责排课系统的核心业务逻辑
class ScheduleSystem : public QObject
{
    Q_OBJECT
    
public:
    explicit ScheduleSystem(QObject *parent = nullptr);
    ~ScheduleSystem();
    
    // 初始化，加载预设的教室信息
    void initialize();
    
    // 教室相关方法
    QVector<Classroom> getClassrooms() const;
    QVector<QString> getClassroomBuildings() const;
    QVector<QString> getClassroomIds(const QString &building) const;
    Classroom getClassroom(const QString &id) const;
    
    // 排课相关方法
    QVector<Schedule> getSchedules() const;
    QVector<Schedule> getSchedules(const QString &course, const QString &teacher, 
                                const QString &studentClass) const;//
    bool isTimeSlotAvailable(const QString &classroomId, const QDate &date, 
                          Schedule::TimeOfDay timeOfDay) const;
    QString generateScheduleCode();

    QString getOrCreateCourseCode(const QString &course, const QString &teacher, const QString &studentClass);
    bool addSchedule(const Schedule &schedule);
    bool removeSchedule(const QString &code);
    int countRemainingSchedules(const QString &course, const QString &teacher,
                               const QString &studentClass, int totalCount) const;
    
    // 查询相关方法
    //用于获取所有教师、课程、学生班级、教师学院、学生学院的名称
    QVector<QString> getAllTeachers() const;
    QVector<QString> getAllCourses() const;
    QVector<QString> getAllStudentClasses() const;
    QVector<QString> getAllTeacherColleges() const;
    QVector<QString> getAllStudentColleges() const;
    
    // 报表和统计相关方法
    QMap<QString, QMap<QString, QMap<QString, QMap<QString, int>>>> getStatistics() const;
    
private:
    QVector<Classroom> m_classrooms;     // 教室列表
    QVector<Schedule> m_schedules;       // 排课记录列表
    int m_lastScheduleCodeNumber;        // 用于生成唯一的排课编号
    QMap<QString, QString> m_courseCodes;  // 用于存储课程-教师-班级与排课码的映射关系
    
    // 辅助方法，从文件加载数据和保存数据到文件
    void loadDataFromFile();
    void saveDataToFile() const;
};

#endif // SCHEDULESYSTEM_H 
