#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QString>
#include <QDate>

// 排课类，用于表示一次具体的排课记录。它包含了排课的各个属性，如课程、教师、学生班级、时间等。
class Schedule
{
public:
    // 午别枚举类型
    enum class TimeOfDay {
        Morning,    // 上午
        Afternoon,  // 下午
        Evening     // 晚上
    };
    
    // 构造函数
    Schedule();
    Schedule(const QString &code, const QString &course, const QString &teacher,
             const QString &teacherCollege, const QString &studentClass, 
             const QString &studentCollege, const QDate &date,
             TimeOfDay timeOfDay, const QString &classroomId);
    
    // getter和setter方法
    QString getCode() const;
    void setCode(const QString &code);
    
    QString getCourse() const;
    void setCourse(const QString &course);
    
    QString getTeacher() const;
    void setTeacher(const QString &teacher);
    
    QString getTeacherCollege() const;
    void setTeacherCollege(const QString &teacherCollege);
    
    QString getStudentClass() const;
    void setStudentClass(const QString &studentClass);
    
    QString getStudentCollege() const;
    void setStudentCollege(const QString &studentCollege);
    
    QDate getDate() const;
    void setDate(const QDate &date);
    
    TimeOfDay getTimeOfDay() const;
    void setTimeOfDay(TimeOfDay timeOfDay);
    
    QString getClassroomId() const;
    void setClassroomId(const QString &classroomId);
    
    // 辅助方法（用于格式化时间和日期）
    static QString timeOfDayToString(TimeOfDay timeOfDay);
    static QString dayOfWeekToChineseString(int dayOfWeek);
    
    // 排课信息格式化输出
    QString toString() const;
    QString getStatusInfo() const;
    
private:
    QString m_code;            // 排课编码
    QString m_course;          // 课程名称
    QString m_teacher;         // 任课教师
    QString m_teacherCollege;  // 教师所在学院
    QString m_studentClass;    // 学生班级
    QString m_studentCollege;  // 学生所在学院
    QDate m_date;              // 日期
    TimeOfDay m_timeOfDay;     // 午别
    QString m_classroomId;     // 教室ID
};

#endif // SCHEDULE_H 
