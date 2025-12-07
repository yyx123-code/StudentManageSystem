#ifndef STUDENTMANAGER_H
#define STUDENTMANAGER_H

#include "student.h"
#include <vector>
#include <string>

class StudentManager {
private:
    std::vector<Student> students;

public:
    StudentManager();

    void addStudent(const Student& student);//添加
    bool deleteStudent(long long id);//删除
    Student* findStudentById(long long id);//查找
    std::vector<Student> findStudentsByName(const std::string& name);
    void updateStudent(int id, const Student& newStudent);//更新

    //获取数据
    const std::vector<Student>& getAllStudents() const;
    int getStudentCount() const;

    //统计功能
    struct Statistics {
        int total;
        float averageScore;
        float passRate;
        int excellent, good, medium, pass, fail;
    };
    Statistics calculateStatistics() const;

    //文件操作
    void saveToBinaryFile(const std::string& filename);
    void loadFromBinaryFile(const std::string& filename);
    void saveToTextFile(const std::string& filename);
    void loadFromTextFile(const std::string& filename);

    //排序
    void sortById();
    void sortByScore(bool ascending = true);
    void sortByName();

private:
    std::vector<Student>::iterator findStudentIterator(long long id);
};//查找学生迭代器

#endif // STUDENTMANAGER_H
