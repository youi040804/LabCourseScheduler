#ifndef SCHEDULETABLE_H
#define SCHEDULETABLE_H

#include <QTableWidget>
#include <QDate>
#include <QMap>
#include "schedulesystem.h"
#include "schedule.h"

// 排课表类，用于显示和管理排课表UI
class ScheduleTable : public QTableWidget
{
    Q_OBJECT
    
public:
    explicit ScheduleTable(QWidget *parent = nullptr);
    ~ScheduleTable();
    
    // 设置排课系统设置排课系统对象
    void setScheduleSystem(ScheduleSystem *system);
    
    // 更新排课表显示
    void updateTable();
    
    // 设置当前选中的教学楼和教室
    void setSelectedBuilding(const QString &building);
    void setSelectedClassroomId(const QString &classroomId);
    
    //设置当前排课的详细信息，包括课程、教师、教师学院、学生班级、学生学院和排课次数
    void setCurrentScheduleInfo(const QString &course, const QString &teacher,
                             const QString &teacherCollege, const QString &studentClass,
                             const QString &studentCollege, int totalCount);
    
    // 获取剩余排课次数
    int getRemainingSchedules() const;
    
signals:
    // 当点击已排课的单元格时，发送信号
    void scheduleClicked(const Schedule &schedule);
    
    // 当排课信息变更时，发送信号
    void scheduleChanged();
    
    // 当剩余排课次数变化时，发送信号
    void remainingSchedulesChanged(int remaining);
    
protected:
    // 处理鼠标事件
    void mousePressEvent(QMouseEvent *event) override;//单击进行排课
    void mouseDoubleClickEvent(QMouseEvent *event) override;//双击取消排课
    
private slots:
    // 处理单元格点击事件
    void onCellClicked(int row, int column);
    
private:
    ScheduleSystem *m_system;           // 排课系统
    QString m_selectedBuilding;         // 选中的教学楼
    QString m_selectedClassroomId;      // 选中的教室ID
    QMap<QPair<int, int>, Schedule> m_cellScheduleMap;  // 单元格到排课记录的映射
    
    // 当前排课信息
    QString m_currentCourse;
    QString m_currentTeacher;
    QString m_currentTeacherCollege;
    QString m_currentStudentClass;
    QString m_currentStudentCollege;
    int m_totalScheduleCount;
    
    // 辅助方法
    void initializeTable();
    void highlightTimeSlot(int row, int column);//高亮排课表单元格
    void resetCellBackground();
    //getCellDateTime() 和 getDateTimeCell() 做行列和日期+午别的双向映射。
    QPair<QDate, Schedule::TimeOfDay> getCellDateTime(int row, int column) const;
    QPair<int, int> getDateTimeCell(const QDate &date, Schedule::TimeOfDay timeOfDay) const;
};

#endif // SCHEDULETABLE_H 
