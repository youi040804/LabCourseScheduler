#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // 设置窗口标题和大小
    setWindowTitle("地大机房排课助手");
    resize(1300, 700);

    // 创建排课系统
    m_scheduleSystem = new ScheduleSystem(this);
    // 创建报表生成器
    m_reportGenerator = new ReportGenerator(this);
    m_reportGenerator->setScheduleSystem(m_scheduleSystem);
    
    // 初始化UI
    initializeUI();
    
    // 更新初始数据
    onBuildingChanged(m_buildingComboBox->currentText());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onBuildingChanged(const QString &building)
{
    m_classroomComboBox->clear();
    
    // 获取选中教学楼的教室列表
    QVector<QString> classrooms = m_scheduleSystem->getClassroomIds(building);
    m_classroomComboBox->addItems(QStringList(classrooms.begin(), classrooms.end()));
    
    // 更新排课表
    m_scheduleTable->setSelectedBuilding(building);
    if (!classrooms.isEmpty()) {
        m_classroomComboBox->setCurrentIndex(0);
        onClassroomChanged(m_classroomComboBox->currentText());
    }
}

void MainWindow::onClassroomChanged(const QString &classroom)
{
    if (classroom.isEmpty()) {
        return;
    }
    
    // 更新排课表
    m_scheduleTable->setSelectedClassroomId(classroom);
    m_scheduleTable->updateTable();
    
    // 显示教室信息
    Classroom selectedClassroom = m_scheduleSystem->getClassroom(classroom);
    m_statusLabel->setText(selectedClassroom.toString());
}

void MainWindow::onScheduleClicked(const Schedule &schedule)
{
    // 在状态栏显示排课信息
    m_statusLabel->setText(schedule.getStatusInfo());
}

void MainWindow::onStartScheduling()
{
    // 从 UI 控件中获取课程、教师、班级、学院和排课次数等排课信息
    QString course = m_courseComboBox->currentText();
    QString teacher = m_teacherComboBox->currentText();
    QString teacherCollege = m_teacherCollegeLineEdit->text();
    QString studentClass = m_studentClassComboBox->currentText();
    QString studentCollege = m_studentCollegeLineEdit->text();
    int totalCount = m_scheduleCountSpinBox->value();

    if (course.isEmpty() || teacher.isEmpty() || teacherCollege.isEmpty() || 
        studentClass.isEmpty() || studentCollege.isEmpty() || totalCount <= 0) {
        QMessageBox::warning(this, "警告", "请填写完整的排课信息！");
        return;
    }

    ScheduleTable *scheduleTable = nullptr;
    QWidget *tab = m_tabWidget->widget(1); // 获取排课标签页
    if (tab) {
        // 在排课标签页中查找ScheduleTable类型的控件
        scheduleTable = tab->findChild<ScheduleTable*>();
    }
    
    if (!scheduleTable) {
        QMessageBox::warning(this, "错误", "未找到排课表格！");
        return;
    }
    
    // 设置排课信息
    scheduleTable->setCurrentScheduleInfo(
        course, teacher, teacherCollege, studentClass, studentCollege, totalCount
    );
    
    // 更新UI，显示剩余排课次数
    int remaining = scheduleTable->getRemainingSchedules();
    m_remainingScheduleLabel->setText(QString("剩余排课次数: %1").arg(remaining));
    
    // 提示用户进行排课
    QMessageBox::information(this, "操作提示", 
        "请在排课表中进行排课操作：\n"
        "1. 左键双击空白单元格进行排课\n"
        "2. 右键单击已排课单元格取消排课\n"
        "3. 切换到其他标签页可结束本次排课");
    
    // 将焦点设置到排课表
    scheduleTable->setFocus();
    
    // 更新课程、教师、班级和报表生成的下拉框内容
    updateCourseComboBox();
    updateTeacherComboBox();
    updateStudentClassComboBox();
    updateReportCombos();
}

void MainWindow::onRemainingSchedulesChanged(int remaining)
{
    m_remainingScheduleLabel->setText(QString("剩余排课次数: %1").arg(remaining));
}

//当用户点击生成报表按钮时，生成排课报表并显示
void MainWindow::onGenerateReport()
{
    //从报表生成的下拉框中获取课程、教师和班级信息
    QString course = m_reportCourseComboBox->currentText();
    QString teacher = m_reportTeacherComboBox->currentText();
    QString studentClass = m_reportStudentClassComboBox->currentText();
    
    // 验证输入
    if (course.isEmpty() || teacher.isEmpty() || studentClass.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择课程、教师和班级！");
        return;
    }
    
    // 调用 m_reportGenerator->generateScheduleReport() 生成排课报表
    QString report = m_reportGenerator->generateScheduleReport(course, teacher, studentClass);
    
    // 显示报表
    m_reportTextEdit->setText(report);
}

void MainWindow::onGenerateStatistics()
{
    // 生成统计报表
    QString report = m_reportGenerator->generateStatisticsReport();
    
    // 显示统计报表
    m_statisticsTextEdit->setText(report);
}
void MainWindow::onTabChanged(int index)
{
    // 切换标签页时，更新数据
    switch (index) {
    case 0: // 浏览功能
        // 更新排课表
        onClassroomChanged(m_classroomComboBox->currentText());
        break;
    case 1: // 排课功能
        updateCourseComboBox();
        updateTeacherComboBox();
        updateStudentClassComboBox();
        // 确保表格与浏览功能中的选项保持一致
        onClassroomChanged(m_classroomComboBox->currentText());
        break;
    case 2: // 报表功能
        updateReportCombos();
        break;
    case 3: // 统计功能
        break;
    }
}

void MainWindow::initializeUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->centralwidget);
    
    // 创建标签页
    m_tabWidget = new QTabWidget();
    mainLayout->addWidget(m_tabWidget);
    
    // 创建四个功能标签页
    QWidget *browseTab = new QWidget();
    QWidget *scheduleTab = new QWidget();
    QWidget *reportTab = new QWidget();
    QWidget *statisticsTab = new QWidget();
    
    // 添加四个标签页
    m_tabWidget->addTab(browseTab, "浏览功能");
    m_tabWidget->addTab(scheduleTab, "排课功能");
    m_tabWidget->addTab(reportTab, "报表功能");
    m_tabWidget->addTab(statisticsTab, "统计功能");
    
    // 创建标签页内容
    createBrowseTab(browseTab);
    createScheduleTab(scheduleTab);
    createReportTab(reportTab);
    createStatisticsTab(statisticsTab);
    
    // 创建状态栏
    m_statusLabel = new QLabel();
    statusBar()->addWidget(m_statusLabel, 1);
    
    // 连接标签页切换信号
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::createBrowseTab(QWidget *tab)
{
    //用于创建垂直布局管理器，它会将所有添加到其中的控件按垂直方向排列
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    // 创建选择区域
    QGroupBox *selectionGroup = new QGroupBox("选择机房");
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionGroup);
    
    // 教学楼选择
    QLabel *buildingLabel = new QLabel("教学楼:");
    m_buildingComboBox = new QComboBox();
    m_buildingComboBox->addItems(m_scheduleSystem->getClassroomBuildings());
    m_buildingComboBox->setEditable(true);
    
    // 教室选择
    QLabel *classroomLabel = new QLabel("教室:");
    m_classroomComboBox = new QComboBox();
    m_classroomComboBox->setEditable(true);
    selectionLayout->addWidget(buildingLabel);
    selectionLayout->addWidget(m_buildingComboBox);
    selectionLayout->addWidget(classroomLabel);
    selectionLayout->addWidget(m_classroomComboBox);
    selectionLayout->addStretch();
    
    layout->addWidget(selectionGroup);
    
    // 创建排课表
    m_scheduleTable = new ScheduleTable(tab);
    m_scheduleTable->setScheduleSystem(m_scheduleSystem);
    layout->addWidget(m_scheduleTable);
    
    // 连接信号与槽
    connect(m_buildingComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onBuildingChanged);
    connect(m_classroomComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onClassroomChanged);
    connect(m_scheduleTable, &ScheduleTable::scheduleClicked, this, &MainWindow::onScheduleClicked);

    connect(m_scheduleTable, &ScheduleTable::scheduleChanged, [=]() {
        // 查找排课界面中的排课表格
        ScheduleTable *scheduleTable = nullptr;
        QWidget *scheduleTab = m_tabWidget->widget(1); // 获取排课标签页
        if (scheduleTab) {
            // 在排课标签页中查找ScheduleTable类型的控件
            scheduleTable = scheduleTab->findChild<ScheduleTable*>();
            if (scheduleTable) {
                scheduleTable->updateTable();
            }
        }
        
        // 更新下拉框数据
        updateCourseComboBox();
        updateTeacherComboBox();
        updateStudentClassComboBox();
        updateReportCombos();
    });
}

void MainWindow::createScheduleTab(QWidget *tab)
{

    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    // 创建排课信息区域
    QGroupBox *infoGroup = new QGroupBox("排课信息");
    QGridLayout *infoLayout = new QGridLayout(infoGroup);
    
    QLabel *courseLabel = new QLabel("课程名称:");
    m_courseComboBox = new QComboBox();
    m_courseComboBox->setEditable(true);
    
    QLabel *teacherLabel = new QLabel("任课教师:");
    m_teacherComboBox = new QComboBox();
    m_teacherComboBox->setEditable(true);
    
    QLabel *teacherCollegeLabel = new QLabel("教师学院:");
    m_teacherCollegeLineEdit = new QLineEdit();
    
    QLabel *studentClassLabel = new QLabel("学生班级:");
    m_studentClassComboBox = new QComboBox();
    m_studentClassComboBox->setEditable(true);
    
    QLabel *studentCollegeLabel = new QLabel("学生学院:");
    m_studentCollegeLineEdit = new QLineEdit();
    
    QLabel *countLabel = new QLabel("排课次数:");
    m_scheduleCountSpinBox = new QSpinBox();
    m_scheduleCountSpinBox->setRange(1, 100);
    m_scheduleCountSpinBox->setValue(1);
    
    infoLayout->addWidget(courseLabel, 0, 0);
    infoLayout->addWidget(m_courseComboBox, 0, 1);
    infoLayout->addWidget(teacherLabel, 0, 2);
    infoLayout->addWidget(m_teacherComboBox, 0, 3);
    
    infoLayout->addWidget(teacherCollegeLabel, 1, 0);
    infoLayout->addWidget(m_teacherCollegeLineEdit, 1, 1);
    infoLayout->addWidget(studentClassLabel, 1, 2);
    infoLayout->addWidget(m_studentClassComboBox, 1, 3);
    
    infoLayout->addWidget(studentCollegeLabel, 2, 0);
    infoLayout->addWidget(m_studentCollegeLineEdit, 2, 1);
    infoLayout->addWidget(countLabel, 2, 2);
    infoLayout->addWidget(m_scheduleCountSpinBox, 2, 3);
    
    layout->addWidget(infoGroup);
    
    // 创建排课操作区域
    QHBoxLayout *operationLayout = new QHBoxLayout();
    m_startSchedulingButton = new QPushButton("开始排课");
    m_remainingScheduleLabel = new QLabel("剩余排课次数: 0");
    
    operationLayout->addWidget(m_startSchedulingButton);
    operationLayout->addWidget(m_remainingScheduleLabel);
    operationLayout->addStretch();
    
    layout->addLayout(operationLayout);
    
    // 创建选择区域
    QGroupBox *selectionGroup = new QGroupBox("选择机房");
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionGroup);
    
    // 教学楼选择
    QLabel *buildingLabel = new QLabel("教学楼:");
    QComboBox *buildingComboBox = new QComboBox();
    buildingComboBox->addItems(m_scheduleSystem->getClassroomBuildings());
    
    // 教室选择
    QLabel *classroomLabel = new QLabel("教室:");
    QComboBox *classroomComboBox = new QComboBox();
    
    selectionLayout->addWidget(buildingLabel);
    selectionLayout->addWidget(buildingComboBox);
    selectionLayout->addWidget(classroomLabel);
    selectionLayout->addWidget(classroomComboBox);
    selectionLayout->addStretch();
    
    layout->addWidget(selectionGroup);
    
    // 创建排课表（恢复独立的表格）
    ScheduleTable *scheduleTable = new ScheduleTable(tab);
    scheduleTable->setScheduleSystem(m_scheduleSystem);//将排课系统传递给排课表格，使得表格能够显示数据
    layout->addWidget(scheduleTable);
    
    // 连接信号与槽
    connect(m_startSchedulingButton, &QPushButton::clicked, this, &MainWindow::onStartScheduling);
    connect(scheduleTable, &ScheduleTable::remainingSchedulesChanged, this, &MainWindow::onRemainingSchedulesChanged);
    connect(scheduleTable, &ScheduleTable::scheduleClicked, this, &MainWindow::onScheduleClicked);
    // 同步排课表和浏览表格的变更
    connect(scheduleTable, &ScheduleTable::scheduleChanged, [=]() {
        // 当排课发生变化时，更新浏览表格
        m_scheduleTable->updateTable();
        // 更新下拉框数据
        updateCourseComboBox();
        updateTeacherComboBox();
        updateStudentClassComboBox();
        updateReportCombos();
    });
    
    //当用户选择不同的教学楼时，会更新教室选择框中的内容，并刷新排课表格，显示对应教学楼下的教室排课数据
    connect(buildingComboBox, &QComboBox::currentTextChanged, [=](const QString &building) {
        classroomComboBox->clear();
        classroomComboBox->addItems(m_scheduleSystem->getClassroomIds(building));
        if (!classroomComboBox->count()) {
            return;
        }
        scheduleTable->setSelectedBuilding(building);
        scheduleTable->setSelectedClassroomId(classroomComboBox->currentText());
        scheduleTable->updateTable();
    });
    //当用户选择不同的教室时，会更新排课表格，显示选中教室的排课数据。
    connect(classroomComboBox, &QComboBox::currentTextChanged, [=](const QString &classroom) {
        scheduleTable->setSelectedClassroomId(classroom);
        scheduleTable->updateTable();
    });
    
    // 初始化数据
    if (buildingComboBox->count() > 0) {
        buildingComboBox->setCurrentIndex(0);
        classroomComboBox->addItems(m_scheduleSystem->getClassroomIds(buildingComboBox->currentText()));
        if (classroomComboBox->count() > 0) {
            classroomComboBox->setCurrentIndex(0);
            scheduleTable->setSelectedBuilding(buildingComboBox->currentText());
            scheduleTable->setSelectedClassroomId(classroomComboBox->currentText());
            scheduleTable->updateTable();
        }
    }
}

//创建并初始化报表功能模块的 UI 界面
void MainWindow::createReportTab(QWidget *tab)
{
    QVBoxLayout *layout = new QVBoxLayout(tab);

    // 创建选择区域
    QGroupBox *selectionGroup = new QGroupBox("选择排课信息");
    QGridLayout *selectionLayout = new QGridLayout(selectionGroup);

    QLabel *courseLabel = new QLabel("课程:");
    m_reportCourseComboBox = new QComboBox();

    QLabel *teacherLabel = new QLabel("教师:");
    m_reportTeacherComboBox = new QComboBox();

    QLabel *classLabel = new QLabel("班级:");
    m_reportStudentClassComboBox = new QComboBox();

    m_generateReportButton = new QPushButton("生成报表");

    selectionLayout->addWidget(courseLabel, 0, 0);
    selectionLayout->addWidget(m_reportCourseComboBox, 0, 1);
    selectionLayout->addWidget(teacherLabel, 0, 2);
    selectionLayout->addWidget(m_reportTeacherComboBox, 0, 3);
    selectionLayout->addWidget(classLabel, 1, 0);
    selectionLayout->addWidget(m_reportStudentClassComboBox, 1, 1);
    selectionLayout->addWidget(m_generateReportButton, 1, 3);

    layout->addWidget(selectionGroup);

    // 创建报表显示区域
    m_reportTextEdit = new QTextEdit();
    m_reportTextEdit->setReadOnly(true);
    m_reportTextEdit->setFontFamily("Consolas");
    m_reportTextEdit->setLineWrapMode(QTextEdit::NoWrap);
    layout->addWidget(m_reportTextEdit, 1);

    // 连接信号与槽
    connect(m_generateReportButton, &QPushButton::clicked, this, &MainWindow::onGenerateReport);
}

void MainWindow::createStatisticsTab(QWidget *tab)
{
    QVBoxLayout *layout = new QVBoxLayout(tab);

    // 创建按钮
    m_generateStatisticsButton = new QPushButton("生成统计表");
    layout->addWidget(m_generateStatisticsButton);

    // 创建统计表显示区域
    m_statisticsTextEdit = new QTextEdit();
    m_statisticsTextEdit->setReadOnly(true);
    m_statisticsTextEdit->setFontFamily("Consolas");
    m_statisticsTextEdit->setLineWrapMode(QTextEdit::NoWrap);
    layout->addWidget(m_statisticsTextEdit, 1);

    // 连接信号与槽
    connect(m_generateStatisticsButton, &QPushButton::clicked, this, &MainWindow::onGenerateStatistics);
}

void MainWindow::updateCourseComboBox()
{
   // 获取课程选择框当前选中的课程文本 currentText
    QString currentText = m_courseComboBox->currentText();
    
    m_courseComboBox->clear();
    m_courseComboBox->addItems(m_scheduleSystem->getAllCourses());
    m_courseComboBox->setCurrentText(currentText);
}

void MainWindow::updateTeacherComboBox()
{
    QString currentText = m_teacherComboBox->currentText();
    
    m_teacherComboBox->clear();
    m_teacherComboBox->addItems(m_scheduleSystem->getAllTeachers());
    m_teacherComboBox->setCurrentText(currentText);
}

void MainWindow::updateStudentClassComboBox()
{
    QString currentText = m_studentClassComboBox->currentText();
    
    m_studentClassComboBox->clear();
    m_studentClassComboBox->addItems(m_scheduleSystem->getAllStudentClasses());
    m_studentClassComboBox->setCurrentText(currentText);
}

//更新报表相关的下拉框（课程、教师、班级）
void MainWindow::updateReportCombos()
{
    // 保存当前选项
    QString currentCourse = m_reportCourseComboBox->currentText();
    QString currentTeacher = m_reportTeacherComboBox->currentText();
    QString currentClass = m_reportStudentClassComboBox->currentText();
    
   // 更新课程下拉框
    m_reportCourseComboBox->clear();
    m_reportCourseComboBox->addItems(m_scheduleSystem->getAllCourses());
    if (!currentCourse.isEmpty() && m_reportCourseComboBox->findText(currentCourse) >= 0) {
        m_reportCourseComboBox->setCurrentText(currentCourse);
    }
    
    // 更新教师下拉框
    m_reportTeacherComboBox->clear();
    m_reportTeacherComboBox->addItems(m_scheduleSystem->getAllTeachers());
    if (!currentTeacher.isEmpty() && m_reportTeacherComboBox->findText(currentTeacher) >= 0) {
        m_reportTeacherComboBox->setCurrentText(currentTeacher);
    }
    
    // 更新班级下拉框
    m_reportStudentClassComboBox->clear();
    m_reportStudentClassComboBox->addItems(m_scheduleSystem->getAllStudentClasses());
    if (!currentClass.isEmpty() && m_reportStudentClassComboBox->findText(currentClass) >= 0) {
        m_reportStudentClassComboBox->setCurrentText(currentClass);
    }
}
