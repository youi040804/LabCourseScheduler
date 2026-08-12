#ifndef CLASSROOM_H
#define CLASSROOM_H

#include <QString>
#include <QVector>

// 教室类，表示一个机房
class Classroom
{
public:
    // 构造函数
    Classroom();
    Classroom(const QString &id, const QString &building, int capacity);
    
    // getter和setter方法
    QString getId() const;
    void setId(const QString &id);
    
    QString getBuilding() const;
    void setBuilding(const QString &building);
    
    int getCapacity() const;
    void setCapacity(int capacity);
    
    // 教室信息格式化输出
    QString toString() const;
    
private:
    QString m_id;          // 教室编号
    QString m_building;    // 所在教学楼
    int m_capacity;        // 容纳人数
};

#endif // CLASSROOM_H 
