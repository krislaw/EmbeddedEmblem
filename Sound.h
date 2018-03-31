#ifndef __sound_h
#define __sound_h

#include <stdint.h>

void SoundInit(void);

void SetSong(uint16_t);

void PlaySong(void); //song is on loop until song is changed

void StopSong(void); //pause

void SetSoundEffect(uint16_t);

void PlaySoundEffect(void); //sound effect plays once

#endif /* __stdint_h */
