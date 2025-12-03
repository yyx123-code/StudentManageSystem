#include "student.h"
#include <algorithm>  // for sort

// ========== 构造函数/析构函数 ==========

Student::Student() : name(""), id(0), score(0.0f), level('E') {}

Student::Student(const std::string& name, int id, float score)
    : name(name), id(id), score(score) {
    calculateLevel();
    validateData();
}

Student::Student(const Student& other)
    : name(other.name), id(other.id), score(other.score), level(other.level) {}

Student::~Student() {
    // 如果有动态内存，在这里释放
}

// ========== 运算符重载 ==========

Student& Student::operator=(const Student& other) {
    if (this != &other) {
        name = other.name;
        id = other.id;
        score = other.score;
        level = other.level;
    }
    return *this;
}

bool Student::operator==(const Student& other) const {
    return id == other.id;  // 按学号比较
}

bool Student::operator!=(const Student& other) const {
    return !(*this == other);
}

bool Student::operator<(const Student& other) const {
    return id < other.id;  // 按学号排序
}

std::ostream& operator<<(std::ostream& os, const Student& stu) {
    os << std::left << std::setw(15) << stu.name
       << std::setw(10) << stu.id
       << std::setw(8) << std::fixed << std::setprecision(1) << stu.score
       << std::setw(5) << stu.level;
    return os;
}

std::istream& operator>>(std::istream& is, Student& stu) {
    std::cout << "请输入姓名: ";
    is >> stu.name;
    std::cout << "请输入学号: ";
    is >> stu.id;
    std::cout << "请输入成绩: ";
    is >> stu.score;

    stu.calculateLevel();
    try {
        stu.validateData();
    } catch (const std::exception& e) {
        std::cerr << "输入错误: " << e.what() << std::endl;
        throw;
    }
    return is;
}

// ========== 内部方法 ==========

void Student::calculateLevel() {
    if (score >= 90) level = 'A';
    else if (score >= 80) level = 'B';
    else if (score >= 70) level = 'C';
    else if (score >= 60) level = 'D';
    else level = 'E';
}

void Student::validateData() const {
    if (name.empty()) {
        throw std::invalid_argument("姓名不能为空");
    }
    if (id <= 0) {
        throw std::invalid_argument("学号必须为正整数");
    }
    if (score < 0 || score > 100) {
        throw std::out_of_range("成绩必须在0-100之间");
    }
}

// ========== 虚函数 ==========

void Student::display() const {
    std::cout << *this << std::endl;
}

// ========== Getter/Setter ==========

std::string Student::getName() const { return name; }
void Student::setName(const std::string& name) {
    this->name = name;
}

int Student::getId() const { return id; }
void Student::setId(int id) {
    this->id = id;
}

float Student::getScore() const { return score; }
void Student::setScore(float score) {
    this->score = score;
    calculateLevel();
}

char Student::getLevel() const { return level; }

// ========== 文件操作 ==========

void Student::writeToBinary(std::ofstream& out) const {
    if (!out.is_open()) {
        throw std::runtime_error("文件未打开");
    }

    // 写入字符串长度和内容
    size_t nameLength = name.length();
    out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    out.write(name.c_str(), nameLength);

    // 写入其他数据
    out.write(reinterpret_cast<const char*>(&id), sizeof(id));
    out.write(reinterpret_cast<const char*>(&score), sizeof(score));
    out.write(reinterpret_cast<const char*>(&level), sizeof(level));
}

void Student::readFromBinary(std::ifstream& in) {
    if (!in.is_open()) {
        throw std::runtime_error("文件未打开");
    }

    // 读取字符串
    size_t nameLength;
    in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

    char* buffer = new char[nameLength + 1];
    in.read(buffer, nameLength);
    buffer[nameLength] = '\0';
    name = buffer;
    delete[] buffer;

    // 读取其他数据
    in.read(reinterpret_cast<char*>(&id), sizeof(id));
    in.read(reinterpret_cast<char*>(&score), sizeof(score));
    in.read(reinterpret_cast<char*>(&level), sizeof(level));
}

void Student::writeToText(std::ofstream& out) const {
    if (!out.is_open()) {
        throw std::runtime_error("文件未打开");
    }
    out << name << " " << id << " " << score << " " << level << std::endl;
}

void Student::readFromText(std::ifstream& in) {
    if (!in.is_open()) {
        throw std::runtime_error("文件未打开");
    }
    in >> name >> id >> score >> level;
}

// ========== 静态方法：管理多个学生 ==========

void Student::saveAllToBinary(const std::vector<Student>& students,
                              const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    // 写入学生数量
    size_t count = students.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    // 写入每个学生
    for (const auto& stu : students) {
        stu.writeToBinary(out);
    }
    out.close();
}

std::vector<Student> Student::loadAllFromBinary(const std::string& filename) {
    std::vector<Student> students;
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    // 读取学生数量
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    // 读取每个学生
    for (size_t i = 0; i < count; i++) {
        Student stu;
        stu.readFromBinary(in);
        students.push_back(stu);
    }
    in.close();
    return students;
}

void Student::saveAllToText(const std::vector<Student>& students,
                            const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    out << students.size() << std::endl;
    for (const auto& stu : students) {
        stu.writeToText(out);
    }
    out.close();
}

std::vector<Student> Student::loadAllFromText(const std::string& filename) {
    std::vector<Student> students;
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    size_t count;
    in >> count;

    for (size_t i = 0; i < count; i++) {
        Student stu;
        stu.readFromText(in);
        students.push_back(stu);
    }
    in.close();
    return students;
}

// ========== 统计功能 ==========

int Student::getTotalCount(const std::vector<Student>& students) {
    return static_cast<int>(students.size());
}

float Student::getPassRate(const std::vector<Student>& students) {
    if (students.empty()) return 0.0f;

    int passCount = 0;
    for (const auto& stu : students) {
        if (stu.getScore() >= 60) {
            passCount++;
        }
    }
    return static_cast<float>(passCount) / students.size() * 100;
}

void Student::getScoreDistribution(const std::vector<Student>& students,
                                   int& excellent, int& good, int& medium,
                                   int& pass, int& fail) {
    excellent = good = medium = pass = fail = 0;

    for (const auto& stu : students) {
        float score = stu.getScore();
        if (score >= 90) excellent++;
        else if (score >= 80) good++;
        else if (score >= 70) medium++;
        else if (score >= 60) pass++;
        else fail++;
    }
}

// ========== 查找/删除功能 ==========

bool Student::deleteById(std::vector<Student>& students, int id) {
    auto it = std::remove_if(students.begin(), students.end(),
                             [id](const Student& stu) { return stu.getId() == id; });

    if (it != students.end()) {
        students.erase(it, students.end());
        return true;
    }
    return false;
}

Student* Student::findById(std::vector<Student>& students, int id) {
    for (auto& stu : students) {
        if (stu.getId() == id) {
            return &stu;
        }
    }
    return nullptr;
}

std::vector<Student> Student::findByName(const std::vector<Student>& students,
                                         const std::string& name) {
    std::vector<Student> result;
    for (const auto& stu : students) {
        if (stu.getName().find(name) != std::string::npos) {
            result.push_back(stu);
        }
    }
    return result;
}
