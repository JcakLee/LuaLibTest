QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/libs/include

LIBS += -L$$PWD/libs/Debug
LIBS += -L$$PWD/libs/Release
# 根据构建模式选择库文件
CONFIG(debug, debug|release) {
    # 调试模式：使用 Debug 库
    LIBS += -lliblua  # 请替换为你的库名
} else {
    # 发布模式：使用 Release 库
    LIBS += -lliblua   # 请替换为你的库名
}

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
