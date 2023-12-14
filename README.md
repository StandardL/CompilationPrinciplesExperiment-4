# 编译原理课程实验4 - SLR(1)分析生成器

### 实验内容

设计一个应用软件，以实现SLR(1)分析生成器。

## 实验要求

1. 必做功能：
   1.   要提供一个文法输入编辑界面，让用户输入文法规则（可保存、打开存有文法规则的文件）
   2. 求出文法各非终结符号的first集合与follow集合，并提供窗口以便用户可以查看这些集合结果。【可以采用表格的形式呈现】
   3. 需要提供窗口以便用户可以查看文法对应的LR(0)DFA图。（可以用画图的方式呈现，也可用表格方式呈现该图点与边数据）
   4. 要提供窗口以便用户可以查看该文法是否为SLR(1)文法。（如果非SLR(1)文法，可查看其原因）
   5. 需要提供窗口以便用户可以查看文法对应的SLR(1)分析表。（如果该文法为SLR(1)文法时）【SLR(1)分析表采用表格的形式呈现】
   6. 应该书写完善的软件文档
   7. 应用程序应为Windows界面。

2.选做功能。

1. 需要提供窗口以便用户输入需要分析的句子。
2. 需要提供窗口以便用户查看使用SLR(1)分析该句子的过程。【可以使用表格的形式逐行显示分析过程】

## 💻程序

### 截图

**软件主界面**

![软件主界面](https://github.com/StandardL/CompilationPrinciplesExperiment-3/raw/main/images/软件主界面.png)

![软件主界面2](https://github.com/StandardL/CompilationPrinciplesExperiment-3/raw/main/images/软件主界面2.png)

### 🧪测试数据

共2组测试数据，已放入[Test instance文件夹](https://github.com/StandardL/CompilationPrinciplesExperiment-4/tree/main/test%20instances).

## 🧭编译

### 环境要求

#### Qt 6.5.3

- Qt Design Studio 4.2.0
- Qt 6.5.3
  - MSVC 2019 64-bit
  - MinGW 11.2.0 64-bit
  - Qt 5 Compatibility Module
  - Qt Shader Tools
  - Additional Libraries
- Developer and Designer Tools
  - Qt Creator 11.0.2
  - Qt Creator 11.0.2 CDB Debugger Support
  - Debugging Tools for Windows
  - MinGW 11.2.0 64-bit
  - Qt Installer Framwork 4.6
  - CMake 3.24.2
  - Ninja 1.10.2

#### Visual Studio 2022

选择安装**使用C++的桌面开发**

<img src="https://github.com/StandardL/CompilationPrinciplesExperiment-3/raw/main/images/VS组件.png" alt="image-20230926211152531" style="zoom:67%;" />

进入到Visual Studio 2022，在**扩展**>**管理扩展**>**联机**（**Extensions** > **Manage Extensions** > **Online**）处搜索安装Qt Visual Studio Tools.

<img src="https://github.com/StandardL/CompilationPrinciplesExperiment-3/raw/main/images/VS插件.png" alt="image-20230926211819218" style="zoom:80%;" />

重启Visual Studio 2022。

点击**扩展**>**Qt VS Tools**>**Qt Versions**，确保已勾选MSVC编译器。若无可手动添加MSVC编译器路径：\\ ${Qt安装目录} \\ {Qt版本号} \ msvc2019_64 \ bin \ qmake.exe

#### 编译方法

使用Visual Studio打开目录下TinyGUI.sln，选择Debug - x64或Release - x64即可开始编译运行。

## ☁未来展望

- [ ] QT界面美化
- [ ] UnitTest

## 🔗其他链接

- [First集合和Follow集合的求解](https://blog.csdn.net/RENSRM/article/details/111595754)
- [DFA图的生成](https://www.cnblogs.com/Serenaxy/p/14145468.html#dfa%E5%9B%BEslr1%E5%88%86%E6%9E%90%E8%A1%A8)
