TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

LIBS += -L/usr/include/boost -lboost_system -lboost_chrono -lboost_thread -lboost_timer

SOURCES += \
        main.cpp \
    servidorboost.cpp \
    mensagem.cpp \
    log.cpp


HEADERS += \
    servidorboost.h \
    mensagem.h \
    log.h


