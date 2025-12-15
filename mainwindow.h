#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    // Lua 相关函数
    void initLua();
    void closeLua();
    double callLuaFunction(const QString& funcName, double x);

    // 绘图函数
    void plotLuaFunction(const QString& funcName, double start, double end, int points);

private:
    Ui::MainWindow *ui;
    QChart *chart;
    QChartView *chartView;
    lua_State *L;
};
#endif // MAINWINDOW_H
