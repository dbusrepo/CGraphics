#pragma once

/* key and button state/action definitions */
#define KEY_RELEASE            0
#define KEY_PRESS              1

/* Keyboard key definitions: 8-bit ISO-8859-1 (Latin 1) encoding is used
 * for printable keys (such as A-Z, 0-9 etc), and values above 256
 * represent special (non-printable) keys (e.g. F1, Page Up etc).
 */
#define KEY_UNKNOWN      -1
#define KEY_SPACE        32
#define KEY_SPECIAL      256
#define KEY_ESC          (KEY_SPECIAL+1)
#define KEY_F1           (KEY_SPECIAL+2)
#define KEY_F2           (KEY_SPECIAL+3)
#define KEY_F3           (KEY_SPECIAL+4)
#define KEY_F4           (KEY_SPECIAL+5)
#define KEY_F5           (KEY_SPECIAL+6)
#define KEY_F6           (KEY_SPECIAL+7)
#define KEY_F7           (KEY_SPECIAL+8)
#define KEY_F8           (KEY_SPECIAL+9)
#define KEY_F9           (KEY_SPECIAL+10)
#define KEY_F10          (KEY_SPECIAL+11)
#define KEY_F11          (KEY_SPECIAL+12)
#define KEY_F12          (KEY_SPECIAL+13)
#define KEY_F13          (KEY_SPECIAL+14)
#define KEY_F14          (KEY_SPECIAL+15)
#define KEY_F15          (KEY_SPECIAL+16)
#define KEY_F16          (KEY_SPECIAL+17)
#define KEY_F17          (KEY_SPECIAL+18)
#define KEY_F18          (KEY_SPECIAL+19)
#define KEY_F19          (KEY_SPECIAL+20)
#define KEY_F20          (KEY_SPECIAL+21)
#define KEY_F21          (KEY_SPECIAL+22)
#define KEY_F22          (KEY_SPECIAL+23)
#define KEY_F23          (KEY_SPECIAL+24)
#define KEY_F24          (KEY_SPECIAL+25)
#define KEY_F25          (KEY_SPECIAL+26)
#define KEY_UP           (KEY_SPECIAL+27)
#define KEY_DOWN         (KEY_SPECIAL+28)
#define KEY_LEFT         (KEY_SPECIAL+29)
#define KEY_RIGHT        (KEY_SPECIAL+30)
#define KEY_LSHIFT       (KEY_SPECIAL+31)
#define KEY_RSHIFT       (KEY_SPECIAL+32)
#define KEY_LCTRL        (KEY_SPECIAL+33)
#define KEY_RCTRL        (KEY_SPECIAL+34)
#define KEY_LALT         (KEY_SPECIAL+35)
#define KEY_RALT         (KEY_SPECIAL+36)
#define KEY_TAB          (KEY_SPECIAL+37)
#define KEY_ENTER        (KEY_SPECIAL+38)
#define KEY_BACKSPACE    (KEY_SPECIAL+39)
#define KEY_INSERT       (KEY_SPECIAL+40)
#define KEY_DEL          (KEY_SPECIAL+41)
#define KEY_PAGEUP       (KEY_SPECIAL+42)
#define KEY_PAGEDOWN     (KEY_SPECIAL+43)
#define KEY_HOME         (KEY_SPECIAL+44)
#define KEY_END          (KEY_SPECIAL+45)
#define KEY_KP_0         (KEY_SPECIAL+46)
#define KEY_KP_1         (KEY_SPECIAL+47)
#define KEY_KP_2         (KEY_SPECIAL+48)
#define KEY_KP_3         (KEY_SPECIAL+49)
#define KEY_KP_4         (KEY_SPECIAL+50)
#define KEY_KP_5         (KEY_SPECIAL+51)
#define KEY_KP_6         (KEY_SPECIAL+52)
#define KEY_KP_7         (KEY_SPECIAL+53)
#define KEY_KP_8         (KEY_SPECIAL+54)
#define KEY_KP_9         (KEY_SPECIAL+55)
#define KEY_KP_DIVIDE    (KEY_SPECIAL+56)
#define KEY_KP_MULTIPLY  (KEY_SPECIAL+57)
#define KEY_KP_SUBTRACT  (KEY_SPECIAL+58)
#define KEY_KP_ADD       (KEY_SPECIAL+59)
#define KEY_KP_DECIMAL   (KEY_SPECIAL+60)
#define KEY_KP_EQUAL     (KEY_SPECIAL+61)
#define KEY_KP_ENTER     (KEY_SPECIAL+62)
#define KEY_KP_NUM_LOCK  (KEY_SPECIAL+63)
#define KEY_CAPS_LOCK    (KEY_SPECIAL+64)
#define KEY_SCROLL_LOCK  (KEY_SPECIAL+65)
#define KEY_PAUSE        (KEY_SPECIAL+66)
#define KEY_LSUPER       (KEY_SPECIAL+67)
#define KEY_RSUPER       (KEY_SPECIAL+68)
#define KEY_MENU         (KEY_SPECIAL+69)
#define KEY_LAST         KEY_MENU

/* Mouse button definitions */
#define MOUSE_BUTTON_1      0
#define MOUSE_BUTTON_2      1
#define MOUSE_BUTTON_3      2
#define MOUSE_BUTTON_4      3
#define MOUSE_BUTTON_5      4
#define MOUSE_BUTTON_6      5
#define MOUSE_BUTTON_7      6
#define MOUSE_BUTTON_8      7
#define MOUSE_BUTTON_LAST   MOUSE_BUTTON_8

/* Mouse button aliases */
#define MOUSE_BUTTON_LEFT   MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT  MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE MOUSE_BUTTON_3
