#include "switch_support.h"

#if defined(__SWITCH__)

#include <switch.h>
#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

#include "log.h"
#include "vkbd.h"

#define SWITCH_APPDIR    "sdmc:/switch/hatari"
#define SWITCH_GAMES_DIR SWITCH_APPDIR "/games"
#define SWITCH_HDD_DIR   SWITCH_APPDIR "/hdd"
#define SWITCH_SAVES_DIR SWITCH_APPDIR "/saves"
#define SWITCH_TOS_FILE  SWITCH_APPDIR "/tos.img"
#define SWITCH_POLL_INTERVAL 16000000LL

#define SWITCH_STICK_BUTTONS \
	(HidNpadButton_StickLUp | HidNpadButton_StickLDown | \
	 HidNpadButton_StickLLeft | HidNpadButton_StickLRight)

static PadState sPad;
static bool bPadInitialised;
static bool bPadThreadRunning;
static volatile bool bPadThreadExit;
static Thread sPadThread;

static int sHostMouseX = 640;
static int sHostMouseY = 360;
static float sMouseAccumX = 0.0f;
static float sMouseAccumY = 0.0f;

static bool sMouseLeftHeld = false;
static bool sMouseRightHeld = false;

static int sLastTouchX = -1;
static int sLastTouchY = -1;
static bool sTouchDown = false;

static Uint32 sLastMenuOpenTicks = 0;
static bool sOskShortcutHeld = false;

static const struct
{
	u64 button;
	SDL_Scancode scancode;
} sKeyBindings[] = {
	{ HidNpadButton_AnyUp,    SDL_SCANCODE_UP },
	{ HidNpadButton_AnyDown,  SDL_SCANCODE_DOWN },
	{ HidNpadButton_AnyLeft,  SDL_SCANCODE_LEFT },
	{ HidNpadButton_AnyRight, SDL_SCANCODE_RIGHT },
	{ HidNpadButton_X,        SDL_SCANCODE_SPACE },
	{ HidNpadButton_Y,        SDL_SCANCODE_RETURN },
	{ HidNpadButton_A,        SDL_SCANCODE_RCTRL }
};
#define SWITCH_KEY_COUNT (sizeof(sKeyBindings) / sizeof(sKeyBindings[0]))

static const struct
{
	u64 button;
	SDL_Scancode scancode;
} sLoneKeyBindings[] = {
	{ HidNpadButton_L,        SDL_SCANCODE_ESCAPE },
	{ HidNpadButton_R,        SDL_SCANCODE_TAB }
};
#define SWITCH_LONE_KEY_COUNT (sizeof(sLoneKeyBindings) / sizeof(sLoneKeyBindings[0]))

static bool sKeyPressed[SWITCH_KEY_COUNT];
static bool sLoneKeyPressed[SWITCH_LONE_KEY_COUNT];

static void Switch_PushKey(SDL_Scancode scancode, bool bDown)
{
	SDL_Event event;
	SDL_zero(event);
	event.type = bDown ? SDL_KEYDOWN : SDL_KEYUP;
	event.key.type = event.type;
	event.key.state = bDown ? SDL_PRESSED : SDL_RELEASED;
	event.key.repeat = 0;
	event.key.keysym.scancode = scancode;
	event.key.keysym.sym = SDL_GetKeyFromScancode(scancode);
	event.key.keysym.mod = KMOD_NONE;
	SDL_PushEvent(&event);
}

static void Switch_TriggerMenu(void)
{
	Switch_PushKey(SDL_SCANCODE_F12, true);
	Switch_PushKey(SDL_SCANCODE_F12, false);
}

static void Switch_PushMouseButton(Uint8 button, bool bDown, int x, int y)
{
	SDL_Event event;
	SDL_zero(event);
	event.type = bDown ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
	event.button.type = event.type;
	event.button.state = bDown ? SDL_PRESSED : SDL_RELEASED;
	event.button.button = button;
	event.button.clicks = 1;
	event.button.x = x;
	event.button.y = y;
	SDL_PushEvent(&event);
}

static void Switch_PushMouseMotion(int dx, int dy, int x, int y)
{
	SDL_Event event;
	if (dx == 0 && dy == 0)
		return;

	SDL_zero(event);
	event.type = SDL_MOUSEMOTION;
	event.motion.type = event.type;
	event.motion.xrel = dx;
	event.motion.yrel = dy;
	event.motion.x = x;
	event.motion.y = y;
	SDL_PushEvent(&event);
}

static void HatariSwitch_ProcessTouch(void)
{
	HidTouchScreenState touchState = {0};
	if (hidGetTouchScreenStates(&touchState, 1) > 0 && touchState.count > 0)
	{
		int tx = touchState.touches[0].x;
		int ty = touchState.touches[0].y;

		if (VKBD_IsOpen())
		{
			VKBD_HandleTouch(tx, ty, !sTouchDown);
			sTouchDown = true;
			sLastTouchX = tx;
			sLastTouchY = ty;
			return;
		}

		if (!sTouchDown)
		{
			sTouchDown = true;
			sLastTouchX = tx;
			sLastTouchY = ty;
			sHostMouseX = tx;
			sHostMouseY = ty;
			Switch_PushMouseButton(SDL_BUTTON_LEFT, true, tx, ty);
		}
		else
		{
			int dx = tx - sLastTouchX;
			int dy = ty - sLastTouchY;
			if (dx != 0 || dy != 0)
			{
				sHostMouseX = tx;
				sHostMouseY = ty;
				Switch_PushMouseMotion(dx, dy, tx, ty);
				sLastTouchX = tx;
				sLastTouchY = ty;
			}
		}
	}
	else
	{
		if (sTouchDown)
		{
			if (VKBD_IsOpen())
			{
				VKBD_HandleTouch(sLastTouchX, sLastTouchY, false);
			}
			else
			{
				Switch_PushMouseButton(SDL_BUTTON_LEFT, false, sLastTouchX, sLastTouchY);
			}
			sTouchDown = false;
			sLastTouchX = -1;
			sLastTouchY = -1;
		}
	}
}

static void HatariSwitch_ProcessSticks(HidAnalogStickState stickL, HidAnalogStickState stickR, u64 nHeld)
{
	float fx = 0.0f, fy = 0.0f;
	bool bLeftStickMouse = (nHeld & HidNpadButton_ZL) != 0;

	int rx = stickR.x;
	int ry = -stickR.y;
	if (abs(rx) > 3500)
	{
		float norm = (float)(rx > 0 ? rx - 3500 : rx + 3500) / 29267.0f;
		fx += norm * fabsf(norm) * 14.0f;
	}
	if (abs(ry) > 3500)
	{
		float norm = (float)(ry > 0 ? ry - 3500 : ry + 3500) / 29267.0f;
		fy += norm * fabsf(norm) * 14.0f;
	}

	if (bLeftStickMouse)
	{
		int lx = stickL.x;
		int ly = -stickL.y;
		if (abs(lx) > 3500)
		{
			float norm = (float)(lx > 0 ? lx - 3500 : lx + 3500) / 29267.0f;
			fx += norm * fabsf(norm) * 14.0f;
		}
		if (abs(ly) > 3500)
		{
			float norm = (float)(ly > 0 ? ly - 3500 : ly + 3500) / 29267.0f;
			fy += norm * fabsf(norm) * 14.0f;
		}
	}

	sMouseAccumX += fx;
	sMouseAccumY += fy;
	int mdx = (int)sMouseAccumX;
	int mdy = (int)sMouseAccumY;
	sMouseAccumX -= (float)mdx;
	sMouseAccumY -= (float)mdy;

	if (mdx != 0 || mdy != 0)
	{
		sHostMouseX += mdx;
		sHostMouseY += mdy;
		if (sHostMouseX < 0) sHostMouseX = 0;
		if (sHostMouseX > 1279) sHostMouseX = 1279;
		if (sHostMouseY < 0) sHostMouseY = 0;
		if (sHostMouseY > 719) sHostMouseY = 719;

		Switch_PushMouseMotion(mdx, mdy, sHostMouseX, sHostMouseY);
	}
}

static void HatariSwitch_ProcessMouseButtons(u64 nHeld)
{
	bool left = false;
	bool right = false;

	if (nHeld & HidNpadButton_ZR)
		right = true;

	if (nHeld & HidNpadButton_ZL)
	{
		if (nHeld & HidNpadButton_A)
			left = true;
		if (nHeld & HidNpadButton_B)
			right = true;

		if (!(nHeld & (HidNpadButton_A | HidNpadButton_B |
		               HidNpadButton_StickLUp | HidNpadButton_StickLDown |
		               HidNpadButton_StickLLeft | HidNpadButton_StickLRight)))
		{
			left = true;
		}
	}

	if (left != sMouseLeftHeld)
	{
		Switch_PushMouseButton(SDL_BUTTON_LEFT, left, sHostMouseX, sHostMouseY);
		sMouseLeftHeld = left;
	}
	if (right != sMouseRightHeld)
	{
		Switch_PushMouseButton(SDL_BUTTON_RIGHT, right, sHostMouseX, sHostMouseY);
		sMouseRightHeld = right;
	}
}

static void HatariSwitch_HandlePad(void)
{
	HidAnalogStickState stickL, stickR;
	u64 nHeld, nDown;
	unsigned int i;
	bool bLAndR;

	static u64 sPrevButtons = 0;

	padUpdate(&sPad);
	nHeld = padGetButtons(&sPad);
	nDown = nHeld & ~sPrevButtons;
	sPrevButtons = nHeld;

	stickL = padGetStickPos(&sPad, 0);
	stickR = padGetStickPos(&sPad, 1);

	if (nDown & HidNpadButton_Plus)
	{
		SDL_Event event;
		SDL_zero(event);
		event.type = SDL_QUIT;
		SDL_PushEvent(&event);
	}

	bLAndR = (nHeld & (HidNpadButton_L | HidNpadButton_R)) == (HidNpadButton_L | HidNpadButton_R);
	if ((bLAndR && !sOskShortcutHeld) ||
	    (nDown & (HidNpadButton_Minus | HidNpadButton_StickL | HidNpadButton_StickR)))
	{
		VKBD_Toggle();
		sOskShortcutHeld = bLAndR;
		return;
	}
	sOskShortcutHeld = bLAndR;

	HatariSwitch_ProcessTouch();

	if (VKBD_IsOpen())
	{
		VKBD_HandleInput(nDown, nHeld, stickL.x, stickL.y, stickR.x, stickR.y);
		return;
	}

	HatariSwitch_ProcessSticks(stickL, stickR, nHeld);
	HatariSwitch_ProcessMouseButtons(nHeld);

	if ((nDown & HidNpadButton_B) && !(nHeld & HidNpadButton_ZL))
	{
		Uint32 now = SDL_GetTicks();
		if (now - sLastMenuOpenTicks > 400)
		{
			sLastMenuOpenTicks = now;
			Switch_TriggerMenu();
		}
	}

	for (i = 0; i < SWITCH_KEY_COUNT; i++)
	{
		u64 button = sKeyBindings[i].button;
		bool bPressed;

		if ((nHeld & HidNpadButton_ZL) && (button & SWITCH_STICK_BUTTONS))
			button &= ~SWITCH_STICK_BUTTONS;

		if ((nHeld & HidNpadButton_ZL) && button == HidNpadButton_A)
			button = 0;

		bPressed = button && (nHeld & button);
		if (bPressed != sKeyPressed[i])
		{
			Switch_PushKey(sKeyBindings[i].scancode, bPressed);
			sKeyPressed[i] = bPressed;
		}
	}

	for (i = 0; i < SWITCH_LONE_KEY_COUNT; i++)
	{
		bool bPressed = !bLAndR && (nHeld & sLoneKeyBindings[i].button);
		if (bPressed != sLoneKeyPressed[i])
		{
			Switch_PushKey(sLoneKeyBindings[i].scancode, bPressed);
			sLoneKeyPressed[i] = bPressed;
		}
	}
}

void HatariSwitch_PollControls(void)
{
	if (!bPadThreadRunning && SDL_WasInit(SDL_INIT_EVENTS))
		HatariSwitch_HandlePad();
}

static void HatariSwitch_PadThread(void *arg)
{
	(void)arg;
	while (!bPadThreadExit)
	{
		if (SDL_WasInit(SDL_INIT_EVENTS))
			HatariSwitch_HandlePad();
		svcSleepThread(SWITCH_POLL_INTERVAL);
	}
}

void HatariSwitch_PreInit(void)
{
	u64 nButtons = 0;

	consoleInit(NULL);

	mkdir("sdmc:/switch", 0777);
	mkdir(SWITCH_APPDIR, 0777);
	mkdir(SWITCH_GAMES_DIR, 0777);
	mkdir(SWITCH_HDD_DIR, 0777);
	mkdir(SWITCH_SAVES_DIR, 0777);
	if (chdir(SWITCH_APPDIR) != 0)
		Log_Printf(LOG_WARN, "Switch: cannot change to %s\n", SWITCH_APPDIR);
	setenv("HOME", SWITCH_APPDIR, 1);

	if (R_FAILED(romfsInit()))
		Log_Printf(LOG_WARN, "Switch: RomFS could not be mounted\n");

	if (!bPadInitialised)
	{
		padConfigureInput(1, HidNpadStyleSet_NpadStandard);
		padInitializeDefault(&sPad);
		hidInitializeTouchScreen();
		bPadInitialised = true;
	}

	if (access(SWITCH_TOS_FILE, R_OK) != 0)
	{
		printf("\x1b[2J\x1b[H");
		printf("Hatari for Nintendo Switch\n\n");
		printf("TOS ROM not found:\n  %s\n\n", SWITCH_TOS_FILE);
		printf("Copy your TOS ROM there, or select\n");
		printf("another one from the TOS ROM options (F12).\n\n");
		printf("Folders:\n  %s\n  %s\n  %s\n\n",
		       SWITCH_GAMES_DIR, SWITCH_HDD_DIR, SWITCH_SAVES_DIR);
		printf("A: continue   +: exit\n");
		while (appletMainLoop())
		{
			consoleUpdate(NULL);
			padUpdate(&sPad);
			nButtons = padGetButtonsDown(&sPad);
			if (nButtons & (HidNpadButton_A | HidNpadButton_Plus))
				break;
		}
		if (nButtons & HidNpadButton_Plus)
		{
			romfsExit();
			consoleExit(NULL);
			exit(0);
		}
	}

	consoleExit(NULL);

	if (R_SUCCEEDED(threadCreate(&sPadThread, HatariSwitch_PadThread, NULL,
	                             NULL, 0x10000, 0x3B, -2)))
	{
		if (R_SUCCEEDED(threadStart(&sPadThread)))
			bPadThreadRunning = true;
		else
			threadClose(&sPadThread);
	}

	atexit(HatariSwitch_Cleanup);
}

void HatariSwitch_Cleanup(void)
{
	if (bPadThreadRunning)
	{
		bPadThreadExit = true;
		threadWaitForExit(&sPadThread);
		threadClose(&sPadThread);
		bPadThreadRunning = false;
	}
	romfsExit();
}

#endif
