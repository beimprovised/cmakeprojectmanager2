/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "cmakebuildconfiguration.h"

#include "cmakebuildinfo.h"
#include "cmakebuildstep.h"
#include "cmakeopenprojectwizard.h"
#include "cmakeproject.h"
#include "cmakeprojectconstants.h"
#include "cmakebuildsettingswidget.h"
#include "cmakeprojectmanager.h"

#include <coreplugin/documentmanager.h>
#include <coreplugin/icore.h>

#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmacroexpander.h>
#include <projectexplorer/target.h>

#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcassert.h>

#include <QInputDialog>

using namespace ProjectExplorer;
using namespace Utils;

namespace CMakeProjectManager {
namespace Internal {

const char INITIAL_ARGUMENTS[] = "CMakeProjectManager.CMakeBuildConfiguration.InitialArgument";
const char CMAKE_PARAMS_KEY[] = "CMakeProjectManager.CMakeBuildConfiguration.CMakeParams";
const char CMAKE_BUILD_TYPE_KEY[] = "CMakeProjectManaget.CMakeBuildConfiguration.CMakeBuildType";
const char CMAKE_TOOLCHAIN_TYPE_KEY[] = "CMakeProjectManaget.CMakeBuildConfiguration.CMakeToolchainOverride";
const char CMAKE_TOOLCHAIN_FILE_KEY[] = "CMakeProjectManaget.CMakeBuildConfiguration.CMakeToolchainFile";
const char CMAKE_TOOLCHAIN_INLINE_KEY[] = "CMakeProjectManaget.CMakeBuildConfiguration.CMakeToolchainInline";

static FileName shadowBuildDirectory(const FileName &projectFilePath, const Kit *k,
                                     const QString &bcName, BuildConfiguration::BuildType buildType)
{
    if (projectFilePath.isEmpty())
        return FileName();

    const QString projectName = projectFilePath.parentDir().fileName();
    ProjectMacroExpander expander(projectName, k, bcName, buildType);
    QDir projectDir = QDir(Project::projectDirectory(projectFilePath).toString());
    QString buildPath = expander.expand(Core::DocumentManager::buildDirectory());
    return FileName::fromUserInput(projectDir.absoluteFilePath(buildPath));
}

CMakeBuildConfiguration::CMakeBuildConfiguration(ProjectExplorer::Target *parent) :
    BuildConfiguration(parent, Core::Id(Constants::CMAKE_BC_ID))
{
    CMakeProject *project = static_cast<CMakeProject *>(parent->project());
    setBuildDirectory(shadowBuildDirectory(project->projectFilePath(),
                                           parent->kit(),
                                           displayName(), BuildConfiguration::Unknown));
}

CMakeBuildConfiguration::CMakeBuildConfiguration(ProjectExplorer::Target *parent,
                                                 CMakeBuildConfiguration *source) :
    BuildConfiguration(parent, source),
    m_initialArguments(source->m_initialArguments)
{
    Q_ASSERT(parent);
    cloneSteps(source);
}

QVariantMap CMakeBuildConfiguration::toMap() const
{
    QVariantMap map(ProjectExplorer::BuildConfiguration::toMap());
    map.insert(QLatin1String(INITIAL_ARGUMENTS), m_initialArguments);
    map.insert(QLatin1String(CMAKE_PARAMS_KEY), m_cmakeParams);
    map.insert(QLatin1String(CMAKE_BUILD_TYPE_KEY), static_cast<int>(m_cmakeParamsExt.buildType));
    map.insert(QLatin1String(CMAKE_TOOLCHAIN_TYPE_KEY), static_cast<int>(m_cmakeParamsExt.toolchainOverride));
    map.insert(QLatin1String(CMAKE_TOOLCHAIN_FILE_KEY), m_cmakeParamsExt.toolchainFile);
    map.insert(QLatin1String(CMAKE_TOOLCHAIN_INLINE_KEY), m_cmakeParamsExt.toolchainInline);
    return map;
}

bool CMakeBuildConfiguration::fromMap(const QVariantMap &map)
{
    if (!BuildConfiguration::fromMap(map))
        return false;

    m_initialArguments = map.value(QLatin1String(INITIAL_ARGUMENTS)).toString();
    m_cmakeParams = map.value(QLatin1String(CMAKE_PARAMS_KEY), QLatin1String("")).toString();

    m_cmakeParamsExt.buildType = static_cast<CMakeBuildType>(
                                       map.value(QLatin1String(CMAKE_BUILD_TYPE_KEY),
                                                 static_cast<int>(CMakeBuildType::Default)).toInt());
    m_cmakeParamsExt.toolchainOverride =
            static_cast<CMakeToolchainOverrideType>(
                map.value(QLatin1String(CMAKE_TOOLCHAIN_TYPE_KEY), static_cast<int>(CMakeToolchainOverrideType::Disabled)).toInt());
    m_cmakeParamsExt.toolchainFile =
            map.value(QLatin1String(CMAKE_TOOLCHAIN_FILE_KEY), QLatin1String("")).toString();
    m_cmakeParamsExt.toolchainInline =
            map.value(QLatin1String(CMAKE_TOOLCHAIN_INLINE_KEY), QLatin1String("")).toString();

    return true;
}

const CMakeParamsExt &CMakeBuildConfiguration::cmakeParamsExt() const
{
    return m_cmakeParamsExt;
}

void CMakeBuildConfiguration::setCMakeParamsExt(const CMakeParamsExt &cmakeParamsExt)
{
    if (m_cmakeParamsExt == cmakeParamsExt)
        return;
    m_cmakeParamsExt = cmakeParamsExt;
    // TODO: is this need?
    emit buildDirectoryChanged();
    emit environmentChanged();
}

void CMakeBuildConfiguration::emitBuildTypeChanged()
{
    emit buildTypeChanged();
}

void CMakeBuildConfiguration::setInitialArguments(const QString &arguments)
{
    m_initialArguments = arguments;
}

QString CMakeBuildConfiguration::initialArguments() const
{
    return m_initialArguments;
}

ProjectExplorer::NamedWidget *CMakeBuildConfiguration::createConfigWidget()
{
    return new CMakeBuildSettingsWidget(this);
}

QString CMakeBuildConfiguration::cmakeParams() const
{
    return m_cmakeParams;
}

void CMakeBuildConfiguration::setCMakeParams(const QString &cmakeParams)
{
    if (m_cmakeParams == cmakeParams)
	return;
    m_cmakeParams = cmakeParams;
    // TODO: is this need?
    emit buildDirectoryChanged();
    emit environmentChanged();
}

/*!
  \class CMakeBuildConfigurationFactory
*/

CMakeBuildConfigurationFactory::CMakeBuildConfigurationFactory(QObject *parent) :
    ProjectExplorer::IBuildConfigurationFactory(parent)
{
}

int CMakeBuildConfigurationFactory::priority(const ProjectExplorer::Target *parent) const
{
    return canHandle(parent) ? 0 : -1;
}

QList<ProjectExplorer::BuildInfo *> CMakeBuildConfigurationFactory::availableBuilds(const ProjectExplorer::Target *parent) const
{
    QList<ProjectExplorer::BuildInfo *> result;

    for (int type = BuildTypeNone; type != BuildTypeLast; ++type) {
        CMakeBuildInfo *info = createBuildInfo(parent->kit(),
                                               parent->project()->projectDirectory().toString(),
                                               BuildType(type));
        result << info;
    }
    return result;
}

int CMakeBuildConfigurationFactory::priority(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    Utils::MimeDatabase mdb;
    if (k && mdb.mimeTypeForFile(projectPath).matchesName(QLatin1String(Constants::CMAKEPROJECTMIMETYPE)))
        return 0;
    return -1;
}

QList<ProjectExplorer::BuildInfo *> CMakeBuildConfigurationFactory::availableSetups(const ProjectExplorer::Kit *k,
                                                                                    const QString &projectPath) const
{
    QList<ProjectExplorer::BuildInfo *> result;
    const FileName projectPathName = FileName::fromString(projectPath);
    for (int type = BuildTypeNone; type != BuildTypeLast; ++type) {
        CMakeBuildInfo *info = createBuildInfo(k,
                                               ProjectExplorer::Project::projectDirectory(projectPathName).toString(),
                                               BuildType(type));
        if (type == BuildTypeNone) {
            //: The name of the build configuration created by default for a cmake project.
            info->displayName = tr("Default");
        } else {
            info->displayName = info->typeName;
        }
        info->buildDirectory
                = shadowBuildDirectory(projectPathName, k, info->displayName, info->buildType);
        result << info;
    }
    return result;
}

ProjectExplorer::BuildConfiguration *CMakeBuildConfigurationFactory::create(ProjectExplorer::Target *parent,
                                                                            const ProjectExplorer::BuildInfo *info) const
{
    QTC_ASSERT(info->factory() == this, return 0);
    QTC_ASSERT(info->kitId == parent->kit()->id(), return 0);
    QTC_ASSERT(!info->displayName.isEmpty(), return 0);

    CMakeBuildInfo copy(*static_cast<const CMakeBuildInfo *>(info));
    CMakeProject *project = static_cast<CMakeProject *>(parent->project());

    if (copy.buildDirectory.isEmpty()) {
        copy.buildDirectory = shadowBuildDirectory(project->projectFilePath(), parent->kit(),
                                                   copy.displayName, info->buildType);
    }

    auto bc = new CMakeBuildConfiguration(parent);
    bc->setDisplayName(copy.displayName);
    bc->setDefaultDisplayName(copy.displayName);

    ProjectExplorer::BuildStepList *buildSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
    ProjectExplorer::BuildStepList *cleanSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_CLEAN);

    auto buildStep = new CMakeBuildStep(buildSteps);
    buildSteps->insertStep(0, buildStep);

    auto cleanStep = new CMakeBuildStep(cleanSteps);
    cleanSteps->insertStep(0, cleanStep);
    cleanStep->setBuildTarget(CMakeBuildStep::cleanTarget(), true);

    bc->setBuildDirectory(copy.buildDirectory);
    bc->setInitialArguments(copy.arguments);
    bc->setCMakeParams(copy.cmakeParams);
    bc->setCMakeParamsExt(copy.cmakeParamsExt);

    // Default to all
    if (project->hasBuildTarget(QLatin1String("all")))
        buildStep->setBuildTarget(QLatin1String("all"), true);

    return bc;
}

bool CMakeBuildConfigurationFactory::canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source) const
{
    if (!canHandle(parent))
        return false;
    return source->id() == Constants::CMAKE_BC_ID;
}

CMakeBuildConfiguration *CMakeBuildConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;
    CMakeBuildConfiguration *old = static_cast<CMakeBuildConfiguration *>(source);
    return new CMakeBuildConfiguration(parent, old);
}

bool CMakeBuildConfigurationFactory::canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    if (!canHandle(parent))
        return false;
    return ProjectExplorer::idFromMap(map) == Constants::CMAKE_BC_ID;
}

CMakeBuildConfiguration *CMakeBuildConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    auto bc = new CMakeBuildConfiguration(parent);
    if (bc->fromMap(map))
        return bc;
    delete bc;
    return 0;
}

bool CMakeBuildConfigurationFactory::canHandle(const ProjectExplorer::Target *t) const
{
    QTC_ASSERT(t, return false);
    if (!t->project()->supportsKit(t->kit()))
        return false;
    return qobject_cast<CMakeProject *>(t->project());
}

CMakeBuildInfo *CMakeBuildConfigurationFactory::createBuildInfo(const ProjectExplorer::Kit *k,
                                                                const QString &sourceDir,
                                                                BuildType buildType) const
{
    CMakeBuildInfo *info = new CMakeBuildInfo(this);
    info->kitId = k->id();
    info->environment = Environment::systemEnvironment();
    k->addToEnvironment(info->environment);
    info->sourceDirectory = sourceDir;
    switch (buildType) {
    case BuildTypeNone:
        info->typeName = tr("Build");
        break;
    case BuildTypeDebug:
        info->arguments = QLatin1String("-DCMAKE_BUILD_TYPE=Debug");
        info->typeName = tr("Debug");
        info->buildType = BuildConfiguration::Debug;
        break;
    case BuildTypeRelease:
        info->arguments = QLatin1String("-DCMAKE_BUILD_TYPE=Release");
        info->typeName = tr("Release");
        info->buildType = BuildConfiguration::Release;
        break;
    case BuildTypeMinSizeRel:
        info->arguments = QLatin1String("-DCMAKE_BUILD_TYPE=MinSizeRel");
        info->typeName = tr("Minimum Size Release");
        info->buildType = BuildConfiguration::Release;
        break;
    case BuildTypeRelWithDebInfo:
        info->arguments = QLatin1String("-DCMAKE_BUILD_TYPE=RelWithDebInfo");
        info->typeName = tr("Release with Debug Information");
        info->buildType = BuildConfiguration::Profile;
        break;
    default:
        QTC_CHECK(false);
        break;
    }

    return info;
}

ProjectExplorer::BuildConfiguration::BuildType CMakeBuildConfiguration::buildType() const
{
    QString cmakeBuildType;
    QFile cmakeCache(buildDirectory().toString() + QLatin1String("/CMakeCache.txt"));
    if (cmakeCache.open(QIODevice::ReadOnly)) {
        while (!cmakeCache.atEnd()) {
            QByteArray line = cmakeCache.readLine();
            if (line.startsWith("CMAKE_BUILD_TYPE")) {
                if (int pos = line.indexOf('='))
                    cmakeBuildType = QString::fromLocal8Bit(line.mid(pos + 1).trimmed());
                break;
            }
        }
        cmakeCache.close();
    }

    // Cover all common CMake build types
    if (cmakeBuildType.compare(QLatin1String("Release"), Qt::CaseInsensitive) == 0
        || cmakeBuildType.compare(QLatin1String("MinSizeRel"), Qt::CaseInsensitive) == 0) {
        return Release;
    } else if (cmakeBuildType.compare(QLatin1String("Debug"), Qt::CaseInsensitive) == 0
               || cmakeBuildType.compare(QLatin1String("DebugFull"), Qt::CaseInsensitive) == 0) {
        return Debug;
    } else if (cmakeBuildType.compare(QLatin1String("RelWithDebInfo"), Qt::CaseInsensitive) == 0) {
        return Profile;
    }

    return Unknown;
}

} // namespace Internal
} // namespace CMakeProjectManager
