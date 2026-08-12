#include "scheduletable.h"
#include <QHeaderView>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>

ScheduleTable::ScheduleTable(QWidget *parent)
    : QTableWidget(parent), m_system(nullptr), m_totalScheduleCount(0)
//m_system 是指向排课系统的指针，m_totalScheduleCount 是排课次数（初始化为 0）
{
    initializeTable();
    //连接信号和槽，当用户点击表格单元格时，触发 onCellClicked 槽函数。
    connect(this, &QTableWidget::cellClicked, this, &ScheduleTable::onCellClicked);
}

ScheduleTable::~ScheduleTable()
{
}

void ScheduleTable::setScheduleSystem(ScheduleSystem *system)
{
    m_system = system;
    updateTable();
}

void ScheduleTable::updateTable()
{
    //如果排课系统或选中的教室为空，直接返回
    if (!m_system || m_selectedClassroomId.isEmpty()) {
        return;
    }
    
    resetCellBackground();
    m_cellScheduleMap.clear();

    // 获取所有排课记录
    QVector<Schedule> schedules = m_system->getSchedules();
    
    // 将排课记录映射到对应的单元格
    for (const Schedule &schedule : schedules) {
        if (schedule.getClassroomId() == m_selectedClassroomId) {
            QPair<int, int> cell = getDateTimeCell(schedule.getDate(), schedule.getTimeOfDay());

            if (cell.first >= 0 && cell.first < rowCount() &&
                cell.second >= 0 && cell.second < columnCount()) {
                m_cellScheduleMap[cell] = schedule;
                

                QTableWidgetItem *item = this->item(cell.first, cell.second);
                if (item) {
                    // 判断该排课记录是否是当前正在排课的课程
                    bool isCurrentCourse = (schedule.getCourse() == m_currentCourse && 
                                          schedule.getTeacher() == m_currentTeacher &&
                                          schedule.getStudentClass() == m_currentStudentClass);
                    

                    item->setText(schedule.getCode());
                    
                    if (isCurrentCourse) {
                        item->setForeground(Qt::red);
                    } else {
                        item->setForeground(Qt::black);
                    }

                    item->setBackground(Qt::lightGray);
                }
            }
        }
    }
    
    // 发送剩余排课次数变化信号
    emit remainingSchedulesChanged(getRemainingSchedules());
}

void ScheduleTable::setSelectedBuilding(const QString &building)
{
    //将传入的 building（教学楼名称）赋值给 m_selectedBuilding 成员变量
    m_selectedBuilding = building;
}

void ScheduleTable::setSelectedClassroomId(const QString &classroomId)
{
    //将传入的 classroomId（教室 ID）赋值给 m_selectedClassroomId 成员变量。
    m_selectedClassroomId = classroomId;
    updateTable();
}

void ScheduleTable::setCurrentScheduleInfo(const QString &course, const QString &teacher, 
                                      const QString &teacherCollege,
                                      const QString &studentClass, 
                                      const QString &studentCollege,
                                      int totalCount)
{
    //将传入的排课信息（课程、教师、学院、班级等）保存到相应的成员变量中。
    m_currentCourse = course;
    m_currentTeacher = teacher;
    m_currentTeacherCollege = teacherCollege;
    m_currentStudentClass = studentClass;
    m_currentStudentCollege = studentCollege;
    m_totalScheduleCount = totalCount;
    
    // 发送剩余排课次数变化信号
    emit remainingSchedulesChanged(getRemainingSchedules());
}

int ScheduleTable::getRemainingSchedules() const
{
    if (!m_system || m_currentCourse.isEmpty() || m_currentTeacher.isEmpty() || m_currentStudentClass.isEmpty()) {
        return 0;
    }
    //调用排课系统的 countRemainingSchedules() 方法，传入当前课程、教师、班级以及总的排课次数，返回剩余的排课次数
    return m_system->countRemainingSchedules(m_currentCourse, m_currentTeacher, 
                                            m_currentStudentClass, m_totalScheduleCount);
}

void ScheduleTable::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        // 右键点击，取消排课
        QPoint pos = event->pos();// 获取鼠标点击位置
        QTableWidgetItem *item = itemAt(pos);// 获取点击位置的单元格
        if (item) {
            int row = item->row();
            int column = item->column();
            QPair<int, int> cell(row, column); // 将行列信息存储在 QPair 中
            
            if (m_cellScheduleMap.contains(cell)) {  // 如果该单元格已经有排课
                Schedule schedule = m_cellScheduleMap[cell];// 获取该单元格对应的排课记录
                


                if (schedule.getCourse() == m_currentCourse &&
                    schedule.getTeacher() == m_currentTeacher &&
                    schedule.getStudentClass() == m_currentStudentClass)
                {

                    // 取消排课
                    if (m_system->removeSchedule(schedule.getCode())) {
                        // 更新表格
                        updateTable();
                        emit scheduleChanged();
                    }
                }

                else {
                    QMessageBox::warning(this, "警告", "只能取消当前课程的排课！");
                }


            }
        }
    }
    
    QTableWidget::mousePressEvent(event);
}

void ScheduleTable::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 左键双击，进行排课
        QPoint pos = event->pos();
        QTableWidgetItem *item = itemAt(pos);
        if (item) {
            int row = item->row();
            int column = item->column();
            QPair<int, int> cell(row, column);
            
            // 检查是否已经排课
            if (m_cellScheduleMap.contains(cell)) {
                QMessageBox::information(this, "提示", "该时间段已经排课！");
                return;
            }
            
            // 检查是否有有效的排课信息
            if (m_currentCourse.isEmpty() || m_currentTeacher.isEmpty() || m_currentStudentClass.isEmpty()) {
                QMessageBox::warning(this, "警告", "请先设置排课信息！");
                return;
            }
            
            // 检查是否还有剩余排课次数
            if (getRemainingSchedules() <= 0) {
                QMessageBox::warning(this, "警告", "已达到排课次数上限！");
                return;
            }
            
            // 获取日期和时间段
            QPair<QDate, Schedule::TimeOfDay> dateTime = getCellDateTime(row, column);
            
            // 获取或创建排课编码（同一课程-教师-班级共用相同的排课码）
            QString code = m_system->getOrCreateCourseCode(
                m_currentCourse, m_currentTeacher, m_currentStudentClass
            );
            
            // 创建排课记录
            Schedule schedule(
                code, m_currentCourse, m_currentTeacher, m_currentTeacherCollege,
                m_currentStudentClass, m_currentStudentCollege, dateTime.first,
                dateTime.second, m_selectedClassroomId
            );
            
            // 添加排课记录
            if (m_system->addSchedule(schedule)) {
                // 更新表格
                updateTable();
                emit scheduleChanged();
            } else {
                QMessageBox::warning(this, "警告", "排课失败！");
            }
        }
    }
    
    QTableWidget::mouseDoubleClickEvent(event);
}

void ScheduleTable::onCellClicked(int row, int column)
{
    QPair<int, int> cell(row, column);
 //检查 m_cellScheduleMap（一个映射，存储了每个单元格对应的排课记录）中是否存在该单元格（cell）的排课记录。
    if (m_cellScheduleMap.contains(cell)) {
        Schedule schedule = m_cellScheduleMap[cell];// 获取该单元格对应的排课记录
        emit scheduleClicked(schedule);// 发送排课记录
    }
}

void ScheduleTable::initializeTable()
{
    // 设置表格为7行（周一至周日）, 15列（5周 x 3个时间段）
    setRowCount(7);
    setColumnCount(15);
    
    // 设置水平表头
    QStringList hHeaders;
    // 指定第一周的开始日期为2月17日
    QDate firstWeekStart = QDate(QDate::currentDate().year(), 2, 17);
    for (int week = 0; week < 5; week++) {
        for (int timeSlot = 0; timeSlot < 3; timeSlot++) {
            QString timeStr;
            switch (timeSlot) {
            case 0:
                timeStr = "上午";
                break;
            case 1:
                timeStr = "下午";
                break;
            case 2:
                timeStr = "晚上";
                break;
            }
            
            QDate weekStartDate = firstWeekStart.addDays(week * 7);
            hHeaders << QString("第%1周%2\n(%3)").arg(week + 1).arg(timeStr).arg(weekStartDate.toString("MM.dd"));
        }
    }
    setHorizontalHeaderLabels(hHeaders);
    
    // 设置垂直表头
    QStringList vHeaders;
    QStringList daysOfWeek = {"周一", "周二", "周三", "周四", "周五", "周六", "周日"};
    vHeaders = daysOfWeek;
    setVerticalHeaderLabels(vHeaders);
    
    // 调整行高和列宽
    for (int i = 0; i < rowCount(); i++) {
        setRowHeight(i, 40);
    }
    
    for (int i = 0; i < columnCount(); i++) {
        setColumnWidth(i, 100);
    }
    
    // 初始化单元格
    for (int row = 0; row < rowCount(); row++) {
        for (int col = 0; col < columnCount(); col++) {
            QTableWidgetItem *item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignCenter);//文本居中对齐
            setItem(row, col, item);
        }
    }
    
    // 设置表格样式
    setFrameShape(QFrame::Box);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setShowGrid(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
//用于高亮显示排课表格中的某个时间段单元格
void ScheduleTable::highlightTimeSlot(int row, int column)
{
    resetCellBackground();
    
    if (row >= 0 && row < rowCount() && column >= 0 && column < columnCount()) {
        item(row, column)->setBackground(Qt::yellow);
    }
}

void ScheduleTable::resetCellBackground()
{
    for (int row = 0; row < rowCount(); row++) {
        for (int col = 0; col < columnCount(); col++) {
            QPair<int, int> cell(row, col);
            if (m_cellScheduleMap.contains(cell)) {// 如果该单元格有排课记录
                Schedule schedule = m_cellScheduleMap[cell];// 获取排课记录
                QTableWidgetItem *item = this->item(row, col);
                
                // 检查是否为当前排课课程
                bool isCurrentCourse = (schedule.getCourse() == m_currentCourse && 
                                      schedule.getTeacher() == m_currentTeacher &&
                                      schedule.getStudentClass() == m_currentStudentClass);
                
                // 设置文本为排课码
                item->setText(schedule.getCode());
                
                // 设置背景和文字颜色
                if (isCurrentCourse) {
                    // 当前排课使用红色文字
                    item->setForeground(Qt::red);
                } else {
                    // 其他排课使用黑色文字
                    item->setForeground(Qt::black);
                }
                
                // 设置背景颜色为浅灰色
                item->setBackground(Qt::lightGray);
            } else {
                QTableWidgetItem *item = this->item(row, col);
                item->setBackground(Qt::white);// 如果该单元格没有排课记录，设置背景为白色
                item->setText("");// 如果该单元格没有排课记录，设置背景为白色
                item->setForeground(Qt::black);// 设置文字为黑色
            }
        }
    }
}
//根据表格中的行和列计算出对应的日期和时间段
QPair<QDate, Schedule::TimeOfDay> ScheduleTable::getCellDateTime(int row, int column) const
{
    // 第一周的开始日期为2月17日（周一）
    QDate firstWeekStart = QDate(QDate::currentDate().year(), 2, 17);
    
    // 计算周数（0,1,2,3,4）和时间段（0=上午,1=下午,2=晚上）
    int week = column / 3;
    int timeSlot = column % 3;
    
    // 计算单元格对应的时间段
    Schedule::TimeOfDay cellTimeOfDay;
    switch (timeSlot) {
    case 0:
        cellTimeOfDay = Schedule::TimeOfDay::Morning;
        break;
    case 1:
        cellTimeOfDay = Schedule::TimeOfDay::Afternoon;
        break;
    case 2:
        cellTimeOfDay = Schedule::TimeOfDay::Evening;
        break;
    default:
        cellTimeOfDay = Schedule::TimeOfDay::Morning;
    }
    
    // 计算日期：第一周开始日期 + 周数偏移 + 行数（天数偏移）
    QDate cellDate = firstWeekStart.addDays(week * 7 + row);
    
    return QPair<QDate, Schedule::TimeOfDay>(cellDate, cellTimeOfDay);
}
//根据给定的日期和时间段，计算该时间段对应的单元格位置（行列号）
QPair<int, int> ScheduleTable::getDateTimeCell(const QDate &date, Schedule::TimeOfDay timeOfDay) const
{
    // 第一周的开始日期为2月17日（周一）
    QDate firstWeekStart = QDate(QDate::currentDate().year(), 2, 17);
    
    // 如果日期早于第一周开始日期，则返回无效单元格
    if (date < firstWeekStart) {
        return QPair<int, int>(-1, -1);
    }
    
    // 计算行索引 (0-6，对应周一到周日)
    int row = date.dayOfWeek() - 1; // Qt中1是周一，7是周日
    
    // 计算日期与第一周周一的天数差
    int daysDiff = firstWeekStart.daysTo(date);
    
    // 计算周数偏移
    int weekOffset = daysDiff / 7;
    
    // 计算时间段偏移
    int timeSlot;
    switch (timeOfDay) {
    case Schedule::TimeOfDay::Morning:
        timeSlot = 0;
        break;
    case Schedule::TimeOfDay::Afternoon:
        timeSlot = 1;
        break;
    case Schedule::TimeOfDay::Evening:
        timeSlot = 2;
        break;
    default:
        timeSlot = 0;
    }
    
    // 计算列索引 (weekOffset * 3 + timeSlot)
    int column = weekOffset * 3 + timeSlot;
    
    // 检查是否在表格范围内
    if (column < 0 || column >= columnCount() || row < 0 || row >= rowCount()) {
        return QPair<int, int>(-1, -1);
    }
    
    return QPair<int, int>(row, column);
} 
