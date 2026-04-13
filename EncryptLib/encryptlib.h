#ifndef ENCRYPTLIB_H
#define ENCRYPTLIB_H
#include <Windows.h>
#include "EncryptLib_global.h"

class ENCRYPTLIB_EXPORT EncryptLib
{
public:
    EncryptLib();
    static bool CheckValidity(bool bCheckRemain);
private:
    static ULONG GetRandomNumber();

    bool m_bInitial;
};

#endif // ENCRYPTLIB_H
