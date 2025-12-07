#include "studentmanager.h"
#include <algorithm>
#include <fstream>      //文件操作
#include <stdexcept>

StudentManager::StudentManager() {}

void StudentManager::addStudent(const Student& student) {
    /*检查学号是否重复*/
    if (findStudentById(student.getId())) {
        throw std::runtime_error("学号已存在");
    }
    students.push_back(student);
}
/*删除学生（按学号）*/
bool StudentManager::deleteStudent(long long id) {
    auto it = findStudentIterator(id);
    if (it != students.end()) {
        students.erase(it);
        return true;
    }
    return false;
}
/*根据学号查找学生*/
Student* StudentManager::findStudentById(long long id) {
    auto it = findStudentIterator(id);
    return (it != students.end()) ? &(*it) : nullptr;
}
/*根据姓名查找学生（支持模糊匹配）*/
std::vector<Student> StudentManager::findStudentsByName(const std::string& name) {
    std::vector<Student> result;
    for (const auto& stu : students) {
        if (stu.getName().find(name) != std::string::npos) {
            result.push_back(stu);
        }
    }
    return result;
}

const std::vector<Student>& StudentManager::getAllStudents() const {
    return students;
}

int StudentManager::getStudentCount() const {
    return static_cast<int>(students.size());
}

StudentManager::Statistics StudentManager::calculateStatistics() const {
    Statistics stats = {0, 0.0f, 0.0f, 0, 0, 0, 0, 0};

    if (students.empty()) return stats;

    float totalScore = 0;
    int passCount = 0;

    for (const auto& stu : students) {
        float score = stu.getScore();
        totalScore += score;

        if (score >= 90) stats.excellent++;
        else if (score >= 80) stats.good++;
        else if (score >= 70) stats.medium++;
        else if (score >= 60) stats.pass++;
        else stats.fail++;

        if (score >= 60) passCount++;
    }

    stats.total = students.size();
    stats.averageScore = totalScore / stats.total;
    stats.passRate = (static_cast<float>(passCount) / stats.total) * 100;

    return stats;
}

/*文件操作*/
void StudentManager::saveToBinaryFile(const std::string& filename) {
    Student::saveAllToBinary(students, filename);
}

void StudentManager::loadFromBinaryFile(const std::string& filename) {
    students = Student::loadAllFromBinary(filename);
}

void StudentManager::saveToTextFile(const std::string& filename) {
    Student::saveAllToText(students, filename);
}

void StudentManager::loadFromTextFile(const std::string& filename) {
    students = Student::loadAllFromText(filename);
}

/*排序*/
void StudentManager::sortById() {
    std::sort(students.begin(), students.end(),
              [](const Student& a, const Student& b) { return a.getId() < b.getId(); });
}

void StudentManager::sortByScore(bool ascending) {
    std::sort(students.begin(), students.end(),
              [ascending](const Student& a, const Student& b) {
                  return ascending ? a.getScore() < b.getScore()
                                   : a.getScore() > b.getScore();
              });
}

void StudentManager::sortByName() {
    std::sort(students.begin(), students.end(),
              [](const Student& a, const Student& b) { return a.getName() < b.getName(); });
}

std::vector<Student>::iterator StudentManager::findStudentIterator(long long id) {
    return std::find_if(students.begin(), students.end(),
                       [id](const Student& stu) { return stu.getId() == id; });
}
