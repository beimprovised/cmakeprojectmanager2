QTC_PLUGIN_NAME = CMakeProjectManager2

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -fPIC
QMAKE_CXXFLAGS += -rdynamic

QTC_LIB_DEPENDS += \
    extensionsystem \
    qmljs \
    utils
QTC_PLUGIN_DEPENDS += \
    coreplugin \
    projectexplorer \
    cpptools \
    texteditor \
    qtsupport
QTC_PLUGIN_RECOMMENDS += \
    designer
