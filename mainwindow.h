#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "student.h"    //包含学生管理头文件

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:                      //自动连接：on_对象名_信号名
    void on_btnAdd_clicked();       //添加
    void on_btnDelete_clicked();    //删除
    void on_btnFind_clicked();      //查找
    void on_btnShowAll_clicked();   //显示
    void on_btnStats_clicked();     //统计
    void on_btnSave_clicked();      //保存

private:
    Ui::MainWindow *ui;
    std::vector<Student> students;//储存所有学生
    void refreshStudentTable();
};

#endif // MAINWINDOW_H
