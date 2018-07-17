## Add following 4 parameters to your qt-creator qmake additional arguments
## QTC_SOURCE=<path-to-qtcreator-source>/src QTC_BUILD=<path-to-qtcreator-build> QTC_PREFIX=<path-to-qtcreator-installation> QT5DIR=<path-to-qtcreator-installation>/gcc_64
## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $${QTC_SOURCE}

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD_INPLACE)

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -fPIC
QMAKE_CXXFLAGS += -rdynamic

DEFINES += CMAKEPROJECTMANAGER_LIBRARY

# For the highlighter:
INCLUDEPATH += $$QTCREATOR_SOURCES/src/plugins/texteditor
INCLUDEPATH += $$QTC_SOURCE/src
INCLUDEPATH += $$QTC_SOURCE/src/plugins

message(Qt Creator source directory (QTC_SOURCE: which sould not be empty): QTC_SOURCE: $$QTC_SOURCE)
message(Qt Creator source directory (QTCREATOR_SOURCES: which sould not be empty): QTCREATOR_SOURCES: $$QTCREATOR_SOURCES)
message(Qt Creator deploy directory (QTC_PREFIX: which sould not be empty): QTC_PREFIX: $$QTC_PREFIX)
message(Qt Creator build directory  (QTC_BUILD: which sould not be empty):  QTC_BUILD:  $$QTC_BUILD)
# https://stackoverflow.com/questions/37453165/is-there-a-qt-install-path-variable-that-i-can-use-in-the-pro-file
QTPATH_PRE = $$dirname(QMAKE_QMAKE)
message(QTPATH_PRE: $$QTPATH_PRE)
RESULT  = $$find(QTPATH_PRE, "/bin")
message(RESULT: $$RESULT)
QT5DIR  = ''
if(count(RESULT, 1)){
    QT5DIR  = $$dirname(QTPATH_PRE)

}else{
    QT5DIR  = $$QTPATH_PRE
}
    message(QT5DIR: $$QT5DIR)


# http://blog.qt.io/blog/2011/10/28/rpath-and-runpath/
if(equals(QT5DIR, '')){
    error(Qt5 directory (which sould not be empty):                             QT5DIR: $${QT5DIR})
    return()
}else{
    message(Qt5 directory (QT5DIR: which sould not be empty):                   QT5DIR: $$QT5DIR)
}




PROJECT_QT_VERSION  = $${QT5DIR}
PROJECT_QT_LIBS     = $${PROJECT_QT_VERSION}/lib
INCLUDEPATH         += $${PROJECT_QT_VERSION}/include
INCLUDEPATH         += $$QTC_BUILD/src
INCLUDEPATH         += $$QTC_BUILD/src/tools/sdktool
DEPENDPATH          += . $${PROJECT_QT_LIBS}

CONFIG    +=  unittest
DEFINES   +=  WITH_TESTS

QMAKE_CXXFLAGS      += -L$$QTC_BUILD/lib/qtcreator/plugins -lProjectExplorer
        # -l$$QTC_BUILD/lib/qtcreator/plugins/libProjectExplorer.so
        # -L$${QTC_PREFIX}/lib/qtcreator/plugins -L$${PROJECT_QT_LIBS} -lProjectExplorer
LIBS += -L$$QTC_BUILD/lib/qtcreator/plugins -lProjectExplorer \ # -l$$QTC_BUILD/lib/qtcreator/plugins/libProjectExplorer.so \ #
        -L$${QTC_PREFIX}/lib/qtcreator \
        -L$${QTC_PREFIX}/lib/qtcreator/plugins \
        -L$${PROJECT_QT_LIBS}

include(cmakeprojectmanager_dependencies.pri)
include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

HEADERS = builddirmanager.h \
    builddirparameters.h \
    builddirreader.h \
    cmakebuildinfo.h \
    cmakebuildstep.h \
    cmakebuildtarget.h \
    cmakeconfigitem.h \
    cmakeproject.h \
    cmakeprojectimporter.h \
    cmakeprojectplugin.h \
    cmakeprojectmanager.h \
    cmakeprojectconstants.h \
    cmakeprojectnodes.h \
    cmakerunconfiguration.h \
    cmakebuildconfiguration.h \
    cmakeeditor.h \
    cmakelocatorfilter.h \
    cmakefilecompletionassist.h \
    cmaketool.h \
    cmakeparser.h \
    cmakesettingspage.h \
    cmaketoolmanager.h \
    cmake_global.h \
    cmakekitinformation.h \
    cmakekitconfigwidget.h \
    cmakecbpparser.h \
    cmakebuildsettingswidget.h \
    cmakeindenter.h \
    cmakeautocompleter.h \
    cmakespecificsettings.h \
    cmakespecificsettingspage.h \
    configmodel.h \
    configmodelitemdelegate.h \
    servermode.h \
    servermodereader.h \
    tealeafreader.h \
    treescanner.h \
    compat.h \
    simpleservermodereader.h

SOURCES = builddirmanager.cpp \
    builddirparameters.cpp \
    builddirreader.cpp \
    cmakebuildstep.cpp \
    cmakebuildtarget.cpp \
    cmakeconfigitem.cpp \
    cmakeproject.cpp \
    cmakeprojectimporter.cpp \
    cmakeprojectplugin.cpp \
    cmakeprojectmanager.cpp \
    cmakeprojectnodes.cpp \
    cmakerunconfiguration.cpp \
    cmakebuildconfiguration.cpp \
    cmakeeditor.cpp \
    cmakelocatorfilter.cpp \
    cmakefilecompletionassist.cpp \
    cmaketool.cpp \
    cmakeparser.cpp \
    cmakesettingspage.cpp \
    cmaketoolmanager.cpp \
    cmakekitinformation.cpp \
    cmakekitconfigwidget.cpp \
    cmakecbpparser.cpp \
    cmakebuildsettingswidget.cpp \
    cmakeindenter.cpp \
    cmakeautocompleter.cpp \
    cmakespecificsettings.cpp \
    cmakespecificsettingspage.cpp \
    configmodel.cpp \
    configmodelitemdelegate.cpp \
    servermode.cpp \
    servermodereader.cpp \
    tealeafreader.cpp \
    treescanner.cpp \
    compat.cpp \
    simpleservermodereader.cpp

RESOURCES += cmakeproject.qrc

FORMS += \
    cmakespecificsettingspage.ui

OTHER_FILES += README.txt

WIZARD_FILES = wizard/cmake2/*

wizardfiles.files = $$WIZARD_FILES
wizardfiles.path = $$QTC_PREFIX/share/qtcreator/templates/wizards/projects/cmake2/
wizardfiles.CONFIG += no_check_exist
INSTALLS += wizardfiles


