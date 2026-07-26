 # 学生信息管理系统 (Student Management System)

 基于 **C++17** 和 **Qt5 Widgets** 构建的桌面版学生信息管理应用，采用 qmake 构建系统，支持中文界面与数据持久化。

 ## 功能一览

 | 模块 | 功能 | 说明 |
 |------|------|------|
 | 添加学生 | 姓名 + 学号 + 成绩 | 自动计算等级（A/B/C/D/E）并在添加后自动保存 |
 | 删除学生 | 选中表格行 → 确认删除 | 弹出确认对话框，防止误操作 |
 | 查找学生 | 按学号精确查找 / 按姓名模糊查找 | 弹窗展示详细结果 |
 | 显示全部 | 刷新表格，展示所有学生 | 状态栏提示当前总人数 |
 | 统计面板 | 总人数、平均分、及格率、分数段分布 | 实时显示在界面右侧统计区 |
 | 文件保存 | 二进制(.dat) / 文本(.txt) | 支持默认保存和另存为 |
 | 文本导入 | 从外部 .txt 导入学生数据 | 自动保存到二进制文件 |
 | 自动持久化 | 启动时加载 `students.dat` | 退出时自动保存，数据不丢失 |
 | 排序（内部） | 按学号 / 成绩(升降序) / 姓名 | `StudentManager` 层支持，便于扩展 |

 ## 技术栈

 - **语言**: C++17
 - **GUI 框架**: Qt 5 (Widgets)
 - **构建工具**: qmake
 - **构建配置**: `StudentManageSystem.pro`
 - **国际化**: Qt Linguist (内嵌 `zh_CN` 翻译)

 ## 项目结构

 ```
 StudentManageSystem/
 ├── main.cpp                 # 入口点，初始化 QApplication 和翻译
 ├── mainwindow.h / .cpp      # Qt 主窗口逻辑（信号/槽、表格刷新、对话框）
 ├── mainwindow.ui            # Qt Designer UI 布局文件
 ├── student.h / .cpp         # 学生实体类（数据校验、运算符重载、文件序列化、统计、查找）
 ├── studentmanager.h / .cpp  # 学生管理类（CRUD 委托、排序、统计、文件操作）
 ├── StudentManageSystem.pro  # qmake 项目文件
 ├── StudentManageSystem_zh_CN.ts  # Qt 翻译源文件
 └── clean.bat                # 清理构建产物脚本
 ```

 ## 快速开始

 ### 前置条件

 - Qt 5.15+（含 `widgets` 模块）
 - 支持 C++17 的编译器（MSVC / MinGW / GCC / Clang）
 - qmake（随 Qt 安装）

 ### 构建与运行

 ```bash
 # 1. 生成 Makefile
 qmake StudentManageSystem.pro

 # 2. 编译
 make          # Linux/macOS/MinGW
 nmake         # Windows MSVC

 # 3. 运行
 ./StudentManageSystem
 # 或双击生成的 exe
 ```

 > 也可直接在 **Qt Creator** 中打开 `.pro` 文件，一键构建运行。

 ### 清理构建产物

 ```bash
 .\clean.bat
 ```

 ## 类设计

 ### Student
 核心实体类，职责单一：

 - 数据成员：`name` / `id` / `score` / `level`（等级由 `score` 自动计算）
 - **运算符重载**：`<<`、`>>`、`==`（按学号比较）、`!=`、`<`、`=`（拷贝赋值）
 - **虚函数**：`display()` — 支持多态扩展（如派生 `GraduateStudent`）
 - **文件 I/O**：二进制 / 文本两种格式的单个读写与批量静态方法
 - **统计/查找静态方法**：`getTotalCount`、`getPassRate`、`getScoreDistribution`、`findById`、`deleteById`、`findByName`

 ### StudentManager
 管理学生集合的中间层，封装了：

 - CRUD（`addStudent` / `deleteStudent` / `findStudentById` / `findStudentsByName`）
 - 排序（`sortById` / `sortByScore` / `sortByName`）
 - 统计（`calculateStatistics` → `Statistics` 结构体）
 - 文件操作（`saveToBinaryFile` / `loadFromBinaryFile` / `saveToTextFile` / `loadFromTextFile`）

 ### MainWindow
 Qt 主窗口，连接 UI 与后端：

 - 信号/槽驱动：`on_btnAdd_clicked` 等六个按钮槽函数
 - 数据校验 → 表格刷新 → 自动保存的完整流程
 - 程序启动时自动加载、退出时自动保存
 - 状态栏反馈操作结果

 ## 成绩等级对照

 | 等级 | 分数范围 | 说明 |
 |------|---------|------|
 | A    | 90–100  | 优秀 |
 | B    | 80–89   | 良好 |
 | C    | 70–79   | 中等 |
 | D    | 60–69   | 及格 |
 | E    | 0–59    | 不及格 |

 ## 数据格式

 ### 文本文件 (.txt)

 ```
 2                      ← 学生数量
 张三 2024001 85.5 B    ← 姓名 学号 成绩 等级
 李四 2024002 92.0 A
 ```

 ### 二进制文件 (.dat)
 内部格式：`[学生数量:size_t] + [姓名长度:size_t][姓名数据][学号:long long][成绩:float][等级:char] × N`

 ## 开发说明

 - 程序启动时自动尝试加载 `students.dat`；若文件不存在则正常进入空状态。
 - 每次增/删操作后自动保存到 `students.dat` 和 `students_backup.txt`。
 - 查找学生支持按学号精确匹配或按姓名子串模糊匹配。
 - Qt UI 使用 Designer 设计，`mainwindow.ui` 可直接在 Qt Creator 中编辑。
 - 编译要求 C++17 标准（`CONFIG += c++17`）。

 ## 许可证

 本项目为课程设计作业，仅供学习参考。
