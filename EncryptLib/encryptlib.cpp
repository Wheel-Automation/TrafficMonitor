#include <stdio.h>
#include <string>
#include <random>
#include <Windows.h>
#include "dog/Lock.h"
#include "dog/dog_header.h"
#include <thread>
#include "encryptlib.h"

struct KEY{
    unsigned short key1;
    unsigned short key2;
    unsigned short key3;
    unsigned short key4;
};


EncryptLib::EncryptLib()
{

      m_bInitial = false;
}

ULONG EncryptLib::GetRandomNumber(){
    unsigned long x = rand() * 529 + rand() * 5367 + rand()* 14451 + rand() * 37;
    return x;
}

bool EncryptLib::CheckValidity(bool bCheckRemain){
    bool res = InitiateLock(0);
    if (!res) {
        TerminateLock(0);
        return false;
    }
    Sleep(10);
    ULONG addres = KEY_START_ADDRES;
    int nKeyCount = 0;
    std::string szKey = "cheliang";
    res = ReadLockEx(addres, &nKeyCount, sizeof(LONG), szKey.c_str(), 0, 3);
    char psz[255] = {0,};
    if (!res || nKeyCount <= 0)
    {
        TerminateLock(0);
        OutputDebugStringA(psz);
        return false;
    }

    res = false;
    addres += 1;
    for (int i = 0; i < nKeyCount; i++)
    {
        KEY kk;
        res = ReadLockEx(addres, &kk, sizeof(unsigned short) * 4, szKey.c_str(), 0, 3);
        if (!res) {
            break;
        }
        else {// compare
            long  ranX = GetRandomNumber();
            ULONG Key1 = ShieldPC(ranX, kk.key1, kk.key2, kk.key3, kk.key4);
            long Key2 = 0;
            bool res = Lock32_Function(ranX, &Key2, 0);
            if (!res || (Key1 != Key2)){
                OutputDebugStringA("authentication failed\n");
                continue;
            }
            // check the remain count
            unsigned long nRemainCnt = 0;
            res = ReadLockEx(KEY_REMAIN_CNT_ADDRES, &nRemainCnt, sizeof(unsigned long), szKey.c_str(), 0, 3);
            if (!res || nRemainCnt <= 0)
                break;


            if (bCheckRemain) {
                nRemainCnt--;
                WriteLockEx(KEY_REMAIN_CNT_ADDRES, &nRemainCnt, sizeof(unsigned long), szKey.c_str(), 0, 3);
            }
            res = true;
            break;
        }
        addres += 4;
    }
    TerminateLock(0);
    return res;
}
