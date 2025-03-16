#ifndef PCSPK_H
#define PCSPK_H

#include <stdint.h>

void pcspk_beep(uint32_t);
void pcspk_stop();
void pcspk_beep_dur(uint32_t, uint32_t);

#endif  // PCSPK_H
