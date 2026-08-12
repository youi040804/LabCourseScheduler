#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QPushButton>

#include "schedulesystem.h"
#include "scheduletable.h"
#include "reportgenerator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //构造函数，用于初始化主窗口。parent 是父窗口，默认值为 nullptr，表示没有父窗口。
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 浏览功能相关槽函数
    void onBuildingChanged(const QString &building);
    void onClassroomChanged(const QString &classroom);
    void onScheduleClicked(const Schedule &schedule);
    
    // 排课功能相关槽函数
    void onStartScheduling();
    void onRemainingSchedulesChanged(int remaining);
    
    // 报表功能相关槽函数
    void onGenerateReport();
    
    // 统计功能相关槽函数
    void onGenerateStatistics();
    
    // 界面切换相关槽函数
    void onTabChanged(int index);

private:
    Ui::MainWindow *ui;
    
    // 排课系统核心
    ScheduleSystem *m_scheduleSystem;
    
    // 报表生成器
    ReportGenerator *m_reportGenerator;
    
    // UI组件
    QTabWidget *m_tabWidget;

    // 浏览功能相关UI组件
    QComboBox *m_buildingComboBox;// 楼栋选择框
    QComboBox *m_classroomComboBox;//教室选择框
    ScheduleTable *m_scheduleTable;//排课表格
    QLabel *m_statusLabel;//状态标签


    // 排课功能相关UI组件
    QComboBox *m_courseComboBox;
    QComboBox *m_teacherComboBox;
    QComboBox *m_studentClassComboBox;
    //教师所在学院和学生所在学院的输入框。
    QLineEdit *m_teacherCollegeLineEdit;
    QLineEdit *m_studentCollegeLineEdit;

    QSpinBox *m_scheduleCountSpinBox; //用于设置排课次数。
    QPushButton *m_startSchedulingButton;  //用于开始排课
    QLabel *m_remainingScheduleLabel;//用于显示剩余排课次数。
    
    // 报表功能相关UI组件
    QComboBox *m_reportCourseComboBox;
    QComboBox *m_reportTeacherComboBox;
    QComboBox *m_reportStudentClassComboBox;

    QPushButton *m_generateReportButton;//用于触发报表生成操作。
    QTextEdit *m_reportTextEdit;//用于显示生成的排课报表
    
    // 统计功能相关UI组件
    QPushButton *m_generateStatisticsButton;//用于触发统计报表的生成。
    QTextEdit *m_statisticsTextEdit;//用于显示生成的统计报表
    
    // 初始化UI
    void initializeUI();
    //创建不同功能模块的选项卡（浏览、排课、报表、统计）
    void createBrowseTab(QWidget *tab);
    void createScheduleTab(QWidget *tab);
    void createReportTab(QWidget *tab);
    void createStatisticsTab(QWidget *tab);
    
    // 更新UI
    void updateCourseComboBox();
    void updateTeacherComboBox();
    void updateStudentClassComboBox();
    void updateReportCombos();
};

#endif // MAINWINDOW_H
