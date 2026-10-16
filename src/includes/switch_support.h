#ifndef HATARI_SWITCH_SUPPORT_H
#define HATARI_SWITCH_SUPPORT_H

#if defined(__SWITCH__)

void HatariSwitch_PreInit(void);
void HatariSwitch_PollControls(void);
void HatariSwitch_Cleanup(void);

#endif /* __SWITCH__ */

#endif /* HATARI_SWITCH_SUPPORT_H */
