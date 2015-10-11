#ifndef __KEYBOARD_H
#define __KEYBOARD_H

int keyboard_subscribe_int();

int keyboard_unsubscribe_int();

unsigned long readFromKBC();

int exit_on_ESC();

#endif
