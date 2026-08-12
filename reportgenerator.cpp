#include "reportgenerator.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QLabel>
ReportGenerator::ReportGenerator(QObject *parent)
    : QObject(parent), m_system(nullptr)
{
}
void ReportGenerator::setScheduleSystem(ScheduleSystem *system)
{
    m_system = system;
}
QString ReportGenerator::generateScheduleReport(const QString &course, const QString &teacher, const QString &studentClass)
{
    if (!m_system) {
        return "错误：未设置排课系统";
    }
    
    // 查询符合条件的排课记录
    QVector<Schedule> schedules = m_system->getSchedules(course, teacher, studentClass);
    if (schedules.isEmpty()) {
        return "未找到符合条件的排课记录";
    }
    
    // 按照日期排序
    std::sort(schedules.begin(), schedules.end(), [](const Schedule &s1, const Schedule &s2) {
        return s1.getDate() < s2.getDate();
    });

    // 创建HTML表格，使用与图片相匹配的样式
    QString html = "<html><head><style>";
    html += "body { font-family: SimSun, Arial, sans-serif; }"; // 使用宋体或类似字体
    html += "table { border-collapse: collapse; width: 100%; border: 1px solid black; }";
    html += "th, td { border: 1px solid black; padding: 6px; text-align: center; }";
    html += "th { font-weight: normal; }"; // 表头不加粗，与图片一致
    html += ".main-title { text-align: center; font-size: 16px; margin: 10px 0; }";
    html += ".course-table { margin-bottom: 0; }"; // 去掉课程表与排课表之间的间距
    html += ".schedule-table { margin-top: 0; }";  // 去掉排课表与课程表之间的间距
    html += "</style></head><body>";

    // 添加标题
    html += "<div class='main-title'>机房排课信息表</div>";
    
    // 添加课程信息表格
    html += "<table class='course-table'>";
    
    // 第一行
    html += "<tr>";
    html += "<td>课程名称</td>";
    html += QString("<td>%1</td>").arg(course);
    html += "<td>任课老师</td>";
    html += QString("<td>%1</td>").arg(teacher);
    html += "</tr>";
    
    // 第二行
    html += "<tr>";
    html += "<td>班级</td>";
    html += QString("<td colspan='3'>%1</td>").arg(studentClass);
    html += "</tr>";
    
    html += "</table>";
    
    // 添加排课明细表格，与第一个表格相连
    html += "<table class='schedule-table'>";
    html += "<tr>";
    html += "<td>序号</td>";
    html += "<td>日期</td>";
    html += "<td>星期</td>";
    html += "<td>午别</td>";
    html += "<td>机房</td>";
    html += "</tr>";
    
    // 添加数据行
    for (int i = 0; i < schedules.size(); i++) {
        const Schedule &schedule = schedules[i];//遍历所有符合条件的排课记录，并将每条记录插入到表格中
        html += "<tr>";
        html += QString("<td>%1</td>").arg(i + 1);
        html += QString("<td>%1</td>").arg(schedule.getDate().toString("M月d日"));
        
        // 星期几转换为中文（一、二、三、四、五、六、日）
        QString dayOfWeek = Schedule::dayOfWeekToChineseString(schedule.getDate().dayOfWeek());
        html += QString("<td>%1</td>").arg(dayOfWeek);
        
        html += QString("<td>%1</td>").arg(Schedule::timeOfDayToString(schedule.getTimeOfDay()));
        html += QString("<td>%1</td>").arg(schedule.getClassroomId());
        html += "</tr>";
    }
    
    // 添加一些空行（如图片中所示）
    for (int i = 0; i < 3; i++) {
        html += "<tr>";
        html += "<td></td><td></td><td></td><td></td><td></td>";
        html += "</tr>";
    }
    
    html += "</table></body></html>";
    
    return html;
}

QString ReportGenerator::generateStatisticsReport()
{
    if (!m_system) {
        return "错误：未设置排课系统";
    }
    
    // 获取统计数据
    QMap<QString, QMap<QString, QMap<QString, QMap<QString, int>>>> stats = m_system->getStatistics();
    if (stats.isEmpty()) {
        return "暂无排课记录";
    }
    
    // 创建HTML表格，使用与图片相匹配的样式
    QString html = "<html><head><style>";
    html += "body { font-family: SimSun, Arial, sans-serif; }"; // 使用宋体或类似字体
    html += "table { border-collapse: collapse; width: 100%; border: 1px solid black; }";
    html += "th, td { border: 1px solid black; padding: 6px; text-align: center; }";
    html += "th { font-weight: normal; }"; // 表头不加粗，与图片一致
    html += ".main-title { text-align: center; font-size: 16px; margin: 10px 0; }";
    html += "</style></head><body>";
    
    // 标题
    html += "<table>";
    html += "<tr><td colspan='5'>2024-2025 学年机房使用情况统计表</td></tr>";
    html += "<tr>";
    html += "<td>学院</td>";
    html += "<td>教员</td>";
    html += "<td>班级</td>";
    html += "<td>课程</td>";
    html += "<td>使用次数</td>";
    html += "</tr>";
    
    // 添加统计数据
    bool hasData = false;
    for (auto collegeIt = stats.begin(); collegeIt != stats.end(); ++collegeIt) {
        QString college = collegeIt.key();
        auto teacherMap = collegeIt.value();
        
        for (auto teacherIt = teacherMap.begin(); teacherIt != teacherMap.end(); ++teacherIt) {
            QString teacher = teacherIt.key();
            auto classMap = teacherIt.value();
            
            for (auto classIt = classMap.begin(); classIt != classMap.end(); ++classIt) {
                QString className = classIt.key();
                auto courseMap = classIt.value();
                
                for (auto courseIt = courseMap.begin(); courseIt != courseMap.end(); ++courseIt) {
                    QString courseName = courseIt.key();
                    int count = courseIt.value();
                    
                    html += "<tr>";
                    html += QString("<td>%1</td>").arg(college);
                    html += QString("<td>%1</td>").arg(teacher);
                    html += QString("<td>%1</td>").arg(className);
                    html += QString("<td>%1</td>").arg(courseName);
                    html += QString("<td>%1</td>").arg(count);
                    html += "</tr>";
                    hasData = true;
                }
            }
        }
    }
    
    // 如果没有数据，添加一行空行
    if (!hasData) {
        html += "<tr>";
        html += "<td></td><td></td><td></td><td></td><td></td>";
        html += "</tr>";
    }
    
    // 添加一些额外的空行（如图片中所示）
    for (int i = 0; i < 3; i++) {
        html += "<tr>";
        html += "<td></td><td></td><td></td><td></td><td></td>";
        html += "</tr>";
    }
    
    html += "</table></body></html>";
    
    return html;
} 
