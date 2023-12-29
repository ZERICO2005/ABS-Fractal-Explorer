/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef KEYBIND_DATA_H
#define KEYBIND_DATA_H

#include "Common_Def.h"

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

/*
**	This file contains the data used to generate the keyboard graphics.
**	The size of one key is 4x4 since ANSI keyboards have keys offset by 1/4 increments.
*/

struct KeyBox {
	const char* name;
	int32_t posX;
	int32_t posY;
	int32_t sizeX;
	int32_t sizeY;
	SDL_KeyCode Keycode;
	SDL_Scancode Scancode;
	const char* KeycodeName;
	const char* ScancodeName;
}; typedef struct KeyBox KeyBox;

KeyBox Keyboard_NULLKEY = {"Unknown",0,0,0,0,SDLK_UNKNOWN,SDL_SCANCODE_UNKNOWN,"SDLK_UNKNOWN","SDL_SCANCODE_UNKNOWN"};
/* These 3 keycodes are listed in the SDL2 docs but don't appear to exist in SDL2/SDL.h */
	#define SDL_SCANCODE_LOCKINGCAPSLOCK SDL_SCANCODE_UNKNOWN
	#define SDL_SCANCODE_LOCKINGNUMLOCK SDL_SCANCODE_UNKNOWN
	#define SDL_SCANCODE_LOCKINGSCROLLLOCK SDL_SCANCODE_UNKNOWN
#define Number_of_Keyboard_Sizes 3

const KeyBox Keyboard_ANSI[] = {
    {"Esc",0,0,4,4,SDLK_ESCAPE,SDL_SCANCODE_ESCAPE,"SDLK_ESCAPE","SDL_SCANCODE_ESCAPE"},{"F1",8,0,4,4,SDLK_F1,SDL_SCANCODE_F1,"SDLK_F1","SDL_SCANCODE_F1"},{"F2",12,0,4,4,SDLK_F2,SDL_SCANCODE_F2,"SDLK_F2","SDL_SCANCODE_F2"},{"F3",16,0,4,4,SDLK_F3,SDL_SCANCODE_F3,"SDLK_F3","SDL_SCANCODE_F3"},{"F4",20,0,4,4,SDLK_F4,SDL_SCANCODE_F4,"SDLK_F4","SDL_SCANCODE_F4"},{"F5",26,0,4,4,SDLK_F5,SDL_SCANCODE_F5,"SDLK_F5","SDL_SCANCODE_F5"},{"F6",30,0,4,4,SDLK_F6,SDL_SCANCODE_F6,"SDLK_F6","SDL_SCANCODE_F6"},{"F7",34,0,4,4,SDLK_F7,SDL_SCANCODE_F7,"SDLK_F7","SDL_SCANCODE_F7"},{"F8",38,0,4,4,SDLK_F8,SDL_SCANCODE_F8,"SDLK_F8","SDL_SCANCODE_F8"},{"F9",44,0,4,4,SDLK_F9,SDL_SCANCODE_F9,"SDLK_F9","SDL_SCANCODE_F9"},{"F10",48,0,4,4,SDLK_F10,SDL_SCANCODE_F10,"SDLK_F10","SDL_SCANCODE_F10"},{"F11",52,0,4,4,SDLK_F11,SDL_SCANCODE_F11,"SDLK_F11","SDL_SCANCODE_F11"},{"F12",56,0,4,4,SDLK_F12,SDL_SCANCODE_F12,"SDLK_F12","SDL_SCANCODE_F12"},
    {"`\n~",0,5,4,4,SDLK_BACKQUOTE,SDL_SCANCODE_GRAVE,"SDLK_BACKQUOTE","SDL_SCANCODE_GRAVE"},{"1\n!",4,5,4,4,SDLK_1,SDL_SCANCODE_1,"SDLK_1","SDL_SCANCODE_1"},{"2\n@",8,5,4,4,SDLK_2,SDL_SCANCODE_2,"SDLK_2","SDL_SCANCODE_2"},{"3\n#",12,5,4,4,SDLK_3,SDL_SCANCODE_3,"SDLK_3","SDL_SCANCODE_3"},{"4\n$",16,5,4,4,SDLK_4,SDL_SCANCODE_4,"SDLK_4","SDL_SCANCODE_4"},{"5\n%",20,5,4,4,SDLK_5,SDL_SCANCODE_5,"SDLK_5","SDL_SCANCODE_5"},{"6\n^",24,5,4,4,SDLK_6,SDL_SCANCODE_6,"SDLK_6","SDL_SCANCODE_6"},{"7\n&",28,5,4,4,SDLK_7,SDL_SCANCODE_7,"SDLK_7","SDL_SCANCODE_7"},{"8\n*",32,5,4,4,SDLK_8,SDL_SCANCODE_8,"SDLK_8","SDL_SCANCODE_8"},{"9\n(",36,5,4,4,SDLK_9,SDL_SCANCODE_9,"SDLK_9","SDL_SCANCODE_9"},{"0\n)",40,5,4,4,SDLK_0,SDL_SCANCODE_0,"SDLK_0","SDL_SCANCODE_0"},{"-\n_",44,5,4,4,SDLK_MINUS,SDL_SCANCODE_MINUS,"SDLK_MINUS","SDL_SCANCODE_MINUS"},{"=\n+",48,5,4,4,SDLK_EQUALS,SDL_SCANCODE_EQUALS,"SDLK_EQUALS","SDL_SCANCODE_EQUALS"},{"Backspace",52,5,8,4,SDLK_BACKSPACE,SDL_SCANCODE_BACKSPACE,"SDLK_BACKSPACE","SDL_SCANCODE_BACKSPACE"},
    {"Tab |<-\n    ->|",0,9,6,4,SDLK_TAB,SDL_SCANCODE_TAB,"SDLK_TAB","SDL_SCANCODE_TAB"},{"Q",6,9,4,4,SDLK_q,SDL_SCANCODE_Q,"SDLK_q","SDL_SCANCODE_Q"},{"W",10,9,4,4,SDLK_w,SDL_SCANCODE_W,"SDLK_w","SDL_SCANCODE_W"},{"E",14,9,4,4,SDLK_e,SDL_SCANCODE_E,"SDLK_e","SDL_SCANCODE_E"},{"R",18,9,4,4,SDLK_r,SDL_SCANCODE_R,"SDLK_r","SDL_SCANCODE_R"},{"T",22,9,4,4,SDLK_t,SDL_SCANCODE_T,"SDLK_t","SDL_SCANCODE_T"},{"Y",26,9,4,4,SDLK_y,SDL_SCANCODE_Y,"SDLK_y","SDL_SCANCODE_Y"},{"U",30,9,4,4,SDLK_u,SDL_SCANCODE_U,"SDLK_u","SDL_SCANCODE_U"},{"I",34,9,4,4,SDLK_i,SDL_SCANCODE_I,"SDLK_i","SDL_SCANCODE_I"},{"O",38,9,4,4,SDLK_o,SDL_SCANCODE_O,"SDLK_o","SDL_SCANCODE_O"},{"P",42,9,4,4,SDLK_p,SDL_SCANCODE_P,"SDLK_p","SDL_SCANCODE_P"},{"[\n{",46,9,4,4,SDLK_LEFTBRACKET,SDL_SCANCODE_LEFTBRACKET,"SDLK_LEFTBRACKET","SDL_SCANCODE_LEFTBRACKET"},{"]\n}",50,9,4,4,SDLK_RIGHTBRACKET,SDL_SCANCODE_RIGHTBRACKET,"SDLK_RIGHTBRACKET","SDL_SCANCODE_RIGHTBRACKET"},{"\\\n|",54,9,6,4,SDLK_BACKSLASH,SDL_SCANCODE_BACKSLASH,"SDLK_BACKSLASH","SDL_SCANCODE_BACKSLASH"},
    {"Caps\nLock",0,13,7,4,SDLK_CAPSLOCK,SDL_SCANCODE_CAPSLOCK,"SDLK_CAPSLOCK","SDL_SCANCODE_CAPSLOCK"},{"A",7,13,4,4,SDLK_a,SDL_SCANCODE_A,"SDLK_a","SDL_SCANCODE_A"},{"S",11,13,4,4,SDLK_s,SDL_SCANCODE_S,"SDLK_s","SDL_SCANCODE_S"},{"D",15,13,4,4,SDLK_d,SDL_SCANCODE_D,"SDLK_d","SDL_SCANCODE_D"},{"F",19,13,4,4,SDLK_f,SDL_SCANCODE_F,"SDLK_f","SDL_SCANCODE_F"},{"G",23,13,4,4,SDLK_g,SDL_SCANCODE_G,"SDLK_g","SDL_SCANCODE_G"},{"H",27,13,4,4,SDLK_h,SDL_SCANCODE_H,"SDLK_h","SDL_SCANCODE_H"},{"J",31,13,4,4,SDLK_j,SDL_SCANCODE_J,"SDLK_j","SDL_SCANCODE_J"},{"K",35,13,4,4,SDLK_k,SDL_SCANCODE_K,"SDLK_k","SDL_SCANCODE_K"},{"L",39,13,4,4,SDLK_l,SDL_SCANCODE_L,"SDLK_l","SDL_SCANCODE_L"},{";\n:",43,13,4,4,SDLK_SEMICOLON,SDL_SCANCODE_SEMICOLON,"SDLK_SEMICOLON","SDL_SCANCODE_SEMICOLON"},{"\'\n\"",47,13,4,4,SDLK_QUOTE,SDL_SCANCODE_APOSTROPHE,"SDLK_QUOTE","SDL_SCANCODE_APOSTROPHE"},{"Enter",51,13,9,4,SDLK_RETURN,SDL_SCANCODE_RETURN,"SDLK_RETURN","SDL_SCANCODE_RETURN"},
    {"Shift",0,17,9,4,SDLK_LSHIFT,SDL_SCANCODE_LSHIFT,"SDLK_LSHIFT","SDL_SCANCODE_LSHIFT"},{"Z",9,17,4,4,SDLK_z,SDL_SCANCODE_Z,"SDLK_z","SDL_SCANCODE_Z"},{"X",13,17,4,4,SDLK_x,SDL_SCANCODE_X,"SDLK_x","SDL_SCANCODE_X"},{"C",17,17,4,4,SDLK_c,SDL_SCANCODE_C,"SDLK_c","SDL_SCANCODE_C"},{"V",21,17,4,4,SDLK_v,SDL_SCANCODE_V,"SDLK_v","SDL_SCANCODE_V"},{"B",25,17,4,4,SDLK_b,SDL_SCANCODE_B,"SDLK_b","SDL_SCANCODE_B"},{"N",29,17,4,4,SDLK_n,SDL_SCANCODE_N,"SDLK_n","SDL_SCANCODE_N"},{"M",33,17,4,4,SDLK_m,SDL_SCANCODE_M,"SDLK_m","SDL_SCANCODE_M"},{",\n<",37,17,4,4,SDLK_COMMA,SDL_SCANCODE_COMMA,"SDLK_COMMA","SDL_SCANCODE_COMMA"},{".\n>",41,17,4,4,SDLK_PERIOD,SDL_SCANCODE_PERIOD,"SDLK_PERIOD","SDL_SCANCODE_PERIOD"},{"/\n?",45,17,4,4,SDLK_SLASH,SDL_SCANCODE_SLASH,"SDLK_SLASH","SDL_SCANCODE_SLASH"},{"Shift",49,17,11,4,SDLK_RSHIFT,SDL_SCANCODE_RSHIFT,"SDLK_RSHIFT","SDL_SCANCODE_RSHIFT"},
    {"Ctrl",0,21,6,4,SDLK_LCTRL,SDL_SCANCODE_LCTRL,"SDLK_LCTRL","SDL_SCANCODE_LCTRL"},{"Meta",6,21,5,4,SDLK_LGUI,SDL_SCANCODE_LGUI,"SDLK_LGUI","SDL_SCANCODE_LGUI"},{"Alt",11,21,5,4,SDLK_LALT,SDL_SCANCODE_LALT,"SDLK_LALT","SDL_SCANCODE_LALT"},{"Space Bar",16,21,23,4,SDLK_SPACE,SDL_SCANCODE_SPACE,"SDLK_SPACE","SDL_SCANCODE_SPACE"},{"Alt Gr",39,21,5,4,SDLK_RALT,SDL_SCANCODE_RALT,"SDLK_RALT","SDL_SCANCODE_RALT"},{"Meta",44,21,5,4,SDLK_RGUI,SDL_SCANCODE_RGUI,"SDLK_RGUI","SDL_SCANCODE_RGUI"},{"Context",49,21,5,4,SDLK_APPLICATION,SDL_SCANCODE_APPLICATION,"SDLK_APPLICATION","SDL_SCANCODE_APPLICATION"},{"Ctrl",54,21,6,4,SDLK_RCTRL,SDL_SCANCODE_RCTRL,"SDLK_RCTRL","SDL_SCANCODE_RCTRL"},
    {"Prt\nScr",61,0,4,4,SDLK_PRINTSCREEN,SDL_SCANCODE_PRINTSCREEN,"SDLK_PRINTSCREEN","SDL_SCANCODE_PRINTSCREEN"},{"Scr\nLock",65,0,4,4,SDLK_SCROLLLOCK,SDL_SCANCODE_SCROLLLOCK,"SDLK_SCROLLLOCK","SDL_SCANCODE_SCROLLLOCK"},{"Pause\nBreak",69,0,4,4,SDLK_PAUSE,SDL_SCANCODE_PAUSE,"SDLK_PAUSE","SDL_SCANCODE_PAUSE"},
    {"Ins",61,5,4,4,SDLK_INSERT,SDL_SCANCODE_INSERT,"SDLK_INSERT","SDL_SCANCODE_INSERT"},{"Home",65,5,4,4,SDLK_HOME,SDL_SCANCODE_HOME,"SDLK_HOME","SDL_SCANCODE_HOME"},{"Page\nUp",69,5,4,4,SDLK_PAGEUP,SDL_SCANCODE_PAGEUP,"SDLK_PAGEUP","SDL_SCANCODE_PAGEUP"},
    {"Del",61,9,4,4,SDLK_DELETE,SDL_SCANCODE_DELETE,"SDLK_DELETE","SDL_SCANCODE_DELETE"},{"End",65,9,4,4,SDLK_END,SDL_SCANCODE_END,"SDLK_END","SDL_SCANCODE_END"},{"Page\nDown",69,9,4,4,SDLK_PAGEDOWN,SDL_SCANCODE_PAGEDOWN,"SDLK_PAGEDOWN","SDL_SCANCODE_PAGEDOWN"},
    {"^\n|",65,17,4,4,SDLK_UP,SDL_SCANCODE_UP,"SDLK_UP","SDL_SCANCODE_UP"},{"<-",61,21,4,4,SDLK_LEFT,SDL_SCANCODE_LEFT,"SDLK_LEFT","SDL_SCANCODE_LEFT"},{"|\nV",65,21,4,4,SDLK_DOWN,SDL_SCANCODE_DOWN,"SDLK_DOWN","SDL_SCANCODE_DOWN"},{"->",69,21,4,4,SDLK_RIGHT,SDL_SCANCODE_RIGHT,"SDLK_RIGHT","SDL_SCANCODE_RIGHT"},
};

const KeyBox Keyboard_ANSI_Numpad[] = {
    {"Num\nLock",74,5,4,4,SDLK_NUMLOCKCLEAR,SDL_SCANCODE_NUMLOCKCLEAR,"SDLK_NUMLOCKCLEAR","SDL_SCANCODE_NUMLOCKCLEAR"},{"/",78,5,4,4,SDLK_KP_DIVIDE,SDL_SCANCODE_KP_DIVIDE,"SDLK_KP_DIVIDE","SDL_SCANCODE_KP_DIVIDE"},{"*",82,5,4,4,SDLK_KP_MULTIPLY,SDL_SCANCODE_KP_MULTIPLY,"SDLK_KP_MULTIPLY","SDL_SCANCODE_KP_MULTIPLY"},{"-",86,5,4,4,SDLK_KP_MINUS,SDL_SCANCODE_KP_MINUS,"SDLK_KP_MINUS","SDL_SCANCODE_KP_MINUS"},
    {"7\nHome",74,9,4,4,SDLK_KP_7,SDL_SCANCODE_KP_7,"SDLK_KP_7","SDL_SCANCODE_KP_7"},{"8^\n |",78,9,4,4,SDLK_KP_8,SDL_SCANCODE_KP_8,"SDLK_KP_8","SDL_SCANCODE_KP_8"},{"9\nPg Up",82,9,4,4,SDLK_KP_9,SDL_SCANCODE_KP_9,"SDLK_KP_9","SDL_SCANCODE_KP_9"},{"+",86,9,4,8,SDLK_KP_PLUS,SDL_SCANCODE_KP_PLUS,"SDLK_KP_PLUS","SDL_SCANCODE_KP_PLUS"},
    {"<-4",74,13,4,4,SDLK_KP_4,SDL_SCANCODE_KP_4,"SDLK_KP_4","SDL_SCANCODE_KP_4"},{"5",78,13,4,4,SDLK_KP_5,SDL_SCANCODE_KP_5,"SDLK_KP_5","SDL_SCANCODE_KP_5"},{"6->",82,13,4,4,SDLK_KP_6,SDL_SCANCODE_KP_6,"SDLK_KP_6","SDL_SCANCODE_KP_6"},
    {"1\nEnd",74,17,4,4,SDLK_KP_1,SDL_SCANCODE_KP_1,"SDLK_KP_1","SDL_SCANCODE_KP_1"},{"2|\n V",78,17,4,4,SDLK_KP_2,SDL_SCANCODE_KP_2,"SDLK_KP_2","SDL_SCANCODE_KP_2"},{"3\nPg Dn",82,17,4,4,SDLK_KP_3,SDL_SCANCODE_KP_3,"SDLK_KP_3","SDL_SCANCODE_KP_3"},{"Enter",86,17,4,8,SDLK_KP_ENTER,SDL_SCANCODE_KP_ENTER,"SDLK_KP_ENTER","SDL_SCANCODE_KP_ENTER"},
    {"0\nIns",74,21,8,4,SDLK_KP_0,SDL_SCANCODE_KP_0,"SDLK_KP_0","SDL_SCANCODE_KP_0"},{".\nDel",82,21,4,4,SDLK_KP_PERIOD,SDL_SCANCODE_KP_PERIOD,"SDLK_KP_PERIOD","SDL_SCANCODE_KP_PERIOD"},
};

const KeyBox Keyboard_Extended[] = {
    {"Eject",0,-5,4,4,SDLK_EJECT,SDL_SCANCODE_EJECT,"SDLK_EJECT","SDL_SCANCODE_EJECT"},
	{"AC\nHome",8,-5,4,4,SDLK_AC_HOME,SDL_SCANCODE_AC_HOME,"SDLK_AC_HOME","SDL_SCANCODE_AC_HOME"},{"AC\nRfrsh",12,-5,4,4,SDLK_AC_REFRESH,SDL_SCANCODE_AC_REFRESH,"SDLK_AC_REFRESH","SDL_SCANCODE_AC_REFRESH"},{"AC\nStop",16,-5,4,4,SDLK_AC_STOP,SDL_SCANCODE_AC_STOP,"SDLK_AC_STOP","SDL_SCANCODE_AC_STOP"},{"AC\nBkMrk",20,-5,4,4,SDLK_AC_BOOKMARKS,SDL_SCANCODE_AC_BOOKMARKS,"SDLK_AC_BOOKMARKS","SDL_SCANCODE_AC_BOOKMARKS"},
	{"Vol\nMute",26,-5,4,4,SDLK_AUDIOMUTE,SDL_SCANCODE_AUDIOMUTE,"SDLK_AUDIOMUTE","SDL_SCANCODE_AUDIOMUTE"},{"Vol\nDown",30,-5,4,4,SDLK_VOLUMEDOWN,SDL_SCANCODE_VOLUMEDOWN,"SDLK_VOLUMEDOWN","SDL_SCANCODE_VOLUMEDOWN"},{"Vol\nUp",34,-5,4,4,SDLK_VOLUMEUP,SDL_SCANCODE_VOLUMEUP,"SDLK_VOLUMEUP","SDL_SCANCODE_VOLUMEUP"},{"Mic\nMute",38,-5,4,4,SDLK_MUTE,SDL_SCANCODE_MUTE,"SDLK_MUTE","SDL_SCANCODE_MUTE"},{"Return",44,-5,4,4,SDLK_RETURN2,SDL_SCANCODE_RETURN2,"SDLK_RETURN2","SDL_SCANCODE_RETURN2"},{"Brtns\nDown",48,-5,4,4,SDLK_BRIGHTNESSDOWN,SDL_SCANCODE_BRIGHTNESSDOWN,"SDLK_BRIGHTNESSDOWN","SDL_SCANCODE_BRIGHTNESSDOWN"},{"Non\nUS \\",52,-5,4,4,SDLK_UNKNOWN,SDL_SCANCODE_NONUSBACKSLASH,"none","SDL_SCANCODE_NONUSBACKSLASH"},{"Help",56,-5,4,4,SDLK_HELP,SDL_SCANCODE_HELP,"SDLK_HELP","SDL_SCANCODE_HELP"},
    {"Menu",0,-9,4,4,SDLK_MENU,SDL_SCANCODE_MENU,"SDLK_MENU","SDL_SCANCODE_MENU"},{"AC\nBack",8,-9,4,4,SDLK_AC_BACK,SDL_SCANCODE_AC_BACK,"SDLK_AC_BACK","SDL_SCANCODE_AC_BACK"},{"AC\nSearch",12,-9,4,4,SDLK_AC_SEARCH,SDL_SCANCODE_AC_SEARCH,"SDLK_AC_SEARCH","SDL_SCANCODE_AC_SEARCH"},{"AC\nFrwd",16,-9,4,4,SDLK_AC_FORWARD,SDL_SCANCODE_AC_FORWARD,"SDLK_AC_FORWARD","SDL_SCANCODE_AC_FORWARD"},{"Media\nSelect",20,-9,4,4,SDLK_MEDIASELECT,SDL_SCANCODE_MEDIASELECT,"SDLK_MEDIASELECT","SDL_SCANCODE_MEDIASELECT"},
	{"Prev\n|<<",26,-9,4,4,SDLK_AUDIOPREV,SDL_SCANCODE_AUDIOPREV,"SDLK_AUDIOPREV","SDL_SCANCODE_AUDIOPREV"},{"Play\n>||",30,-9,4,4,SDLK_AUDIOPLAY,SDL_SCANCODE_AUDIOPLAY,"SDLK_AUDIOPLAY","SDL_SCANCODE_AUDIOPLAY"},{"Next\n>>|",34,-9,4,4,SDLK_AUDIONEXT,SDL_SCANCODE_AUDIONEXT,"SDLK_AUDIONEXT","SDL_SCANCODE_AUDIONEXT"},{"Stop\n[]",38,-9,4,4,SDLK_AUDIOSTOP,SDL_SCANCODE_AUDIOSTOP,"SDLK_AUDIOSTOP","SDL_SCANCODE_AUDIOSTOP"},{"Mail",44,-9,4,4,SDLK_MAIL,SDL_SCANCODE_MAIL,"SDLK_MAIL","SDL_SCANCODE_MAIL"},{"Brtns\nUp",48,-9,4,4,SDLK_BRIGHTNESSUP,SDL_SCANCODE_BRIGHTNESSUP,"SDLK_BRIGHTNESSUP","SDL_SCANCODE_BRIGHTNESSUP"},{"Non\nUS #",52,-9,4,4,SDLK_UNKNOWN,SDL_SCANCODE_NONUSHASH,"none","SDL_SCANCODE_NONUSHASH"},{"WWW",56,-9,4,4,SDLK_WWW,SDL_SCANCODE_WWW,"SDLK_WWW","SDL_SCANCODE_WWW"},
    {"Sys\nReq",61,-5,4,4,SDLK_SYSREQ,SDL_SCANCODE_SYSREQ,"SDLK_SYSREQ","SDL_SCANCODE_SYSREQ"},{"Sleep",65,-5,4,4,SDLK_SLEEP,SDL_SCANCODE_SLEEP,"SDLK_SLEEP","SDL_SCANCODE_SLEEP"},{"Power",69,-5,4,4,SDLK_POWER,SDL_SCANCODE_POWER,"SDLK_POWER","SDL_SCANCODE_POWER"},
};

const KeyBox Keyboard_Extended_Numpad[] = {
    {"Curr\nUnit",74,-9,4,4,SDLK_CURRENCYUNIT,SDL_SCANCODE_CURRENCYUNIT,"SDLK_CURRENCYUNIT","SDL_SCANCODE_CURRENCYUNIT"},{"Curr\nSub",78,-9,4,4,SDLK_CURRENCYSUBUNIT,SDL_SCANCODE_CURRENCYSUBUNIT,"SDLK_CURRENCYSUBUNIT","SDL_SCANCODE_CURRENCYSUBUNIT"},{"Separ",82,-9,4,4,SDLK_SEPARATOR,SDL_SCANCODE_SEPARATOR,"SDLK_SEPARATOR","SDL_SCANCODE_SEPARATOR"},{"Dec\nSepar",86,-9,4,4,SDLK_DECIMALSEPARATOR,SDL_SCANCODE_DECIMALSEPARATOR,"SDLK_DECIMALSEPARATOR","SDL_SCANCODE_DECIMALSEPARATOR"},{"Thous\nSepar",90,-9,4,4,SDLK_THOUSANDSSEPARATOR,SDL_SCANCODE_THOUSANDSSEPARATOR,"SDLK_THOUSANDSSEPARATOR","SDL_SCANCODE_THOUSANDSSEPARATOR"},
    {"MR",74,-3,4,4,SDLK_KP_MEMRECALL,SDL_SCANCODE_KP_MEMRECALL,"SDLK_KP_MEMRECALL","SDL_SCANCODE_KP_MEMRECALL"},{"MS",78,-3,4,4,SDLK_KP_MEMSTORE,SDL_SCANCODE_KP_MEMSTORE,"SDLK_KP_MEMSTORE","SDL_SCANCODE_KP_MEMSTORE"},{"MC",82,-3,4,4,SDLK_KP_MEMCLEAR,SDL_SCANCODE_KP_MEMCLEAR,"SDLK_KP_MEMCLEAR","SDL_SCANCODE_KP_MEMCLEAR"},{"Backspace",86,-3,8,4,SDLK_KP_BACKSPACE,SDL_SCANCODE_KP_BACKSPACE,"SDLK_KP_BACKSPACE","SDL_SCANCODE_KP_BACKSPACE"},
    {"M+",74,1,4,4,SDLK_KP_MEMADD,SDL_SCANCODE_KP_MEMADD,"SDLK_KP_MEMADD","SDL_SCANCODE_KP_MEMADD"},{"M-",78,1,4,4,SDLK_KP_MEMSUBTRACT,SDL_SCANCODE_KP_MEMSUBTRACT,"SDLK_KP_MEMSUBTRACT","SDL_SCANCODE_KP_MEMSUBTRACT"},{"/",82,1,4,4,SDLK_KP_DIVIDE,SDL_SCANCODE_KP_DIVIDE,"SDLK_KP_DIVIDE","SDL_SCANCODE_KP_DIVIDE"},{"*",86,1,4,4,SDLK_KP_MULTIPLY,SDL_SCANCODE_KP_MULTIPLY,"SDLK_KP_MULTIPLY","SDL_SCANCODE_KP_MULTIPLY"},{"Clear",90,1,4,4,SDLK_KP_CLEAR,SDL_SCANCODE_KP_CLEAR,"SDLK_KP_CLEAR","SDL_SCANCODE_KP_CLEAR"},
    {"7\nHome",74,5,4,4,SDLK_KP_7,SDL_SCANCODE_KP_7,"SDLK_KP_7","SDL_SCANCODE_KP_7"},{"8^\n |",78,5,4,4,SDLK_KP_8,SDL_SCANCODE_KP_8,"SDLK_KP_8","SDL_SCANCODE_KP_8"},{"9\nPg Up",82,5,4,4,SDLK_KP_9,SDL_SCANCODE_KP_9,"SDLK_KP_9","SDL_SCANCODE_KP_9"},{"-",86,5,4,4,SDLK_KP_MINUS,SDL_SCANCODE_KP_MINUS,"SDLK_KP_MINUS","SDL_SCANCODE_KP_MINUS"},{"Clear\nEntry",90,5,4,4,SDLK_KP_CLEARENTRY,SDL_SCANCODE_KP_CLEARENTRY,"SDLK_KP_CLEARENTRY","SDL_SCANCODE_KP_CLEARENTRY"},
    {"<-4",74,9,4,4,SDLK_KP_4,SDL_SCANCODE_KP_4,"SDLK_KP_4","SDL_SCANCODE_KP_4"},{"5",78,9,4,4,SDLK_KP_5,SDL_SCANCODE_KP_5,"SDLK_KP_5","SDL_SCANCODE_KP_5"},{"6->",82,9,4,4,SDLK_KP_6,SDL_SCANCODE_KP_6,"SDLK_KP_6","SDL_SCANCODE_KP_6"},{"+",86,9,4,8,SDLK_KP_PLUS,SDL_SCANCODE_KP_PLUS,"SDLK_KP_PLUS","SDL_SCANCODE_KP_PLUS"},{"+/-",90,9,4,4,SDLK_KP_PLUSMINUS,SDL_SCANCODE_KP_PLUSMINUS,"SDLK_KP_PLUSMINUS","SDL_SCANCODE_KP_PLUSMINUS"},
    {"1\nEnd",74,13,4,4,SDLK_KP_1,SDL_SCANCODE_KP_1,"SDLK_KP_1","SDL_SCANCODE_KP_1"},{"2|\n V",78,13,4,4,SDLK_KP_2,SDL_SCANCODE_KP_2,"SDLK_KP_2","SDL_SCANCODE_KP_2"},{"3\nPg Dn",82,13,4,4,SDLK_KP_3,SDL_SCANCODE_KP_3,"SDLK_KP_3","SDL_SCANCODE_KP_3"},{"%",90,13,4,4,SDLK_KP_PERCENT,SDL_SCANCODE_KP_PERCENT,"SDLK_KP_PERCENT","SDL_SCANCODE_KP_PERCENT"},
    {"0\nIns",74,17,8,4,SDLK_KP_0,SDL_SCANCODE_KP_0,"SDLK_KP_0","SDL_SCANCODE_KP_0"},{".\nDel",82,17,4,4,SDLK_KP_PERIOD,SDL_SCANCODE_KP_PERIOD,"SDLK_KP_PERIOD","SDL_SCANCODE_KP_PERIOD"},{"Enter",86,17,4,8,SDLK_KP_ENTER,SDL_SCANCODE_KP_ENTER,"SDLK_KP_ENTER","SDL_SCANCODE_KP_ENTER"},{"^",90,17,4,4,SDLK_KP_POWER,SDL_SCANCODE_KP_POWER,"SDLK_KP_POWER","SDL_SCANCODE_KP_POWER"},
    {"00",74,21,4,4,SDLK_KP_00,SDL_SCANCODE_KP_00,"SDLK_KP_00","SDL_SCANCODE_KP_00"},{"000",78,21,4,4,SDLK_KP_000,SDL_SCANCODE_KP_000,"SDLK_KP_000","SDL_SCANCODE_KP_000"},{"=",82,21,4,4,SDLK_KP_EQUALS,SDL_SCANCODE_KP_EQUALS,"SDLK_KP_EQUALS","SDL_SCANCODE_KP_EQUALS"},{"Num\nLock",90,21,4,4,SDLK_NUMLOCKCLEAR,SDL_SCANCODE_NUMLOCKCLEAR,"SDLK_NUMLOCKCLEAR","SDL_SCANCODE_NUMLOCKCLEAR"},
};

const KeyBox Keyboard_Complete[] = {
    {"Eject",0,-5,4,4,SDLK_EJECT,SDL_SCANCODE_EJECT,"SDLK_EJECT","SDL_SCANCODE_EJECT"},{"F13",8,-5,4,4,SDLK_F13,SDL_SCANCODE_F13,"SDLK_F13","SDL_SCANCODE_F13"},{"F14",12,-5,4,4,SDLK_F14,SDL_SCANCODE_F14,"SDLK_F14","SDL_SCANCODE_F14"},{"F15",16,-5,4,4,SDLK_F15,SDL_SCANCODE_F15,"SDLK_F15","SDL_SCANCODE_F15"},{"F16",20,-5,4,4,SDLK_F16,SDL_SCANCODE_F16,"SDLK_F16","SDL_SCANCODE_F16"},{"F17",26,-5,4,4,SDLK_F17,SDL_SCANCODE_F17,"SDLK_F17","SDL_SCANCODE_F17"},{"F18",30,-5,4,4,SDLK_F18,SDL_SCANCODE_F18,"SDLK_F18","SDL_SCANCODE_F18"},{"F19",34,-5,4,4,SDLK_F19,SDL_SCANCODE_F19,"SDLK_F19","SDL_SCANCODE_F19"},{"F20",38,-5,4,4,SDLK_F20,SDL_SCANCODE_F20,"SDLK_F20","SDL_SCANCODE_F20"},{"F21",44,-5,4,4,SDLK_F21,SDL_SCANCODE_F21,"SDLK_F21","SDL_SCANCODE_F21"},{"F22",48,-5,4,4,SDLK_F22,SDL_SCANCODE_F22,"SDLK_F22","SDL_SCANCODE_F22"},{"F23",52,-5,4,4,SDLK_F23,SDL_SCANCODE_F23,"SDLK_F23","SDL_SCANCODE_F23"},{"F24",56,-5,4,4,SDLK_F24,SDL_SCANCODE_F24,"SDLK_F24","SDL_SCANCODE_F24"},
    {"Sys\nReq",61,-5,4,4,SDLK_SYSREQ,SDL_SCANCODE_SYSREQ,"SDLK_SYSREQ","SDL_SCANCODE_SYSREQ"},{"Sleep",65,-5,4,4,SDLK_SLEEP,SDL_SCANCODE_SLEEP,"SDLK_SLEEP","SDL_SCANCODE_SLEEP"},{"Power",69,-5,4,4,SDLK_POWER,SDL_SCANCODE_POWER,"SDLK_POWER","SDL_SCANCODE_POWER"},
    {"Clear",0,-10,4,4,SDLK_CLEAR,SDL_SCANCODE_CLEAR,"SDLK_CLEAR","SDL_SCANCODE_CLEAR"},{"AC\nHome",8,-10,4,4,SDLK_AC_HOME,SDL_SCANCODE_AC_HOME,"SDLK_AC_HOME","SDL_SCANCODE_AC_HOME"},{"AC\nRfrsh",12,-10,4,4,SDLK_AC_REFRESH,SDL_SCANCODE_AC_REFRESH,"SDLK_AC_REFRESH","SDL_SCANCODE_AC_REFRESH"},{"AC\nStop",16,-10,4,4,SDLK_AC_STOP,SDL_SCANCODE_AC_STOP,"SDLK_AC_STOP","SDL_SCANCODE_AC_STOP"},{"AC\nBkMrk",20,-10,4,4,SDLK_AC_BOOKMARKS,SDL_SCANCODE_AC_BOOKMARKS,"SDLK_AC_BOOKMARKS","SDL_SCANCODE_AC_BOOKMARKS"},{"Vol\nMute",26,-10,4,4,SDLK_AUDIOMUTE,SDL_SCANCODE_AUDIOMUTE,"SDLK_AUDIOMUTE","SDL_SCANCODE_AUDIOMUTE"},{"Vol\nDown",30,-10,4,4,SDLK_VOLUMEDOWN,SDL_SCANCODE_VOLUMEDOWN,"SDLK_VOLUMEDOWN","SDL_SCANCODE_VOLUMEDOWN"},{"Vol\nUp",34,-10,4,4,SDLK_VOLUMEUP,SDL_SCANCODE_VOLUMEUP,"SDLK_VOLUMEUP","SDL_SCANCODE_VOLUMEUP"},{"Mic\nMute",38,-10,4,4,SDLK_MUTE,SDL_SCANCODE_MUTE,"SDLK_MUTE","SDL_SCANCODE_MUTE"},{"KB Light\nDown",44,-10,6,4,SDLK_KBDILLUMDOWN,SDL_SCANCODE_KBDILLUMDOWN,"SDLK_KBDILLUMDOWN","SDL_SCANCODE_KBDILLUMDOWN"},{"KB Light\nUp",50,-10,6,4,SDLK_KBDILLUMUP,SDL_SCANCODE_KBDILLUMUP,"SDLK_KBDILLUMUP","SDL_SCANCODE_KBDILLUMUP"},{"KB Lt\nTog",56,-10,4,4,SDLK_KBDILLUMTOGGLE,SDL_SCANCODE_KBDILLUMTOGGLE,"SDLK_KBDILLUMTOGGLE","SDL_SCANCODE_KBDILLUMTOGGLE"},
    {"Clear\nAgain",0,-14,4,4,SDLK_CLEARAGAIN,SDL_SCANCODE_CLEARAGAIN,"SDLK_CLEARAGAIN","SDL_SCANCODE_CLEARAGAIN"},{"AC\nBack",8,-14,4,4,SDLK_AC_BACK,SDL_SCANCODE_AC_BACK,"SDLK_AC_BACK","SDL_SCANCODE_AC_BACK"},{"AC\nSearch",12,-14,4,4,SDLK_AC_SEARCH,SDL_SCANCODE_AC_SEARCH,"SDLK_AC_SEARCH","SDL_SCANCODE_AC_SEARCH"},{"AC\nFrwd",16,-14,4,4,SDLK_AC_FORWARD,SDL_SCANCODE_AC_FORWARD,"SDLK_AC_FORWARD","SDL_SCANCODE_AC_FORWARD"},{"Media\nSelect",20,-14,4,4,SDLK_MEDIASELECT,SDL_SCANCODE_MEDIASELECT,"SDLK_MEDIASELECT","SDL_SCANCODE_MEDIASELECT"},{"Prev\n|<<",26,-14,4,4,SDLK_AUDIOPREV,SDL_SCANCODE_AUDIOPREV,"SDLK_AUDIOPREV","SDL_SCANCODE_AUDIOPREV"},{"Play\n>||",30,-14,4,4,SDLK_AUDIOPLAY,SDL_SCANCODE_AUDIOPLAY,"SDLK_AUDIOPLAY","SDL_SCANCODE_AUDIOPLAY"},{"Next\n>>|",34,-14,4,4,SDLK_AUDIONEXT,SDL_SCANCODE_AUDIONEXT,"SDLK_AUDIONEXT","SDL_SCANCODE_AUDIONEXT"},{"Stop\n[]",38,-14,4,4,SDLK_AUDIOSTOP,SDL_SCANCODE_AUDIOSTOP,"SDLK_AUDIOSTOP","SDL_SCANCODE_AUDIOSTOP"},{"Brightns\nDown",44,-14,6,4,SDLK_BRIGHTNESSDOWN,SDL_SCANCODE_BRIGHTNESSDOWN,"SDLK_BRIGHTNESSDOWN","SDL_SCANCODE_BRIGHTNESSDOWN"},{"Brightns\nUp",50,-14,6,4,SDLK_BRIGHTNESSUP,SDL_SCANCODE_BRIGHTNESSUP,"SDLK_BRIGHTNESSUP","SDL_SCANCODE_BRIGHTNESSUP"},{"Help",56,-14,4,4,SDLK_HELP,SDL_SCANCODE_HELP,"SDLK_HELP","SDL_SCANCODE_HELP"},
    {"Lock\nCaps",61,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LOCKINGCAPSLOCK,"none","SDL_SCANCODE_LOCKINGCAPSLOCK"},{"Lock\nNum",65,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LOCKINGNUMLOCK,"none","SDL_SCANCODE_LOCKINGNUMLOCK"},{"Lock\nScroll",69,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LOCKINGSCROLLLOCK,"none","SDL_SCANCODE_LOCKINGSCROLLLOCK"},
    {"Unknown",0,-23,8,4,SDLK_UNKNOWN,SDL_SCANCODE_UNKNOWN,"SDLK_UNKNOWN","SDL_SCANCODE_UNKNOWN"},{"CrSel",8,-23,4,4,SDLK_CRSEL,SDL_SCANCODE_CRSEL,"SDLK_CRSEL","SDL_SCANCODE_CRSEL"},{"Alt\nErase",12,-23,4,4,SDLK_ALTERASE,SDL_SCANCODE_ALTERASE,"SDLK_ALTERASE","SDL_SCANCODE_ALTERASE"},{"Exec",16,-23,4,4,SDLK_EXECUTE,SDL_SCANCODE_EXECUTE,"SDLK_EXECUTE","SDL_SCANCODE_EXECUTE"},{"Stop",20,-23,4,4,SDLK_STOP,SDL_SCANCODE_STOP,"SDLK_STOP","SDL_SCANCODE_STOP"},
    {"Menu",0,-19,4,4,SDLK_MENU,SDL_SCANCODE_MENU,"SDLK_MENU","SDL_SCANCODE_MENU"},{"Out",4,-19,4,4,SDLK_OUT,SDL_SCANCODE_OUT,"SDLK_OUT","SDL_SCANCODE_OUT"},{"ExSel",8,-19,4,4,SDLK_EXSEL,SDL_SCANCODE_EXSEL,"SDLK_EXSEL","SDL_SCANCODE_EXSEL"},{"Prior",12,-19,4,4,SDLK_PRIOR,SDL_SCANCODE_PRIOR,"SDLK_PRIOR","SDL_SCANCODE_PRIOR"},{"Oper",16,-19,4,4,SDLK_OPER,SDL_SCANCODE_OPER,"SDLK_OPER","SDL_SCANCODE_OPER"},{"Cancel",20,-19,4,4,SDLK_CANCEL,SDL_SCANCODE_CANCEL,"SDLK_CANCEL","SDL_SCANCODE_CANCEL"},
    {"Undo",26,-19,4,4,SDLK_UNDO,SDL_SCANCODE_UNDO,"SDLK_UNDO","SDL_SCANCODE_UNDO"},{"Cut",30,-19,4,4,SDLK_CUT,SDL_SCANCODE_CUT,"SDLK_CUT","SDL_SCANCODE_CUT"},{"Copy",34,-19,4,4,SDLK_COPY,SDL_SCANCODE_COPY,"SDLK_COPY","SDL_SCANCODE_COPY"},{"Paste",38,-19,4,4,SDLK_PASTE,SDL_SCANCODE_PASTE,"SDLK_PASTE","SDL_SCANCODE_PASTE"},{"Calc",44,-19,4,4,SDLK_CALCULATOR,SDL_SCANCODE_CALCULATOR,"SDLK_CALCULATOR","SDL_SCANCODE_CALCULATOR"},{"Mode\nSwitch",48,-19,4,4,SDLK_MODE,SDL_SCANCODE_MODE,"SDLK_MODE","SDL_SCANCODE_MODE"},{"Mail",52,-19,4,4,SDLK_MAIL,SDL_SCANCODE_MAIL,"SDLK_MAIL","SDL_SCANCODE_MAIL"},{"Non\nUS \\",56,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_NONUSBACKSLASH,"none","SDL_SCANCODE_NONUSBACKSLASH"},
    {"Again\nRedo",26,-23,4,4,SDLK_AGAIN,SDL_SCANCODE_AGAIN,"SDLK_AGAIN","SDL_SCANCODE_AGAIN"},{"Find",30,-23,4,4,SDLK_FIND,SDL_SCANCODE_FIND,"SDLK_FIND","SDL_SCANCODE_FIND"},{"Select",34,-23,4,4,SDLK_SELECT,SDL_SCANCODE_SELECT,"SDLK_SELECT","SDL_SCANCODE_SELECT"},{"Return",38,-23,4,4,SDLK_RETURN2,SDL_SCANCODE_RETURN2,"SDLK_RETURN2","SDL_SCANCODE_RETURN2"},{"Comp",44,-23,4,4,SDLK_COMPUTER,SDL_SCANCODE_COMPUTER,"SDLK_COMPUTER","SDL_SCANCODE_COMPUTER"},{"Disp",48,-23,4,4,SDLK_DISPLAYSWITCH,SDL_SCANCODE_DISPLAYSWITCH,"SDLK_DISPLAYSWITCH","SDL_SCANCODE_DISPLAYSWITCH"},{"WWW",52,-23,4,4,SDLK_WWW,SDL_SCANCODE_WWW,"SDLK_WWW","SDL_SCANCODE_WWW"},{"Non\nUS #",56,-23,4,4,SDLK_UNKNOWN,SDL_SCANCODE_NONUSHASH,"none","SDL_SCANCODE_NONUSHASH"},
    {"Currency\nUnit",61,-19,6,4,SDLK_CURRENCYUNIT,SDL_SCANCODE_CURRENCYUNIT,"SDLK_CURRENCYUNIT","SDL_SCANCODE_CURRENCYUNIT"},{"Currency\nSub Unit",67,-19,6,4,SDLK_CURRENCYSUBUNIT,SDL_SCANCODE_CURRENCYSUBUNIT,"SDLK_CURRENCYSUBUNIT","SDL_SCANCODE_CURRENCYSUBUNIT"},{"Separ",61,-23,4,4,SDLK_SEPARATOR,SDL_SCANCODE_SEPARATOR,"SDLK_SEPARATOR","SDL_SCANCODE_SEPARATOR"},{"Dec\nSepar",65,-23,4,4,SDLK_DECIMALSEPARATOR,SDL_SCANCODE_DECIMALSEPARATOR,"SDLK_DECIMALSEPARATOR","SDL_SCANCODE_DECIMALSEPARATOR"},{"Thous\nSepar",69,-23,4,4,SDLK_THOUSANDSSEPARATOR,SDL_SCANCODE_THOUSANDSSEPARATOR,"SDLK_THOUSANDSSEPARATOR","SDL_SCANCODE_THOUSANDSSEPARATOR"},
};

const KeyBox Keyboard_Complete_Numpad[] = {
    {"Inter\n1",74,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL1,"none","SDL_SCANCODE_INTERNATIONAL1"},{"Inter\n2",78,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL2,"none","SDL_SCANCODE_INTERNATIONAL2"},{"Inter\n3",82,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL3,"none","SDL_SCANCODE_INTERNATIONAL3"},{"Inter\n4",87,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL4,"none","SDL_SCANCODE_INTERNATIONAL4"},{"Inter\n5",91,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL5,"none","SDL_SCANCODE_INTERNATIONAL5"},{"Inter\n6",95,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL6,"none","SDL_SCANCODE_INTERNATIONAL6"},{"Inter\n7",100,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL7,"none","SDL_SCANCODE_INTERNATIONAL7"},{"Inter\n8",104,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL8,"none","SDL_SCANCODE_INTERNATIONAL8"},{"Inter\n9",108,-19,4,4,SDLK_UNKNOWN,SDL_SCANCODE_INTERNATIONAL9,"none","SDL_SCANCODE_INTERNATIONAL9"},
    {"Lang\n1",74,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG1,"none","SDL_SCANCODE_LANG1"},{"Lang\n2",78,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG2,"none","SDL_SCANCODE_LANG2"},{"Lang\n3",82,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG3,"none","SDL_SCANCODE_LANG3"},
    {"Lang\n4",87,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG4,"none","SDL_SCANCODE_LANG4"},{"Lang\n5",91,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG5,"none","SDL_SCANCODE_LANG5"},{"Lang\n6",95,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG6,"none","SDL_SCANCODE_LANG6"},
    {"Lang\n7",100,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG7,"none","SDL_SCANCODE_LANG7"},{"Lang\n8",104,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG8,"none","SDL_SCANCODE_LANG8"},{"Lang\n9",108,-14,4,4,SDLK_UNKNOWN,SDL_SCANCODE_LANG9,"none","SDL_SCANCODE_LANG9"},
    {"Clear",74,-7,6,4,SDLK_KP_CLEAR,SDL_SCANCODE_KP_CLEARENTRY,"SDLK_KP_CLEAR","SDL_SCANCODE_KP_CLEARENTRY"},{"Clear\nEntry",80,-7,6,4,SDLK_KP_CLEARENTRY,SDL_SCANCODE_KP_CLEARENTRY,"SDLK_KP_CLEARENTRY","SDL_SCANCODE_KP_CLEARENTRY"},{"Backspace",86,-7,8,4,SDLK_KP_BACKSPACE,SDL_SCANCODE_KP_BACKSPACE,"SDLK_KP_BACKSPACE","SDL_SCANCODE_KP_BACKSPACE"},
    {"/",86,-3,4,4,SDLK_KP_DIVIDE,SDL_SCANCODE_KP_DIVIDE,"SDLK_KP_DIVIDE","SDL_SCANCODE_KP_DIVIDE"},{"%",90,-3,4,4,SDLK_KP_PERCENT,SDL_SCANCODE_KP_PERCENT,"SDLK_KP_PERCENT","SDL_SCANCODE_KP_PERCENT"},{"D",74,-3,4,4,SDLK_KP_D,SDL_SCANCODE_KP_D,"SDLK_KP_D","SDL_SCANCODE_KP_D"},{"E",78,-3,4,4,SDLK_KP_E,SDL_SCANCODE_KP_E,"SDLK_KP_E","SDL_SCANCODE_KP_E"},{"F",82,-3,4,4,SDLK_KP_F,SDL_SCANCODE_KP_F,"SDLK_KP_F","SDL_SCANCODE_KP_F"},
    {"*",86,1,4,4,SDLK_KP_MULTIPLY,SDL_SCANCODE_KP_MULTIPLY,"SDLK_KP_MULTIPLY","SDL_SCANCODE_KP_MULTIPLY"},{"^",90,1,4,4,SDLK_KP_POWER,SDL_SCANCODE_KP_POWER,"SDLK_KP_POWER","SDL_SCANCODE_KP_POWER"},{"A",74,1,4,4,SDLK_KP_A,SDL_SCANCODE_KP_A,"SDLK_KP_A","SDL_SCANCODE_KP_A"},{"B",78,1,4,4,SDLK_KP_B,SDL_SCANCODE_KP_B,"SDLK_KP_B","SDL_SCANCODE_KP_B"},{"C",82,1,4,4,SDLK_KP_C,SDL_SCANCODE_KP_C,"SDLK_KP_C","SDL_SCANCODE_KP_C"},
    {"7\nHome",74,5,4,4,SDLK_KP_7,SDL_SCANCODE_KP_7,"SDLK_KP_7","SDL_SCANCODE_KP_7"},{"8^\n |",78,5,4,4,SDLK_KP_8,SDL_SCANCODE_KP_8,"SDLK_KP_8","SDL_SCANCODE_KP_8"},{"9\nPg Up",82,5,4,4,SDLK_KP_9,SDL_SCANCODE_KP_9,"SDLK_KP_9","SDL_SCANCODE_KP_9"},{"-",86,5,4,4,SDLK_KP_MINUS,SDL_SCANCODE_KP_MINUS,"SDLK_KP_MINUS","SDL_SCANCODE_KP_MINUS"},{"+/-",90,5,4,4,SDLK_KP_PLUSMINUS,SDL_SCANCODE_KP_PLUSMINUS,"SDLK_KP_PLUSMINUS","SDL_SCANCODE_KP_PLUSMINUS"},
    {"<-4",74,9,4,4,SDLK_KP_4,SDL_SCANCODE_KP_4,"SDLK_KP_4","SDL_SCANCODE_KP_4"},{"5",78,9,4,4,SDLK_KP_5,SDL_SCANCODE_KP_5,"SDLK_KP_5","SDL_SCANCODE_KP_5"},{"6->",82,9,4,4,SDLK_KP_6,SDL_SCANCODE_KP_6,"SDLK_KP_6","SDL_SCANCODE_KP_6"},{"+",86,9,4,8,SDLK_KP_PLUS,SDL_SCANCODE_KP_PLUS,"SDLK_KP_PLUS","SDL_SCANCODE_KP_PLUS"},{"Dec",90,9,4,4,SDLK_KP_DECIMAL,SDL_SCANCODE_KP_DECIMAL,"SDLK_KP_DECIMAL","SDL_SCANCODE_KP_DECIMAL"},
    {"1\nEnd",74,13,4,4,SDLK_KP_1,SDL_SCANCODE_KP_1,"SDLK_KP_1","SDL_SCANCODE_KP_1"},{"2|\n V",78,13,4,4,SDLK_KP_2,SDL_SCANCODE_KP_2,"SDLK_KP_2","SDL_SCANCODE_KP_2"},{"3\nPg Dn",82,13,4,4,SDLK_KP_3,SDL_SCANCODE_KP_3,"SDLK_KP_3","SDL_SCANCODE_KP_3"},{"Hex",90,13,4,4,SDLK_KP_HEXADECIMAL,SDL_SCANCODE_KP_HEXADECIMAL,"SDLK_KP_HEXADECIMAL","SDL_SCANCODE_KP_HEXADECIMAL"},
    {"0\nIns",74,17,8,4,SDLK_KP_0,SDL_SCANCODE_KP_0,"SDLK_KP_0","SDL_SCANCODE_KP_0"},{".\nDel",82,17,4,4,SDLK_KP_PERIOD,SDL_SCANCODE_KP_PERIOD,"SDLK_KP_PERIOD","SDL_SCANCODE_KP_PERIOD"},{"Enter",86,17,4,8,SDLK_KP_ENTER,SDL_SCANCODE_KP_ENTER,"SDLK_KP_ENTER","SDL_SCANCODE_KP_ENTER"},{"Oct",90,17,4,4,SDLK_KP_OCTAL,SDL_SCANCODE_KP_OCTAL,"SDLK_KP_OCTAL","SDL_SCANCODE_KP_OCTAL"},
    {"00",74,21,4,4,SDLK_KP_00,SDL_SCANCODE_KP_00,"SDLK_KP_00","SDL_SCANCODE_KP_00"},{"000",78,21,4,4,SDLK_KP_000,SDL_SCANCODE_KP_000,"SDLK_KP_000","SDL_SCANCODE_KP_000"},{"=",82,21,4,4,SDLK_KP_EQUALS,SDL_SCANCODE_KP_EQUALS,"SDLK_KP_EQUALS","SDL_SCANCODE_KP_EQUALS"},{"Bin",90,21,4,4,SDLK_KP_BINARY,SDL_SCANCODE_KP_BINARY,"SDLK_KP_BINARY","SDL_SCANCODE_KP_BINARY"},
    {"MR",96,-7,8,4,SDLK_KP_MEMRECALL,SDL_SCANCODE_KP_MEMRECALL,"SDLK_KP_MEMRECALL","SDL_SCANCODE_KP_MEMRECALL"},{"Num\nLock",104,-7,8,4,SDLK_NUMLOCKCLEAR,SDL_SCANCODE_NUMLOCKCLEAR,"SDLK_NUMLOCKCLEAR","SDL_SCANCODE_NUMLOCKCLEAR"},
    {"M/",96,-3,4,4,SDLK_KP_MEMDIVIDE,SDL_SCANCODE_KP_MEMDIVIDE,"SDLK_KP_MEMDIVIDE","SDL_SCANCODE_KP_MEMDIVIDE"},{"MS",100,-3,4,4,SDLK_KP_MEMSTORE,SDL_SCANCODE_KP_MEMSTORE,"SDLK_KP_MEMSTORE","SDL_SCANCODE_KP_MEMSTORE"},{"MC",104,-3,4,4,SDLK_KP_MEMCLEAR,SDL_SCANCODE_KP_MEMCLEAR,"SDLK_KP_MEMCLEAR","SDL_SCANCODE_KP_MEMCLEAR"},{"Equals\nAS400",108,-3,4,4,SDLK_KP_EQUALSAS400,SDL_SCANCODE_KP_EQUALSAS400,"SDLK_KP_EQUALSAS400","SDL_SCANCODE_KP_EQUALSAS400"},
    {"M*",96,1,4,4,SDLK_KP_MEMMULTIPLY,SDL_SCANCODE_KP_MEMMULTIPLY,"SDLK_KP_MEMMULTIPLY","SDL_SCANCODE_KP_MEMMULTIPLY"},{"(",100,1,4,4,SDLK_KP_LEFTPAREN,SDL_SCANCODE_KP_LEFTPAREN,"SDLK_KP_LEFTPAREN","SDL_SCANCODE_KP_LEFTPAREN"},{")",104,1,4,4,SDLK_KP_RIGHTPAREN,SDL_SCANCODE_KP_RIGHTPAREN,"SDLK_KP_RIGHTPAREN","SDL_SCANCODE_KP_RIGHTPAREN"},{"@",108,1,4,4,SDLK_KP_AT,SDL_SCANCODE_KP_AT,"SDLK_KP_AT","SDL_SCANCODE_KP_AT"},
    {"M-",96,5,4,4,SDLK_KP_MEMSUBTRACT,SDL_SCANCODE_KP_MEMSUBTRACT,"SDLK_KP_MEMSUBTRACT","SDL_SCANCODE_KP_MEMSUBTRACT"},{"{",100,5,4,4,SDLK_KP_LEFTBRACE,SDL_SCANCODE_KP_LEFTBRACE,"SDLK_KP_LEFTBRACE","SDL_SCANCODE_KP_LEFTBRACE"},{"}",104,5,4,4,SDLK_KP_RIGHTBRACE,SDL_SCANCODE_KP_RIGHTBRACE,"SDLK_KP_RIGHTBRACE","SDL_SCANCODE_KP_RIGHTBRACE"},{":",108,5,4,4,SDLK_KP_COLON,SDL_SCANCODE_KP_COLON,"SDLK_KP_COLON","SDL_SCANCODE_KP_COLON"},
    {"M+",96,9,4,4,SDLK_KP_MEMADD,SDL_SCANCODE_KP_MEMADD,"SDLK_KP_MEMADD","SDL_SCANCODE_KP_MEMADD"},{"<",100,9,4,4,SDLK_KP_LESS,SDL_SCANCODE_KP_LESS,"SDLK_KP_LESS","SDL_SCANCODE_KP_LESS"},{">",104,9,4,4,SDLK_KP_GREATER,SDL_SCANCODE_KP_GREATER,"SDLK_KP_GREATER","SDL_SCANCODE_KP_GREATER"},{"#",108,9,4,4,SDLK_KP_HASH,SDL_SCANCODE_KP_HASH,"SDLK_KP_HASH","SDL_SCANCODE_KP_HASH"},
    {"&",96,13,6,4,SDLK_KP_AMPERSAND,SDL_SCANCODE_KP_AMPERSAND,"SDLK_KP_AMPERSAND","SDL_SCANCODE_KP_AMPERSAND"},{"&&",102,13,6,4,SDLK_KP_DBLAMPERSAND,SDL_SCANCODE_KP_DBLAMPERSAND,"SDLK_KP_DBLAMPERSAND","SDL_SCANCODE_KP_DBLAMPERSAND"},{"Space",108,13,4,4,SDLK_KP_SPACE,SDL_SCANCODE_KP_SPACE,"SDLK_KP_SPACE","SDL_SCANCODE_KP_SPACE"},
    {"|",96,17,6,4,SDLK_KP_VERTICALBAR,SDL_SCANCODE_KP_VERTICALBAR,"SDLK_KP_VERTICALBAR","SDL_SCANCODE_KP_VERTICALBAR"},{"||",102,17,6,4,SDLK_KP_DBLVERTICALBAR,SDL_SCANCODE_KP_DBLVERTICALBAR,"SDLK_KP_DBLVERTICALBAR","SDL_SCANCODE_KP_DBLVERTICALBAR"},{"Tab",108,17,4,4,SDLK_KP_TAB,SDL_SCANCODE_KP_TAB,"SDLK_KP_TAB","SDL_SCANCODE_KP_TAB"},
    {"XOR",96,21,6,4,SDLK_KP_XOR,SDL_SCANCODE_KP_XOR,"SDLK_KP_XOR","SDL_SCANCODE_KP_XOR"},{"!",102,21,6,4,SDLK_KP_EXCLAM,SDL_SCANCODE_KP_EXCLAM,"SDLK_KP_EXCLAM","SDL_SCANCODE_KP_EXCLAM"},{",",108,21,4,4,SDLK_KP_COMMA,SDL_SCANCODE_KP_COMMA,"SDLK_KP_COMMA","SDL_SCANCODE_KP_COMMA"},
};

#endif /* KEYBIND_DATA_H */