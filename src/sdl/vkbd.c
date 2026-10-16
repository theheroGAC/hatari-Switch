#include "vkbd.h"

#if defined(__SWITCH__)

#include <switch.h>
#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ikbd.h"
#include "font10x16.h"

enum {
	VKBD_FLAG_NORM = 0,
	VKBD_FLAG_SHIFT = 1,
	VKBD_FLAG_CTRL = 2,
	VKBD_FLAG_ALT = 3,
	VKBD_FLAG_CAPS = 4,
	VKBD_FLAG_CLOSE = 5
};

typedef struct {
	const char *norm;
	const char *shift;
	uint8_t scancode;
	uint8_t flags;
	int w;
} VkbdKey;

static const VkbdKey sRow0[] = {
	{ "ESC", "ESC", 0x01, VKBD_FLAG_NORM, 50 },
	{ "F1",  "F1",  0x3B, VKBD_FLAG_NORM, 55 },
	{ "F2",  "F2",  0x3C, VKBD_FLAG_NORM, 55 },
	{ "F3",  "F3",  0x3D, VKBD_FLAG_NORM, 55 },
	{ "F4",  "F4",  0x3E, VKBD_FLAG_NORM, 55 },
	{ "F5",  "F5",  0x3F, VKBD_FLAG_NORM, 55 },
	{ "F6",  "F6",  0x40, VKBD_FLAG_NORM, 55 },
	{ "F7",  "F7",  0x41, VKBD_FLAG_NORM, 55 },
	{ "F8",  "F8",  0x42, VKBD_FLAG_NORM, 55 },
	{ "F9",  "F9",  0x43, VKBD_FLAG_NORM, 55 },
	{ "F10", "F10", 0x44, VKBD_FLAG_NORM, 55 },
	{ "HELP","HELP",0x62, VKBD_FLAG_NORM, 60 },
	{ "UNDO","UNDO",0x61, VKBD_FLAG_NORM, 60 }
};

static const VkbdKey sRow1[] = {
	{ "`",  "~",  0x29, VKBD_FLAG_NORM, 50 },
	{ "1",  "!",  0x02, VKBD_FLAG_NORM, 50 },
	{ "2",  "@",  0x03, VKBD_FLAG_NORM, 50 },
	{ "3",  "#",  0x04, VKBD_FLAG_NORM, 50 },
	{ "4",  "$",  0x05, VKBD_FLAG_NORM, 50 },
	{ "5",  "%",  0x06, VKBD_FLAG_NORM, 50 },
	{ "6",  "^",  0x07, VKBD_FLAG_NORM, 50 },
	{ "7",  "&",  0x08, VKBD_FLAG_NORM, 50 },
	{ "8",  "*",  0x09, VKBD_FLAG_NORM, 50 },
	{ "9",  "(",  0x0A, VKBD_FLAG_NORM, 50 },
	{ "0",  ")",  0x0B, VKBD_FLAG_NORM, 50 },
	{ "-",  "_",  0x0C, VKBD_FLAG_NORM, 50 },
	{ "=",  "+",  0x0D, VKBD_FLAG_NORM, 50 },
	{ "\\", "|",  0x2B, VKBD_FLAG_NORM, 50 },
	{ "BS", "BS", 0x0E, VKBD_FLAG_NORM, 75 }
};

static const VkbdKey sRow2[] = {
	{ "Tab", "Tab", 0x0F, VKBD_FLAG_NORM, 65 },
	{ "q",   "Q",   0x10, VKBD_FLAG_NORM, 50 },
	{ "w",   "W",   0x11, VKBD_FLAG_NORM, 50 },
	{ "e",   "E",   0x12, VKBD_FLAG_NORM, 50 },
	{ "r",   "R",   0x13, VKBD_FLAG_NORM, 50 },
	{ "t",   "T",   0x14, VKBD_FLAG_NORM, 50 },
	{ "y",   "Y",   0x15, VKBD_FLAG_NORM, 50 },
	{ "u",   "U",   0x16, VKBD_FLAG_NORM, 50 },
	{ "i",   "I",   0x17, VKBD_FLAG_NORM, 50 },
	{ "o",   "O",   0x18, VKBD_FLAG_NORM, 50 },
	{ "p",   "P",   0x19, VKBD_FLAG_NORM, 50 },
	{ "[",   "{",   0x1A, VKBD_FLAG_NORM, 50 },
	{ "]",   "}",   0x1B, VKBD_FLAG_NORM, 50 },
	{ "RET", "RET", 0x1C, VKBD_FLAG_NORM, 110 }
};

static const VkbdKey sRow3[] = {
	{ "Ctrl","Ctrl",0x1D, VKBD_FLAG_CTRL, 75 },
	{ "a",   "A",   0x1E, VKBD_FLAG_NORM, 50 },
	{ "s",   "S",   0x1F, VKBD_FLAG_NORM, 50 },
	{ "d",   "D",   0x20, VKBD_FLAG_NORM, 50 },
	{ "f",   "F",   0x21, VKBD_FLAG_NORM, 50 },
	{ "g",   "G",   0x22, VKBD_FLAG_NORM, 50 },
	{ "h",   "H",   0x23, VKBD_FLAG_NORM, 50 },
	{ "j",   "J",   0x24, VKBD_FLAG_NORM, 50 },
	{ "k",   "K",   0x25, VKBD_FLAG_NORM, 50 },
	{ "l",   "L",   0x26, VKBD_FLAG_NORM, 50 },
	{ ";",   ":",   0x27, VKBD_FLAG_NORM, 50 },
	{ "'",   "\"",  0x28, VKBD_FLAG_NORM, 50 },
	{ "Del", "Del", 0x53, VKBD_FLAG_NORM, 65 },
	{ "Clr", "Home",0x47, VKBD_FLAG_NORM, 60 }
};

static const VkbdKey sRow4[] = {
	{ "Shift","Shift",0x2A, VKBD_FLAG_SHIFT, 90 },
	{ "z",    "Z",    0x2C, VKBD_FLAG_NORM,  50 },
	{ "x",    "X",    0x2D, VKBD_FLAG_NORM,  50 },
	{ "c",    "C",    0x2E, VKBD_FLAG_NORM,  50 },
	{ "v",    "V",    0x2F, VKBD_FLAG_NORM,  50 },
	{ "b",    "B",    0x30, VKBD_FLAG_NORM,  50 },
	{ "n",    "N",    0x31, VKBD_FLAG_NORM,  50 },
	{ "m",    "M",    0x32, VKBD_FLAG_NORM,  50 },
	{ ",",    "<",    0x33, VKBD_FLAG_NORM,  50 },
	{ ".",    ">",    0x34, VKBD_FLAG_NORM,  50 },
	{ "/",    "?",    0x35, VKBD_FLAG_NORM,  50 },
	{ "Shift","Shift",0x36, VKBD_FLAG_SHIFT, 90 },
	{ "^",    "^",    0x48, VKBD_FLAG_NORM,  50 }
};

static const VkbdKey sRow5[] = {
	{ "Alt",  "Alt",  0x38, VKBD_FLAG_ALT,   75 },
	{ "Caps", "Caps", 0x3A, VKBD_FLAG_CAPS,  75 },
	{ "SPACE","SPACE",0x39, VKBD_FLAG_NORM,  360 },
	{ "<",    "<",    0x4B, VKBD_FLAG_NORM,  50 },
	{ "v",    "v",    0x50, VKBD_FLAG_NORM,  50 },
	{ ">",    ">",    0x4D, VKBD_FLAG_NORM,  50 },
	{ "[X]",  "[X]",  0x00, VKBD_FLAG_CLOSE, 65 }
};

static const VkbdKey * const sRows[] = { sRow0, sRow1, sRow2, sRow3, sRow4, sRow5 };
static const int sRowLengths[] = {
	sizeof(sRow0) / sizeof(sRow0[0]),
	sizeof(sRow1) / sizeof(sRow1[0]),
	sizeof(sRow2) / sizeof(sRow2[0]),
	sizeof(sRow3) / sizeof(sRow3[0]),
	sizeof(sRow4) / sizeof(sRow4[0]),
	sizeof(sRow5) / sizeof(sRow5[0])
};

static bool sVkbdOpen = false;
static bool sNeedsRedraw = false;
static int sVkbdX = 210;
static int sVkbdY = 445;
static const int sVkbdW = 860;
static const int sVkbdH = 260;
static int sAlpha = 220;

static int sCurRow = 2;
static int sCurCol = 1;

static bool sShiftActive = false;
static bool sCtrlActive = false;
static bool sAltActive = false;
static bool sCapsActive = false;

static SDL_Texture *sFontTexture = NULL;
static Uint32 sNextRepeatTime = 0;

static void VKBD_PressKey(const VkbdKey *k)
{
	if (!k)
		return;

	switch (k->flags)
	{
	case VKBD_FLAG_SHIFT:
		sShiftActive = !sShiftActive;
		IKBD_PressSTKey(0x2A, sShiftActive);
		sNeedsRedraw = true;
		break;
	case VKBD_FLAG_CTRL:
		sCtrlActive = !sCtrlActive;
		IKBD_PressSTKey(0x1D, sCtrlActive);
		sNeedsRedraw = true;
		break;
	case VKBD_FLAG_ALT:
		sAltActive = !sAltActive;
		IKBD_PressSTKey(0x38, sAltActive);
		sNeedsRedraw = true;
		break;
	case VKBD_FLAG_CAPS:
		sCapsActive = !sCapsActive;
		IKBD_PressSTKey(0x3A, sCapsActive);
		sNeedsRedraw = true;
		break;
	case VKBD_FLAG_CLOSE:
		VKBD_Close();
		break;
	default:
		if (k->scancode != 0)
		{
			IKBD_PressSTKey(k->scancode, true);
			IKBD_PressSTKey(k->scancode, false);
		}
		break;
	}
}

static int VKBD_GetRowStartX(int row)
{
	int total = 0;
	int count = sRowLengths[row];
	int i;
	for (i = 0; i < count; i++)
		total += sRows[row][i].w;
	total += (count - 1) * 4;
	return sVkbdX + (sVkbdW - total) / 2;
}

static int VKBD_GetKeyCenterX(int row, int col)
{
	int x = VKBD_GetRowStartX(row);
	int i;
	for (i = 0; i < col; i++)
		x += sRows[row][i].w + 4;
	return x + sRows[row][col].w / 2;
}

static int VKBD_FindClosestCol(int targetRow, int currentCenterX)
{
	int count = sRowLengths[targetRow];
	int bestCol = 0;
	int bestDiff = 999999;
	int i;
	for (i = 0; i < count; i++)
	{
		int cx = VKBD_GetKeyCenterX(targetRow, i);
		int diff = abs(cx - currentCenterX);
		if (diff < bestDiff)
		{
			bestDiff = diff;
			bestCol = i;
		}
	}
	return bestCol;
}

void VKBD_Init(void)
{
}

void VKBD_Quit(void)
{
	VKBD_Close();
	if (sFontTexture)
	{
		SDL_DestroyTexture(sFontTexture);
		sFontTexture = NULL;
	}
}

static Uint32 sVkbdOpenTime = 0;

void VKBD_Toggle(void)
{
	if (sVkbdOpen)
		VKBD_Close();
	else
	{
		sVkbdOpen = true;
		sVkbdOpenTime = SDL_GetTicks();
		sNeedsRedraw = true;
	}
}

bool VKBD_IsOpen(void)
{
	return sVkbdOpen;
}

bool VKBD_NeedsRedraw(void)
{
	bool needed = sNeedsRedraw;
	sNeedsRedraw = false;
	return needed;
}

void VKBD_Close(void)
{
	if (sShiftActive)
	{
		IKBD_PressSTKey(0x2A, false);
		sShiftActive = false;
	}
	if (sCtrlActive)
	{
		IKBD_PressSTKey(0x1D, false);
		sCtrlActive = false;
	}
	if (sAltActive)
	{
		IKBD_PressSTKey(0x38, false);
		sAltActive = false;
	}
	if (sCapsActive)
	{
		IKBD_PressSTKey(0x3A, false);
		sCapsActive = false;
	}
	sVkbdOpen = false;
	sNeedsRedraw = true;
}

bool VKBD_HandleInput(uint64_t down, uint64_t held, int stickLX, int stickLY, int stickRX, int stickRY)
{
	Uint32 now;
	int navX = 0, navY = 0;

	if (!sVkbdOpen)
		return false;

	if (down & (HidNpadButton_Minus | HidNpadButton_B | HidNpadButton_StickL | HidNpadButton_StickR))
	{
		if (SDL_GetTicks() - sVkbdOpenTime > 250)
		{
			VKBD_Close();
			return true;
		}
	}

	if (down & HidNpadButton_X)
	{
		sShiftActive = !sShiftActive;
		IKBD_PressSTKey(0x2A, sShiftActive);
		sNeedsRedraw = true;
		return true;
	}

	if (down & HidNpadButton_Y)
	{
		IKBD_PressSTKey(0x0E, true);
		IKBD_PressSTKey(0x0E, false);
		return true;
	}

	if (down & HidNpadButton_A)
	{
		VKBD_PressKey(&sRows[sCurRow][sCurCol]);
		return true;
	}

	if (abs(stickRY) > 7000)
	{
		sVkbdY -= (stickRY > 0) ? 6 : -6;
		if (sVkbdY < 20) sVkbdY = 20;
		if (sVkbdY > 450) sVkbdY = 450;
		sNeedsRedraw = true;
	}
	if (abs(stickRX) > 7000)
	{
		sAlpha += (stickRX > 0) ? 3 : -3;
		if (sAlpha < 120) sAlpha = 120;
		if (sAlpha > 255) sAlpha = 255;
		sNeedsRedraw = true;
	}

	if (held & (HidNpadButton_AnyLeft | HidNpadButton_StickLLeft))
		navX = -1;
	else if (held & (HidNpadButton_AnyRight | HidNpadButton_StickLRight))
		navX = 1;

	if (held & (HidNpadButton_AnyUp | HidNpadButton_StickLUp))
		navY = -1;
	else if (held & (HidNpadButton_AnyDown | HidNpadButton_StickLDown))
		navY = 1;

	if (stickLX < -15000) navX = -1;
	else if (stickLX > 15000) navX = 1;
	if (stickLY > 15000) navY = -1;
	else if (stickLY < -15000) navY = 1;

	now = SDL_GetTicks();
	if (navX != 0 || navY != 0)
	{
		if (now >= sNextRepeatTime)
		{
			if (navX < 0)
			{
				sCurCol = (sCurCol > 0) ? sCurCol - 1 : sRowLengths[sCurRow] - 1;
			}
			else if (navX > 0)
			{
				sCurCol = (sCurCol + 1 < sRowLengths[sCurRow]) ? sCurCol + 1 : 0;
			}

			if (navY < 0)
			{
				int cx = VKBD_GetKeyCenterX(sCurRow, sCurCol);
				sCurRow = (sCurRow > 0) ? sCurRow - 1 : 5;
				sCurCol = VKBD_FindClosestCol(sCurRow, cx);
			}
			else if (navY > 0)
			{
				int cx = VKBD_GetKeyCenterX(sCurRow, sCurCol);
				sCurRow = (sCurRow + 1 < 6) ? sCurRow + 1 : 0;
				sCurCol = VKBD_FindClosestCol(sCurRow, cx);
			}

			sNextRepeatTime = now + (sNextRepeatTime == 0 ? 250 : 100);
			sNeedsRedraw = true;
		}
	}
	else
	{
		sNextRepeatTime = 0;
	}

	return true;
}

bool VKBD_HandleTouch(int touchX, int touchY, bool touchDown)
{
	int r, c;

	if (!sVkbdOpen)
		return false;

	if (touchX < sVkbdX || touchX > sVkbdX + sVkbdW ||
	    touchY < sVkbdY || touchY > sVkbdY + sVkbdH)
	{
		if (touchDown)
			VKBD_Close();
		return true;
	}

	for (r = 0; r < 6; r++)
	{
		int kx = VKBD_GetRowStartX(r);
		int ky = sVkbdY + 10 + r * (32 + 4);
		for (c = 0; c < sRowLengths[r]; c++)
		{
			int kw = sRows[r][c].w;
			int kh = 32;
			if (touchX >= kx && touchX <= kx + kw &&
			    touchY >= ky && touchY <= ky + kh)
			{
				sCurRow = r;
				sCurCol = c;
				if (touchDown)
					VKBD_PressKey(&sRows[r][c]);
				sNeedsRedraw = true;
				return true;
			}
			kx += kw + 4;
		}
	}

	return true;
}

static void VKBD_DrawString(SDL_Renderer *renderer, int x, int y, const char *text, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	int i;
	if (!sFontTexture || !text)
		return;

	SDL_SetTextureColorMod(sFontTexture, r, g, b);
	SDL_SetTextureAlphaMod(sFontTexture, a);

	for (i = 0; text[i] != '\0'; i++)
	{
		unsigned char ch = (unsigned char)text[i];
		SDL_Rect src, dst;
		src.x = (ch % 16) * 10;
		src.y = (ch / 16) * 16;
		src.w = 10;
		src.h = 16;
		dst.x = x + i * 10;
		dst.y = y;
		dst.w = 10;
		dst.h = 16;
		SDL_RenderCopy(renderer, sFontTexture, &src, &dst);
	}
}

void VKBD_Render(SDL_Renderer *renderer)
{
	int r, c;
	SDL_Rect bgRect;

	if (!sVkbdOpen || !renderer)
		return;

	if (!sFontTexture)
	{
		int fw = 160;
		int fh = 256;
		Uint32 *pixels = (Uint32 *)malloc(fw * fh * sizeof(Uint32));
		if (pixels)
		{
			int y, x;
			for (y = 0; y < fh; y++)
			{
				const Uint8 *row = &font10x16_bits[y * 20];
				for (x = 0; x < fw; x++)
				{
					int on = (row[x / 8] >> (x % 8)) & 1;
					pixels[y * fw + x] = on ? 0xFFFFFFFF : 0x00000000;
				}
			}

			sFontTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			                                 SDL_TEXTUREACCESS_STATIC, fw, fh);
			if (sFontTexture)
			{
				SDL_UpdateTexture(sFontTexture, NULL, pixels, fw * sizeof(Uint32));
				SDL_SetTextureBlendMode(sFontTexture, SDL_BLENDMODE_BLEND);
			}
			free(pixels);
		}
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	bgRect.x = sVkbdX;
	bgRect.y = sVkbdY;
	bgRect.w = sVkbdW;
	bgRect.h = sVkbdH;
	SDL_SetRenderDrawColor(renderer, 20, 24, 30, sAlpha);
	SDL_RenderFillRect(renderer, &bgRect);
	SDL_SetRenderDrawColor(renderer, 60, 75, 95, sAlpha);
	SDL_RenderDrawRect(renderer, &bgRect);

	for (r = 0; r < 6; r++)
	{
		int kx = VKBD_GetRowStartX(r);
		int ky = sVkbdY + 10 + r * (32 + 4);

		for (c = 0; c < sRowLengths[r]; c++)
		{
			const VkbdKey *k = &sRows[r][c];
			const char *label;
			int textLen, tx, ty;
			SDL_Rect krect;
			bool selected = (r == sCurRow && c == sCurCol);
			bool activeModifier = false;

			if (k->flags == VKBD_FLAG_SHIFT && sShiftActive) activeModifier = true;
			if (k->flags == VKBD_FLAG_CTRL && sCtrlActive) activeModifier = true;
			if (k->flags == VKBD_FLAG_ALT && sAltActive) activeModifier = true;
			if (k->flags == VKBD_FLAG_CAPS && sCapsActive) activeModifier = true;

			krect.x = kx;
			krect.y = ky;
			krect.w = k->w;
			krect.h = 32;

			if (selected)
			{
				SDL_SetRenderDrawColor(renderer, 230, 160, 30, sAlpha);
				SDL_RenderFillRect(renderer, &krect);
				SDL_SetRenderDrawColor(renderer, 255, 230, 120, sAlpha);
				SDL_RenderDrawRect(renderer, &krect);
			}
			else if (activeModifier)
			{
				SDL_SetRenderDrawColor(renderer, 35, 150, 70, sAlpha);
				SDL_RenderFillRect(renderer, &krect);
				SDL_SetRenderDrawColor(renderer, 70, 220, 110, sAlpha);
				SDL_RenderDrawRect(renderer, &krect);
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 42, 50, 62, sAlpha);
				SDL_RenderFillRect(renderer, &krect);
				SDL_SetRenderDrawColor(renderer, 75, 88, 105, sAlpha);
				SDL_RenderDrawRect(renderer, &krect);
			}

			label = (sShiftActive || sCapsActive) ? k->shift : k->norm;
			textLen = (int)strlen(label);
			tx = kx + (k->w - textLen * 10) / 2;
			ty = ky + (32 - 16) / 2;

			if (selected)
				VKBD_DrawString(renderer, tx, ty, label, 15, 15, 15, sAlpha);
			else
				VKBD_DrawString(renderer, tx, ty, label, 240, 240, 240, sAlpha);

			kx += k->w + 4;
		}
	}

	VKBD_DrawString(renderer, sVkbdX + 20, sVkbdY + sVkbdH - 20,
	                "A:Type  X:Shift  Y:Bksp  B/-:Close  R-Stick:Move/Alpha",
	                150, 175, 205, sAlpha);
}

#endif
