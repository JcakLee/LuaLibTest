#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lua.hpp"
#include <QMessageBox>
#include <QVBoxLayout>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化 Lua
    initLua();

    // 初始化图表
    chart = new QChart();
    chart->setTitle("Lua 函数绘图");
    chart->legend()->hide();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 将图表添加到 widget
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(chartView);
    ui->widget->setLayout(layout);

    // 连接按钮信号（如果还没有自动连接）
    bool isconnect = false;
    if(!isconnect)
    {
        isconnect = connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    }

}

MainWindow::~MainWindow()
{
    closeLua();
    delete ui;
}

void MainWindow::initLua()
{
    L = luaL_newstate();
    if (!L) {
        QMessageBox::critical(this, "错误", "无法创建 Lua 状态机");
        return;
    }

    // 打开标准库
    luaL_openlibs(L);

    // 注册一些数学函数到 Lua 环境
    // （实际上 luaL_openlibs 已经包含了 math 库）
}

void MainWindow::closeLua()
{
    if (L) {
        lua_close(L);
        L = nullptr;
    }
}

double MainWindow::callLuaFunction(const QString& funcName, double x)
{
    if (!L) return 0.0;

    // 获取 Lua 函数
    lua_getglobal(L, funcName.toStdString().c_str());

    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1); // 弹出非函数的值
        return 0.0;
    }

    // 压入参数
    lua_pushnumber(L, x);

    // 调用函数 (1个参数, 1个返回值)
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        QString error = QString("Lua 错误: %1").arg(lua_tostring(L, -1));
        lua_pop(L, 1); // 弹出错误信息
        QMessageBox::warning(this, "Lua 错误", error);
        return 0.0;
    }

    // 获取返回值
    if (!lua_isnumber(L, -1)) {
        lua_pop(L, 1); // 弹出非数值的返回值
        return 0.0;
    }

    double result = lua_tonumber(L, -1);
    lua_pop(L, 1); // 弹出返回值

    return result;
}

void MainWindow::plotLuaFunction(const QString& funcName, double start, double end, int points)
{
    if (!L) return;

    // 清除之前的图表
    chart->removeAllSeries();

    // 创建新的线系列
    QLineSeries *series = new QLineSeries();

    double step = (end - start) / points;

    // 计算并添加点
    for (int i = 0; i <= points; ++i) {
        double x = start + i * step;
        double y = callLuaFunction(funcName, x);
        series->append(x, y);
    }

    // 添加到图表
    chart->addSeries(series);

    // 创建坐标轴
    chart->createDefaultAxes();

    // 设置图表标题
    chart->setTitle(QString("函数: %1(x)").arg(funcName));
}

void MainWindow::on_pushButton_clicked()
{
    if (!L) {
        QMessageBox::warning(this, "错误", "Lua 未初始化");
        return;
    }

    // 定义一个 Lua 函数：正弦函数
    const char* luaCode = R"(
        function mysin(x)
            return math.sin(x)
        end

        function myquadratic(x)
            return x * x - 2 * x + 1
        end

        function mysigmoid(x)
            return 1 / (1 + math.exp(-x))
        end

        function mygaussian(x)
            return math.exp(-(x * x) / 2) / math.sqrt(2 * math.pi)
        end
    )";

    // 执行 Lua 代码
    if (luaL_dostring(L, luaCode) != LUA_OK) {
        QString error = QString("Lua 代码错误: %1").arg(lua_tostring(L, -1));
        lua_pop(L, 1);
        QMessageBox::critical(this, "Lua 错误", error);
        return;
    }

    // 让用户选择要绘制的函数
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("选择函数");
    msgBox.setText("请选择要绘制的函数:");
    msgBox.addButton("mysin(x) = sin(x)", QMessageBox::ActionRole);
    msgBox.addButton("myquadratic(x) = x² - 2x + 1", QMessageBox::ActionRole);
    msgBox.addButton("mysigmoid(x) = 1/(1+e⁻ˣ)", QMessageBox::ActionRole);
    msgBox.addButton("mygaussian(x) = e⁻ˣ²/²/√(2π)", QMessageBox::ActionRole);

    QString selectedFunc;
    double start, end;

    if (msgBox.exec() == 0) { // 第一个按钮
        selectedFunc = "mysin";
        start = -10;
        end = 10;
    } else if (msgBox.exec() == 1) {
        selectedFunc = "myquadratic";
        start = -5;
        end = 5;
    } else if (msgBox.exec() == 2) {
        selectedFunc = "mysigmoid";
        start = -5;
        end = 5;
    } else {
        selectedFunc = "mygaussian";
        start = -3;
        end = 3;
    }

    // 绘制选中的函数
    plotLuaFunction(selectedFunc, start, end, 200);

    // 显示成功消息
    ui->statusbar->showMessage(QString("已绘制函数: %1").arg(selectedFunc), 3000);
}

