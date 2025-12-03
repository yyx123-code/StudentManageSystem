#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iomanip>

class Student {
private:
    std::string name;
    int id;
    float score;
    char level;          // A/B/C/D/E

    // 内部方法
    void calculateLevel();
    void validateData() const;

public:
    // ========== 构造函数/析构函数 ==========
    Student();                                   // 默认构造
    Student(const std::string& name, int id, float score); // 带参构造
    Student(const Student& other);               // 拷贝构造
    ~Student();                                  // 析构函数

    // ========== 运算符重载（课程要求） ==========
    Student& operator=(const Student& other);    // 赋值运算符
    bool operator==(const Student& other) const; // 比较运算符
    bool operator!=(const Student& other) const;
    bool operator<(const Student& other) const;  // 用于排序

    // 输入输出运算符
    friend std::ostream& operator<<(std::ostream& os, const Student& stu);
    friend std::istream& operator>>(std::istream& is, Student& stu);

    // ========== 虚函数（课程要求） ==========
    virtual void display() const;

    // ========== Getter/Setter ==========
    std::string getName() const;
    void setName(const std::string& name);
    int getId() const;
    void setId(int id);
    float getScore() const;
    void setScore(float score);
    char getLevel() const;

    // ========== 文件操作（课程要求） ==========
    void writeToBinary(std::ofstream& out) const;      // 二进制写
    void readFromBinary(std::ifstream& in);            // 二进制读
    void writeToText(std::ofstream& out) const;        // 文本写
    void readFromText(std::ifstream& in);              // 文本读

    // ========== 静态方法（管理多个学生） ==========
    static void saveAllToBinary(const std::vector<Student>& students,
                                const std::string& filename);
    static std::vector<Student> loadAllFromBinary(const std::string& filename);
    static void saveAllToText(const std::vector<Student>& students,
                              const std::string& filename);
    static std::vector<Student> loadAllFromText(const std::string& filename);

    // ========== 统计功能 ==========
    static int getTotalCount(const std::vector<Student>& students);
    static float getPassRate(const std::vector<Student>& students);
    static void getScoreDistribution(const std::vector<Student>& students,
                                     int& excellent, int& good, int& medium,
                                     int& pass, int& fail);

    // ========== 查找/删除功能 ==========
    static bool deleteById(std::vector<Student>& students, int id);
    static Student* findById(std::vector<Student>& students, int id);
    static std::vector<Student> findByName(const std::vector<Student>& students,
                                           const std::string& name);
};

#endif // STUDENT_H
