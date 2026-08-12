#include "schedule.h"
//用于创建一个空的排课记录对象，初始化各个成员变量
Schedule::Schedule()
    : m_code(""), m_course(""), m_teacher(""), m_teacherCollege(""),
      m_studentClass(""), m_studentCollege(""), m_date(QDate::currentDate()),
      m_timeOfDay(TimeOfDay::Morning), m_classroomId("")
{
}

//带参构造函数：用于创建一个完整的排课记录对象，并用提供的参数初始化各个成员变量。
//这里的参数包括排课编码、课程名称、教师名称、教师学院、学生班级、学生学院、日期、时间段、教室ID。
Schedule::Schedule(const QString &code, const QString &course, const QString &teacher,
                   const QString &teacherCollege, const QString &studentClass,
                   const QString &studentCollege, const QDate &date,
                   TimeOfDay timeOfDay, const QString &classroomId)
    : m_code(code), m_course(course), m_teacher(teacher), m_teacherCollege(teacherCollege),
      m_studentClass(studentClass), m_studentCollege(studentCollege), m_date(date),
      m_timeOfDay(timeOfDay), m_classroomId(classroomId)
{
}

//每个成员变量都有对应的 getter 和 setter 方法，用于获取和设置排课记录的属性
QString Schedule::getCode() const
{
    return m_code;
}

void Schedule::setCode(const QString &code)
{
    m_code = code;
}

QString Schedule::getCourse() const
{
    return m_course;
}

void Schedule::setCourse(const QString &course)
{
    m_course = course;
}

QString Schedule::getTeacher() const
{
    return m_teacher;
}

void Schedule::setTeacher(const QString &teacher)
{
    m_teacher = teacher;
}

QString Schedule::getTeacherCollege() const
{
    return m_teacherCollege;
}

void Schedule::setTeacherCollege(const QString &teacherCollege)
{
    m_teacherCollege = teacherCollege;
}

QString Schedule::getStudentClass() const
{
    return m_studentClass;
}

void Schedule::setStudentClass(const QString &studentClass)
{
    m_studentClass = studentClass;
}

QString Schedule::getStudentCollege() const
{
    return m_studentCollege;
}

void Schedule::setStudentCollege(const QString &studentCollege)
{
    m_studentCollege = studentCollege;
}

QDate Schedule::getDate() const
{
    return m_date;
}

void Schedule::setDate(const QDate &date)
{
    m_date = date;
}

Schedule::TimeOfDay Schedule::getTimeOfDay() const
{
    return m_timeOfDay;
}

void Schedule::setTimeOfDay(TimeOfDay timeOfDay)
{
    m_timeOfDay = timeOfDay;
}

QString Schedule::getClassroomId() const
{
    return m_classroomId;
}

void Schedule::setClassroomId(const QString &classroomId)
{
    m_classroomId = classroomId;
}

//将 TimeOfDay 枚举类型转换为对应的中文字符串（"上午"、"下午"、"晚上"）
QString Schedule::timeOfDayToString(TimeOfDay timeOfDay)
{
    switch (timeOfDay) {
    case TimeOfDay::Morning:
        return "上午";
    case TimeOfDay::Afternoon:
        return "下午";
    case TimeOfDay::Evening:
        return "晚上";
    default:
        return "";
    }
}
//将 QDate::dayOfWeek() 返回的星期几（1 表示星期一，7 表示星期天）转换为中文星期的表示形式（"一"、"二"、"三" 等）
QString Schedule::dayOfWeekToChineseString(int dayOfWeek)
{
    switch (dayOfWeek) {
    case 1:
        return "一";
    case 2:
        return "二";
    case 3:
        return "三";
    case 4:
        return "四";
    case 5:
        return "五";
    case 6:
        return "六";
    case 7:
        return "日";
    default:
        return "";
    }
}
QString Schedule::toString() const
{
    return QString("%1 - %2 - %3 - %4 - %5(%6) - %7(%8)")
            .arg(m_code)
            .arg(m_course)
            .arg(m_date.toString("yyyy-MM-dd"))
            .arg(timeOfDayToString(m_timeOfDay))
            .arg(m_teacher)
            .arg(m_teacherCollege)
            .arg(m_studentClass)
            .arg(m_studentCollege);
}
//方法返回排课记录的简短状态信息。
QString Schedule::getStatusInfo() const
{
    return QString("课程: %1 | 教师: %2 | 班级: %3 | 日期: %4 | 时间: %5 | 教室: %6")
            .arg(m_course)
            .arg(m_teacher)
            .arg(m_studentClass)
            .arg(m_date.toString("yyyy年MM月dd日"))
            .arg(timeOfDayToString(m_timeOfDay))
            .arg(m_classroomId);
} 
