#include <am.h>

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  // kbd->keydown = 0;
  // kbd->keycode = AM_KEY_NONE;

  int c = uart_getc_nonblock();
  if(c == -1 ){
    kbd->keydown = 0;
    kbd->keycode = AM_KEY_NONE;
  }else{
    kbd->keydown = 1;
    if(c == 'a' || c == 'A')
      kbd->keycode = AM_KEY_A;
    else if(c == 'w' || c == 'W')
      kbd->keycode = AM_KEY_W;
    else if(c == 's' || c == 'S')
      kbd->keycode = AM_KEY_S;
    else if(c == 'd' || c == 'D')
      kbd->keycode = AM_KEY_D;
    else
      kbd->keycode = AM_KEY_NONE;
  }

}
