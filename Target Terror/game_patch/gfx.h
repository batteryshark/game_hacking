#ifndef TT_EMU_GFX_H
#define TT_EMU_GFX_H

static struct systime{
    unsigned char idk_0;
    unsigned long long idk_4;
};
// Function Definitions
int ScrShake_Start(int a1, int a2, int a3);
int ScrShake_Update(float a1);
void ScrShake_Stop();
unsigned long long rtTime_ReadSysTime(struct systime *a1);
#endif //TT_EMU_GFX_H
