#ifndef __input_h
#define __input_h

#include <stdint.h>

void InputInit(void);

uint8_t GetButtonPush(void);
void ClearButtonPush(void);

uint32_t JSgetX(void);
uint32_t JSgetY(void);

int8_t GetX(void);
int8_t GetY(void);


#endif /* __input_h */
