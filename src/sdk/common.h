#ifndef SDK_COMMON_H
#define SDK_COMMON_H

class QString;

namespace SdkCommon
{

bool isAnotherInstanceRunning(const QString &applicationName, const QString &lockName);

}

#endif // SDK_COMMON_H
