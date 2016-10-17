#include "common.h"
#include "runoncechecker.h"
#include "binaryformat.h"
#include "productkeycheck.h"
#include "binaryformatenginehandler.h"

#include <QString>
#include <QDir>

namespace SdkCommon
{

bool isAnotherInstanceRunning(const QString &applicationName, const QString &lockName)
{
    RunOnceChecker runCheck(QDir::tempPath()
                            + QLatin1Char('/')
                            + applicationName
                            + lockName
                            + QLatin1String(".lock"));
    if (runCheck.isRunning(RunOnceChecker::ConditionFlag::Lockfile)) {
        // It is possible that two installers with the same name get executed
        // concurrently and thus try to access the same lock file. This causes
        // a warning to be shown (when verbose output is enabled) but let's
        // just silently ignore the fact that we could not create the lock file
        // and check the running processes.
        return runCheck.isRunning(RunOnceChecker::ConditionFlag::ProcessList);
    }
    return false;
}

void addProductKeyCheckPackagesAndRegisterResourceCollections(QInstaller::PackageManagerCore *core, const QInstaller::ResourceCollectionManager &manager)
{
    using namespace QInstaller;
    ProductKeyCheck::instance()->init(core);
    ProductKeyCheck::instance()->addPackagesFromXml(QLatin1String(":/metadata/Updates.xml"));
    BinaryFormatEngineHandler::instance()->registerResources(manager.collections());
}

}
