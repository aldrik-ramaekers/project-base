/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_AUDIO
#define INCLUDE_AUDIO

void audio_play_sound(sound* s,  s8 channel);
void audio_set_sound_volume(sound* s, float p);
void audio_set_mixer_volume(s8 channel, float p);

#endif