#ifndef SDK_COMMON_H
#define SDK_COMMON_H

class QString;

namespace QInstaller
{
    class ResourceCollectionManager;
    class PackageManagerCore;
}

namespace SdkCommon
{

bool isAnotherInstanceRunning(const QString &applicationName, const QString &lockName);
void addProductKeyCheckPackagesAndRegisterResourceCollections(QInstaller::PackageManagerCore *core, const QInstaller::ResourceCollectionManager &manager);

}

#endif // SDK_COMMON_H
