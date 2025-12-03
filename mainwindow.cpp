#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "student.h"
#include <QMessageBox>
#include <QFileDialog>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("学生信息管理系统");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_btnAdd_clicked() {
    QString name = ui->txtName->text().trimmed();
    QString idStr = ui->txtId->text().trimmed();
    QString scoreStr = ui->txtScore->text().trimmed();

    // 验证
    if (name.isEmpty() || idStr.isEmpty() || scoreStr.isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写完整信息！");
        return;
    }

    bool ok;
    int id = idStr.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "输入错误", "学号必须是数字！");
        return;
    }

    float score = scoreStr.toFloat(&ok);
    if (!ok || score < 0 || score > 100) {
        QMessageBox::warning(this, "输入错误", "成绩必须是0-100的数字！");
        return;
    }

    try {
        // 使用 Student 类创建对象
        Student stu(name.toStdString(), id, score);

        // 添加到成员变量
        students.push_back(stu);

        // 刷新显示
        refreshStudentTable();

        ui->statusbar->showMessage("添加成功", 3000);

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "错误", QString::fromStdString(e.what()));
    }
}

// 修改函数定义，去掉参数
void MainWindow::refreshStudentTable() {
    ui->tableStudents->setRowCount(0);

    for (const auto& stu : students) {
        int row = ui->tableStudents->rowCount();
        ui->tableStudents->insertRow(row);

        ui->tableStudents->setItem(row, 0,
                                   new QTableWidgetItem(QString::fromStdString(stu.getName())));
        ui->tableStudents->setItem(row, 1,
                                   new QTableWidgetItem(QString::number(stu.getId())));
        ui->tableStudents->setItem(row, 2,
                                   new QTableWidgetItem(QString::number(stu.getScore(), 'f', 1)));
        ui->tableStudents->setItem(row, 3,
                                   new QTableWidgetItem(QString(stu.getLevel())));
    }
}

void MainWindow::on_btnDelete_clicked() {
    if (students.empty()) {
        QMessageBox::information(this, "提示", "没有学生可删除");
        return;
    }

    // 简单示例：删除最后一个
    students.pop_back();
    refreshStudentTable();
    ui->statusbar->showMessage("删除成功", 3000);
}

void MainWindow::on_btnFind_clicked() {
    if (students.empty()) {
        QMessageBox::information(this, "提示", "没有学生数据");
        return;
    }

    // 简单示例：查找第一个学生
    Student& stu = students[0];
    QMessageBox::information(this, "查找结果",
                             QString("姓名: %1\n学号: %2\n成绩: %3\n等级: %4")
                                 .arg(QString::fromStdString(stu.getName()))
                                 .arg(stu.getId())
                                 .arg(stu.getScore())
                                 .arg(QString(stu.getLevel())));
}

void MainWindow::on_btnShowAll_clicked() {
    refreshStudentTable();
    ui->statusbar->showMessage("显示所有学生", 3000);
}

void MainWindow::on_btnStats_clicked() {
    if (students.empty()) {
        ui->textStats->setText("暂无学生数据");
        return;
    }

    // 使用 Student 类的静态方法进行统计
    int excellent = 0, good = 0, medium = 0, pass = 0, fail = 0;
    Student::getScoreDistribution(students, excellent, good, medium, pass, fail);

    QString stats = QString("总人数: %1人\n及格率: %2%\n\n"
                            "分数分布:\n"
                            "A(90-100): %3人\n"
                            "B(80-89):  %4人\n"
                            "C(70-79):  %5人\n"
                            "D(60-69):  %6人\n"
                            "E(0-59):   %7人")
                        .arg(students.size())
                        .arg(Student::getPassRate(students), 0, 'f', 1)
                        .arg(excellent).arg(good).arg(medium).arg(pass).arg(fail);

    ui->textStats->setText(stats);
    ui->statusbar->showMessage("统计完成", 3000);
}

void MainWindow::on_btnSave_clicked() {
    if (students.empty()) {
        QMessageBox::information(this, "提示", "没有数据可保存");
        return;
    }

    // 保存到二进制文件
    try {
        Student::saveAllToBinary(students, "students.dat");
        ui->statusbar->showMessage("保存到 students.dat 成功", 3000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "保存失败", QString::fromStdString(e.what()));
    }
}
