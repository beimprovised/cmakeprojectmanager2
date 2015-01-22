/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef CMAKEOPENPROJECTWIZARD_H
#define CMAKEOPENPROJECTWIZARD_H

#include "cmakebuildconfiguration.h"
#include "cmakebuildinfo.h"
#include "cmakeparamsext.h"

#include <utils/environment.h>
#include <utils/wizard.h>
#include <utils/qtcprocess.h>
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>

#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QRadioButton>

namespace Utils {
class FancyLineEdit;
class PathChooser;
}

namespace ProjectExplorer { class Kit; }

namespace CMakeProjectManager {
namespace Internal {

class CMakeManager;

class CMakeOpenProjectWizard : public Utils::Wizard
{
    Q_OBJECT
public:
    enum Mode {
        Nothing,
        NeedToCreate,
        NeedToUpdate,
        WantToUpdate,
        ChangeDirectory
    };

    /// used at importing a project without a .user file
    CMakeOpenProjectWizard(QWidget *parent, CMakeManager *cmakeManager, const QString &sourceDirectory, Utils::Environment env);

    /// used to update if we have already a .user file
    /// recreates or updates the cbp file
    /// Also used to change the build directory of one buildconfiguration or create a new buildconfiguration
    CMakeOpenProjectWizard(QWidget *parent, CMakeManager *cmakeManager, Mode mode, const CMakeBuildInfo *info);

    QString buildDirectory() const;
    QString sourceDirectory() const;
    void setBuildDirectory(const QString &directory);
    bool useNinja() const;
    void setUseNinja(bool b);
    CMakeManager *cmakeManager() const;
    QString arguments() const;
    void setArguments(const QString &args);
    Utils::Environment environment() const;
    ProjectExplorer::Kit *kit() const;
    void setKit(ProjectExplorer::Kit *kit);
    bool existsUpToDateXmlFile() const;
    bool compatibleKitExist() const;
    const CMakeParamsExt& cmakeParamsExt() const;
    void setCMakeParamsExt(const CMakeParamsExt& cmakeParams);

private:
    void init();
    bool hasInSourceBuild() const;
    CMakeManager *m_cmakeManager;
    QString m_buildDirectory;
    QString m_sourceDirectory;
    QString m_arguments;
    Utils::Environment m_environment;
    bool m_useNinja;
    ProjectExplorer::Kit *m_kit;
    CMakeParamsExt m_cmakeParamsExt;
};

class NoKitPage : public QWizardPage
{
    Q_OBJECT
public:
    NoKitPage(CMakeOpenProjectWizard *cmakeWizard);
    bool isComplete() const;
private slots:
    void kitsChanged();
    void showOptions();
private:
    QLabel *m_descriptionLabel;
    QPushButton *m_optionsButton;
    CMakeOpenProjectWizard *m_cmakeWizard;
};

class InSourceBuildPage : public QWizardPage
{
    Q_OBJECT
public:
    InSourceBuildPage(CMakeOpenProjectWizard *cmakeWizard);
private:
    CMakeOpenProjectWizard *m_cmakeWizard;
};

class ShadowBuildPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit ShadowBuildPage(CMakeOpenProjectWizard *cmakeWizard, bool change = false);
private slots:
    void buildDirectoryChanged();
private:
    CMakeOpenProjectWizard *m_cmakeWizard;
    Utils::PathChooser *m_pc;
};

class ChooseCMakePage : public QWizardPage
{
    Q_OBJECT
public:
    ChooseCMakePage(CMakeOpenProjectWizard *cmakeWizard);

    virtual bool isComplete() const;
public slots:
    void cmakeExecutableChanged();
private:
    void updateErrorText();
    QLabel *m_cmakeLabel;
    CMakeOpenProjectWizard *m_cmakeWizard;
    Utils::PathChooser *m_cmakeExecutable;
};

class CMakeRunPage : public QWizardPage
{
    Q_OBJECT
public:
    enum Mode { Initial, NeedToUpdate, Recreate, ChangeDirectory, WantToUpdate };
    explicit CMakeRunPage(CMakeOpenProjectWizard *cmakeWizard, Mode mode = Initial, const QString &buildDirectory = QString());

    virtual void initializePage();
    virtual bool validatePage();
    virtual void cleanupPage();
    virtual bool isComplete() const;
private slots:
    void runCMake();
    void cmakeFinished();
    void cmakeReadyReadStandardOutput();
    void cmakeReadyReadStandardError();
    void toolchainEdit();
    void toolchainRadio(bool);
private:
    void initWidgets();
    QByteArray cachedGeneratorFromFile(const QString &cache);
    CMakeOpenProjectWizard *m_cmakeWizard;
    QPlainTextEdit *m_output;
    QPushButton *m_runCMake;
    Utils::QtcProcess *m_cmakeProcess;
    Utils::FancyLineEdit *m_argumentsLineEdit;
    QComboBox *m_generatorComboBox;
    QLabel *m_generatorExtraText;
    QComboBox *m_buildTypeComboBox;
    QGroupBox *m_toolchainGroupbox;
    QComboBox *m_toolchainComboBox;
    Utils::FancyLineEdit *m_toolchainLineEdit;
    QPushButton *m_toolchainPushButton;
    QRadioButton *m_qtcToolchainRadioButton;
    QRadioButton *m_fileToolchainRadioButton;
    QRadioButton *m_inlineToolchainRadioButton;
    QLabel *m_descriptionLabel;
    QLabel *m_exitCodeLabel;
    bool m_haveCbpFile;
    Mode m_mode;
    QString m_buildDirectory;
    CMakeParamsExt m_cmakeParamsExt;
    QString        m_toolchainInlineCurrent;
};

}
}

#endif // CMAKEOPENPROJECTWIZARD_H
