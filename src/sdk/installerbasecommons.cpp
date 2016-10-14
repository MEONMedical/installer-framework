/**************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
**************************************************************************/
#include "installerbasecommons.h"

#include <packagemanagercore.h>
#include <scriptengine.h>
#include <packagemanagerpagefactory.h>
#include <productkeycheck.h>
#include <settings.h>

using namespace QInstaller;

static void addProductKeyCheckPagesIfAny(PackageManagerGui *manager, PackageManagerCore *core)
{
    ProductKeyCheck *checker = ProductKeyCheck::instance();
    foreach (const int id, checker->registeredPages()) {
        PackageManagerPage *page = PackageManagerPageFactory::instance().create(id, core);
        Q_ASSERT_X(page, Q_FUNC_INFO, qPrintable(QString::fromLatin1("Page with %1 couldn't be "
            "constructed.").arg(id)));
        manager->setPage(id, page);
    }
}

static void addPagesToControlAndComponentScriptEngines(PackageManagerGui *manager, PackageManagerCore *core)
{
    foreach (const int id, manager->pageIds()) {
        QWizardPage *wizardPage = manager->page(id);
        core->controlScriptEngine()->addToGlobalObject(wizardPage);
        core->componentScriptEngine()->addToGlobalObject(wizardPage);
    }
}

// -- InstallerGui

InstallerGui::InstallerGui(PackageManagerCore *core)
    : PackageManagerGui(core, 0)
{
    addProductKeyCheckPagesIfAny(this, core);

    setPage(PackageManagerCore::Introduction, new IntroductionPage(core));
    setPage(PackageManagerCore::TargetDirectory, new TargetDirectoryPage(core));
    setPage(PackageManagerCore::ComponentSelection, new ComponentSelectionPage(core));
    setPage(PackageManagerCore::LicenseCheck, new LicenseAgreementPage(core));
#ifdef Q_OS_WIN
    setPage(PackageManagerCore::StartMenuSelection, new StartMenuDirectoryPage(core));
#endif
    setPage(PackageManagerCore::ReadyForInstallation, new ReadyForInstallationPage(core));
    setPage(PackageManagerCore::PerformInstallation, new PerformInstallationPage(core));
    setPage(PackageManagerCore::InstallationFinished, new FinishedPage(core));

    addPagesToControlAndComponentScriptEngines(this, core);
}


// -- MaintenanceGui

MaintenanceGui::MaintenanceGui(PackageManagerCore *core)
    : PackageManagerGui(core, 0)
{
    addProductKeyCheckPagesIfAny(this, core);

    IntroductionPage *intro = new IntroductionPage(core);
    connect(intro, &IntroductionPage::packageManagerCoreTypeChanged,
            this, &MaintenanceGui::updateRestartPage);

    if (!core->isOfflineOnly() || validRepositoriesAvailable()) {
        setPage(PackageManagerCore::Introduction, intro);
        setPage(PackageManagerCore::ComponentSelection, new ComponentSelectionPage(core));
        setPage(PackageManagerCore::LicenseCheck, new LicenseAgreementPage(core));
    } else {
        core->setUninstaller();
        core->setCompleteUninstallation(true);
    }

    setPage(PackageManagerCore::ReadyForInstallation, new ReadyForInstallationPage(core));
    setPage(PackageManagerCore::PerformInstallation, new PerformInstallationPage(core));
    setPage(PackageManagerCore::InstallationFinished, new FinishedPage(core));

    RestartPage *p = new RestartPage(core);
    connect(p, &RestartPage::restart, this, &PackageManagerGui::gotRestarted);
    setPage(PackageManagerCore::InstallationFinished + 1, p);

    if (core->isUninstaller())
        wizardPageVisibilityChangeRequested(false, PackageManagerCore::InstallationFinished + 1);

    addPagesToControlAndComponentScriptEngines(this, core);
}

void MaintenanceGui::updateRestartPage()
{
    wizardPageVisibilityChangeRequested((packageManagerCore()->isUninstaller() ? false : true),
        PackageManagerCore::InstallationFinished + 1);
}

bool MaintenanceGui::validRepositoriesAvailable() const
{
    foreach (const Repository &repo, packageManagerCore()->settings().repositories()) {
        if (repo.isEnabled() && repo.isValid()) {
            return true;
        }
    }
    return false;
}
