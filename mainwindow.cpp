#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "studentmanager.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QInputDialog>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("学生信息管理系统");

    // 设置表格
    ui->tableStudents->setColumnCount(4);
    QStringList headers;
    headers << "姓名" << "学号" << "成绩" << "等级";
    ui->tableStudents->setHorizontalHeaderLabels(headers);

    // ⭐ 新增：程序启动时自动加载数据
    try {
        studentManager.loadFromBinaryFile("students.dat");
        refreshStudentTable();
        ui->statusbar->showMessage(QString("已加载 %1 名学生数据").arg(studentManager.getStudentCount()), 3000);
    } catch (const std::exception& e) {
        // 文件不存在是正常的，不显示错误
        ui->statusbar->showMessage("欢迎使用学生信息管理系统", 3000);
    }
}

MainWindow::~MainWindow()
{
    // 程序关闭时自动保存
    try {
        if (studentManager.getStudentCount() > 0) {
            studentManager.saveToBinaryFile("students.dat");
            qDebug() << "程序关闭，数据已自动保存";
        }
    } catch (...) {
        // 忽略析构函数中的异常
    }
    delete ui;
}

void MainWindow::on_btnAdd_clicked()
{
    QString name = ui->txtName->text().trimmed();
    QString idStr = ui->txtId->text().trimmed();
    QString scoreStr = ui->txtScore->text().trimmed();

    /*验证输入*/
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
        Student stu(name.toStdString(), id, score);
        studentManager.addStudent(stu);

        refreshStudentTable();

        ui->txtName->clear();
        ui->txtId->clear();
        ui->txtScore->clear();

        ui->statusbar->showMessage(QString("添加成功！当前共 %1 名学生").arg(studentManager.getStudentCount()), 3000);
        saveDataToFile();

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "错误", QString::fromStdString(e.what()));
    }
}

void MainWindow::on_btnDelete_clicked()
{
    if (studentManager.getStudentCount() == 0) {
        QMessageBox::information(this, "提示", "没有学生可删除");
        return;
    }

    /*获取当前选中的行*/
    int currentRow = ui->tableStudents->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要删除的学生行");
        return;
    }

    /*获取选中行的学号*/
    QTableWidgetItem* idItem = ui->tableStudents->item(currentRow, 1);
    if (!idItem) {
        QMessageBox::warning(this, "错误", "无法获取学号");
        return;
    }

    int idToDelete = idItem->text().toInt();

    /*确认删除*/
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                  QString("确定要删除学号为 %1 的学生吗？").arg(idToDelete),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (studentManager.deleteStudent(idToDelete)) {
            refreshStudentTable();
            ui->statusbar->showMessage(QString("成功删除学号 %1").arg(idToDelete), 3000);
            saveDataToFile();
        } else {
            QMessageBox::warning(this, "错误", "未找到该学号的学生");
        }
    }
}

void MainWindow::on_btnFind_clicked()
{
    if (studentManager.getStudentCount() == 0) {
        QMessageBox::information(this, "提示", "没有学生数据");
        return;
    }

    /*提供两种查找方式*/
    QStringList options;
    options << "按学号查找" << "按姓名查找";

    bool ok;
    QString choice = QInputDialog::getItem(this, "查找方式", "选择查找方式:",
                                           options, 0, false, &ok);
    if (!ok) return;

    if (choice == "按学号查找") {
        QString idStr = QInputDialog::getText(this, "按学号查找", "请输入学号:",
                                              QLineEdit::Normal, "", &ok);
        if (!ok || idStr.isEmpty()) return;

        int id = idStr.toInt();
        Student* stu = studentManager.findStudentById(id);

        if (stu) {
            showStudentInfo(*stu, "查找结果");
        } else {
            QMessageBox::information(this, "结果", "未找到该学号的学生");
        }

    } else { //按姓名查找
        QString name = QInputDialog::getText(this, "按姓名查找", "请输入姓名(支持模糊匹配):",
                                             QLineEdit::Normal, "", &ok);
        if (!ok || name.isEmpty()) return;

        std::vector<Student> results = studentManager.findStudentsByName(name.toStdString());

        if (!results.empty()) {
            QString message = QString("找到 %1 名学生:\n\n").arg(results.size());
            for (const auto& stu : results) {
                message += QString("姓名: %1, 学号: %2, 成绩: %3, 等级: %4\n")
                               .arg(QString::fromStdString(stu.getName()))
                               .arg(stu.getId())
                               .arg(stu.getScore(), 0, 'f', 1)
                               .arg(QString(stu.getLevel()));
            }
            QMessageBox::information(this, "查找结果", message);
        } else {
            QMessageBox::information(this, "结果", "未找到匹配的学生");
        }
    }
}

void MainWindow::on_btnShowAll_clicked()
{
    refreshStudentTable();
    ui->statusbar->showMessage(QString("显示所有 %1 名学生").arg(studentManager.getStudentCount()), 3000);
}

void MainWindow::on_btnStats_clicked()
{
    if (studentManager.getStudentCount() == 0) {
        ui->textStats->setText("暂无学生数据");
        return;
    }

    StudentManager::Statistics stats = studentManager.calculateStatistics();

    QString statsText = QString("📊 学生信息统计\n"
                                "====================\n"
                                "总人数: %1 人\n"
                                "平均分: %2 分\n"
                                "及格率: %3%\n\n"
                                "📈 分数段分布:\n"
                                "A (90-100): %4 人 (%5%)\n"
                                "B (80-89):  %6 人 (%7%)\n"
                                "C (70-79):  %8 人 (%9%)\n"
                                "D (60-69):  %10 人 (%11%)\n"
                                "E (0-59):   %12 人 (%13%)\n\n"
                                "💾 数据统计完成时间: %14")
                            .arg(stats.total)
                            .arg(stats.averageScore, 0, 'f', 1)
                            .arg(stats.passRate, 0, 'f', 1)
                            .arg(stats.excellent)
                            .arg((stats.excellent * 100.0 / stats.total), 0, 'f', 1)
                            .arg(stats.good)
                            .arg((stats.good * 100.0 / stats.total), 0, 'f', 1)
                            .arg(stats.medium)
                            .arg((stats.medium * 100.0 / stats.total), 0, 'f', 1)
                            .arg(stats.pass)
                            .arg((stats.pass * 100.0 / stats.total), 0, 'f', 1)
                            .arg(stats.fail)
                            .arg((stats.fail * 100.0 / stats.total), 0, 'f', 1)
                            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    ui->textStats->setText(statsText);
    ui->statusbar->showMessage("统计完成", 3000);
}

void MainWindow::on_btnSave_clicked()
{
    if (studentManager.getStudentCount() == 0) {
        QMessageBox::information(this, "提示", "没有数据可保存");
        return;
    }

    // 提供更多保存选项
    QStringList formats;
    formats << "保存到二进制文件 (.dat)"
            << "导出到文本文件 (.txt)"
            << "另存为二进制文件..."
            << "另存为文本文件...";

    bool ok;
    QString choice = QInputDialog::getItem(this, "保存选项", "选择操作:",
                                           formats, 0, false, &ok);
    if (!ok) return;

    try {
        if (choice.contains("二进制文件 (.dat)")) {
            //默认保存
            studentManager.saveToBinaryFile("students.dat");
            ui->statusbar->showMessage("数据已保存到 students.dat", 3000);

        } else if (choice.contains("文本文件 (.txt)")) {
            //导出为文本
            studentManager.saveToTextFile("students_export.txt");
            ui->statusbar->showMessage("数据已导出到 students_export.txt", 3000);

        } else if (choice.contains("另存为二进制文件")) {
            //另存为二进制
            QString fileName = QFileDialog::getSaveFileName(this,
                                                            "另存为二进制文件", "", "二进制文件 (*.dat)");
            if (!fileName.isEmpty()) {
                studentManager.saveToBinaryFile(fileName.toStdString());
                ui->statusbar->showMessage("数据已保存", 3000);
            }

        } else if (choice.contains("另存为文本文件")) {
            //另存为文本
            QString fileName = QFileDialog::getSaveFileName(this,
                                                            "另存为文本文件", "", "文本文件 (*.txt)");
            if (!fileName.isEmpty()) {
                studentManager.saveToTextFile(fileName.toStdString());
                ui->statusbar->showMessage("数据已保存", 3000);
            }
        }

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "保存失败",
                              QString("保存文件时出错:\n%1").arg(QString::fromStdString(e.what())));
    }
}

void MainWindow::on_btnImportTxt_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "选择文本文件", "", "文本文件 (*.txt)");

    if (fileName.isEmpty()) return;

    try {
        // 从文本文件加载
        studentManager.loadFromTextFile(fileName.toStdString());

        refreshStudentTable();

        ui->statusbar->showMessage(
            QString("从 %1 导入 %2 名学生成功")
                .arg(QFileInfo(fileName).fileName())
                .arg(studentManager.getStudentCount()),
            5000);

        // 自动保存到二进制文件
        saveDataToFile();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "导入失败",
                              QString("导入文件时出错:\n%1\n\n"
                                      "请确保文件格式正确，每行格式为:\n"
                                      "姓名 学号 成绩 等级").arg(QString::fromStdString(e.what())));
    }
}

void MainWindow::refreshStudentTable()
{
    ui->tableStudents->setRowCount(0);

    const std::vector<Student>& students = studentManager.getAllStudents();

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

    /*设置表格样式*/
    ui->tableStudents->resizeColumnsToContents();
}

void MainWindow::showStudentInfo(const Student& stu, const QString& title)
{
    QString info = QString("👤 学生详细信息\n"
                           "====================\n"
                           "姓名: %1\n"
                           "学号: %2\n"
                           "成绩: %3\n"
                           "等级: %4\n\n"
                           "📝 完整信息:\n%5")
                       .arg(QString::fromStdString(stu.getName()))
                       .arg(stu.getId())
                       .arg(stu.getScore(), 0, 'f', 1)
                       .arg(QString(stu.getLevel()))
                       .arg("学生信息显示完整");  // 这里可以调用 stu.toString() 如果有的话

    QMessageBox::information(this, title, info);
}

void MainWindow::saveDataToFile()
{
    if (studentManager.getStudentCount() == 0) {
        return;//没有数据不保存
    }

    try {
        //保存到二进制文件
        studentManager.saveToBinaryFile("students.dat");

        //可选：同时保存到文本文件（便于查看）
        studentManager.saveToTextFile("students_backup.txt");

        //不显示提示，避免干扰用户
    } catch (const std::exception& e) {
        //静默失败，避免频繁弹窗
        qDebug() << "保存文件失败:" << e.what();
    }
}
