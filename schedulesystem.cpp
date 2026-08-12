#include "schedulesystem.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QDebug>

ScheduleSystem::ScheduleSystem(QObject *parent)
    : QObject(parent), m_lastScheduleCodeNumber(0)
{
    initialize();//调用初始化方法，初始化系统
    loadDataFromFile();//从文件中加载排课数据
}

ScheduleSystem::~ScheduleSystem()
{
    saveDataToFile();//将当前的排课数据保存到文件
}

void ScheduleSystem::initialize()
{
    m_classrooms.clear();
    
    //初始化教室信息（教室ID、所在楼栋和容量）
    // 北区综合楼机房
    m_classrooms.append(Classroom("310", "北区综合楼", 40));
    m_classrooms.append(Classroom("311", "北区综合楼", 45));
    m_classrooms.append(Classroom("312", "北区综合楼", 50));
    m_classrooms.append(Classroom("417", "北区综合楼", 60));
    m_classrooms.append(Classroom("418", "北区综合楼", 65));
    
    // 西区信息楼机房（展示空机房）
    m_classrooms.append(Classroom("201", "西区信息楼", 50));
    m_classrooms.append(Classroom("202", "西区信息楼", 55));
    m_classrooms.append(Classroom("301", "西区信息楼", 60));
    m_classrooms.append(Classroom("302", "西区信息楼", 65));
    
    // 未来城综合楼机房（展示空机房）
    m_classrooms.append(Classroom("101", "未来城综合楼", 80));
    m_classrooms.append(Classroom("102", "未来城综合楼", 100));
    m_classrooms.append(Classroom("201", "未来城综合楼", 90));
    m_classrooms.append(Classroom("202", "未来城综合楼", 120));
}
//返回所有教室
QVector<Classroom> ScheduleSystem::getClassrooms() const
{
    return m_classrooms;
}
//返回所有教学楼的名称
QVector<QString> ScheduleSystem::getClassroomBuildings() const
{
    QSet<QString> buildingSet;
    for (const Classroom &classroom : m_classrooms) {
        buildingSet.insert(classroom.getBuilding());
    }
    
    return QVector<QString>(buildingSet.begin(), buildingSet.end());
}
//根据指定教学楼名称返回该楼的所有教室ID
QVector<QString> ScheduleSystem::getClassroomIds(const QString &building) const
{
    QVector<QString> ids;
    for (const Classroom &classroom : m_classrooms) {
        if (classroom.getBuilding() == building) {
            ids.append(classroom.getId());
        }
    }
    return ids;
}
//根据教室ID返回具体的教室对象
Classroom ScheduleSystem::getClassroom(const QString &id) const
{
    for (const Classroom &classroom : m_classrooms) {
        if (classroom.getId() == id) {
            return classroom;
        }
    }
    return Classroom();
}
//返回所有排课记录
QVector<Schedule> ScheduleSystem::getSchedules() const
{
    return m_schedules;
}

QVector<Schedule> ScheduleSystem::getSchedules(const QString &course, const QString &teacher, 
                                           const QString &studentClass) const
{
    QVector<Schedule> result;
    for (const Schedule &schedule : m_schedules) {
        bool match = true;
        if (!course.isEmpty() && schedule.getCourse() != course) {
            match = false;
        }
        if (!teacher.isEmpty() && schedule.getTeacher() != teacher) {
            match = false;
        }
        if (!studentClass.isEmpty() && schedule.getStudentClass() != studentClass) {
            match = false;
        }
        if (match) {
            result.append(schedule);
        }
    }
    return result;
}
//检查给定的教室、日期和时间段是否已经被排课占用
bool ScheduleSystem::isTimeSlotAvailable(const QString &classroomId, const QDate &date, 
                                      Schedule::TimeOfDay timeOfDay) const
{
    for (const Schedule &schedule : m_schedules) {
        if (schedule.getClassroomId() == classroomId &&
            schedule.getDate() == date &&
            schedule.getTimeOfDay() == timeOfDay) {
            return false;
        }
    }
    return true;
}
//生成排课编码
QString ScheduleSystem::generateScheduleCode()
{
    m_lastScheduleCodeNumber++;//m_lastScheduleCodeNumber 记录上次生成的排课编码的数字部分
    return QString("C%1").arg(m_lastScheduleCodeNumber, 3, 10, QChar('0'));
}

//将一个新的排课记录添加到排课系统中。
bool ScheduleSystem::addSchedule(const Schedule &schedule)
{
    // 检查是否有冲突
    if (!isTimeSlotAvailable(schedule.getClassroomId(), schedule.getDate(), schedule.getTimeOfDay())) {
        return false;
    }
    
    m_schedules.append(schedule);// 将排课记录添加到排课系统
    saveDataToFile();
    return true;
}
//删除排课记录
bool ScheduleSystem::removeSchedule(const QString &code)
{
    for (int i = 0; i < m_schedules.size(); i++) {
        if (m_schedules[i].getCode() == code) {
            m_schedules.remove(i);//当找到与传入的 code 匹配的排课记录时，使用 remove(i) 方法从 m_schedules
            //列表中删除该记录。remove(i) 方法会删除索引为 i 的元素（即排课记录），并自动调整列表的大小
            saveDataToFile();
            return true;
        }
    }
    return false;
}
//计算排课剩余次数
int ScheduleSystem::countRemainingSchedules(const QString &course, const QString &teacher,
                                         const QString &studentClass, int totalCount) const
{
    int scheduledCount = getSchedules(course, teacher, studentClass).size();
    return totalCount - scheduledCount;
}


//获取所有不同的教师名称
QVector<QString> ScheduleSystem::getAllTeachers() const
{
    QSet<QString> teacherSet;
    for (const Schedule &schedule : m_schedules) {
        teacherSet.insert(schedule.getTeacher());
    }
   return QVector<QString>(teacherSet.begin(), teacherSet.end());
}
//获取系统中所有不同的课程名称
QVector<QString> ScheduleSystem::getAllCourses() const
{
    QSet<QString> courseSet;
    for (const Schedule &schedule : m_schedules) {
        courseSet.insert(schedule.getCourse());
    }
    return QVector<QString>(courseSet.begin(), courseSet.end());
}
//获取系统中所有不同的学生班级
QVector<QString> ScheduleSystem::getAllStudentClasses() const
{
    QSet<QString> classSet;
    for (const Schedule &schedule : m_schedules) {
        classSet.insert(schedule.getStudentClass());
    }
    return QVector<QString>(classSet.begin(), classSet.end());
}
//获取所有不同的教师学院名称
QVector<QString> ScheduleSystem::getAllTeacherColleges() const
{
    QSet<QString> collegeSet;
    for (const Schedule &schedule : m_schedules) {
        collegeSet.insert(schedule.getTeacherCollege());
    }
    return QVector<QString>(collegeSet.begin(), collegeSet.end());
}
//获取系统中所有不同的学生班级
QVector<QString> ScheduleSystem::getAllStudentColleges() const
{
    QSet<QString> collegeSet;
    for (const Schedule &schedule : m_schedules) {
        collegeSet.insert(schedule.getStudentCollege());
    }
    return QVector<QString>(collegeSet.begin(), collegeSet.end());
}
//使用了一个 四层嵌套的 QMap 来存储机房使用情况
QMap<QString, QMap<QString, QMap<QString, QMap<QString, int>>>> ScheduleSystem::getStatistics() const
{
    // 统计机房使用情况：教师学院 -> 老师 -> 班级 -> 课程 -> 使用次数
    QMap<QString, QMap<QString, QMap<QString, QMap<QString, int>>>> stats;
    
    for (const Schedule &schedule : m_schedules) {
        QString college = schedule.getTeacherCollege();
        QString teacher = schedule.getTeacher();
        QString studentClass = schedule.getStudentClass();
        QString course = schedule.getCourse();

        stats[college][teacher][studentClass][course]++;
    }
    
    return stats;
}

//根据课程、教师和班级的组合生成一个唯一的排课编码（course code）
QString ScheduleSystem::getOrCreateCourseCode(const QString &course, const QString &teacher, const QString &studentClass)
{

    QString key = course + "|" + teacher + "|" + studentClass;
    
    // 检查是否已有对应的排课码
    if (m_courseCodes.contains(key)) {
        return m_courseCodes[key];
    }
    
    // 没有已存在的排课码，创建一个新的
    QString code = generateScheduleCode();
    m_courseCodes[key] = code;

    
    return code;
}

//从文件中读取排课数据（例如排课记录、课程码、排课编号等）并加载到内存中的成员变量中
void ScheduleSystem::loadDataFromFile()
{
    QFile file("schedules.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件以读取数据";
        return;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();
    
    // 读取排课编号计数器
    m_lastScheduleCodeNumber = root["lastScheduleCode"].toInt();
    
    // 读取课程码映射
    m_courseCodes.clear();
    if (root.contains("courseCodes")) {
        QJsonObject codeMap = root["courseCodes"].toObject();
        for (auto it = codeMap.begin(); it != codeMap.end(); ++it) {
            m_courseCodes[it.key()] = it.value().toString();
        }
    }
    
    // 读取排课记录
    QJsonArray schedulesArray = root["schedules"].toArray();
    m_schedules.clear();

    for (const QJsonValue &value : schedulesArray) {
        QJsonObject obj = value.toObject();
        
        QString code = obj["code"].toString();
        QString course = obj["course"].toString();
        QString teacher = obj["teacher"].toString();
        QString teacherCollege = obj["teacherCollege"].toString();
        QString studentClass = obj["studentClass"].toString();
        QString studentCollege = obj["studentCollege"].toString();
        QDate date = QDate::fromString(obj["date"].toString(), "yyyy-MM-dd");
        int timeOfDayInt = obj["timeOfDay"].toInt();
        Schedule::TimeOfDay timeOfDay = static_cast<Schedule::TimeOfDay>(timeOfDayInt);
        QString classroomId = obj["classroomId"].toString();
        
        // 确保课程码映射存在
        QString key = course + "|" + teacher + "|" + studentClass;
        if (!m_courseCodes.contains(key)) {
            m_courseCodes[key] = code;
        }
        
        m_schedules.append(Schedule(
            code, course, teacher, teacherCollege, studentClass,
            studentCollege, date, timeOfDay, classroomId
        ));
    }
    
    file.close();
}

void ScheduleSystem::saveDataToFile() const
{
    QFile file("schedules.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开文件以保存数据";
        return;
    }
    
    QJsonObject root;
    root["lastScheduleCode"] = m_lastScheduleCodeNumber;
    
    // 保存课程码映射
    QJsonObject codeMap;
    for (auto it = m_courseCodes.begin(); it != m_courseCodes.end(); ++it) {
        codeMap[it.key()] = it.value();
    }
    root["courseCodes"] = codeMap;
    
    QJsonArray schedulesArray;
    for (const Schedule &schedule : m_schedules) {
        QJsonObject obj;
        obj["code"] = schedule.getCode();
        obj["course"] = schedule.getCourse();
        obj["teacher"] = schedule.getTeacher();
        obj["teacherCollege"] = schedule.getTeacherCollege();
        obj["studentClass"] = schedule.getStudentClass();
        obj["studentCollege"] = schedule.getStudentCollege();
        obj["date"] = schedule.getDate().toString("yyyy-MM-dd");
        obj["timeOfDay"] = static_cast<int>(schedule.getTimeOfDay());
        obj["classroomId"] = schedule.getClassroomId();
        
        schedulesArray.append(obj);
    }
    
    root["schedules"] = schedulesArray;
    
    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
} 
