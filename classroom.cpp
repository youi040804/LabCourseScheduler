#include "classroom.h"

Classroom::Classroom()
    : m_id(""), m_building(""), m_capacity(0)
{
}
//带参构造函数
Classroom::Classroom(const QString &id, const QString &building, int capacity)
    : m_id(id), m_building(building), m_capacity(capacity)
{
}

QString Classroom::getId() const
{
    return m_id;
}

void Classroom::setId(const QString &id)
{
    m_id = id;
}

QString Classroom::getBuilding() const
{
    return m_building;
}

void Classroom::setBuilding(const QString &building)
{
    m_building = building;
}

int Classroom::getCapacity() const
{
    return m_capacity;
}

void Classroom::setCapacity(int capacity)
{
    m_capacity = capacity;
}
QString Classroom::toString() const
{
    return QString("%1 - %2（容纳%3人）").arg(m_building).arg(m_id).arg(m_capacity);
} 
