#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include "studentmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAdd_clicked();
    void on_btnDelete_clicked();
    void on_btnFind_clicked();
    void on_btnShowAll_clicked();
    void on_btnStats_clicked();
    void on_btnSave_clicked();

private:
    Ui::MainWindow *ui;
    StudentManager studentManager;

    void refreshStudentTable();
    void showStudentInfo(const Student& stu, const QString& title = "学生信息");
};

#endif // MAINWINDOW_H
