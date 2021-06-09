
void audio_play_sound(sound* s,  s8 channel)
{
	if (!s->loaded) return;
	Mix_PlayChannel(channel, s->chunk, 0);
}

void audio_set_sound_volume(sound* s, float p)
{
	if (!s->loaded) return;
	Mix_VolumeChunk(s->chunk, 128*p);
}

void audio_set_mixer_volume(s8 channel, float p)
{
	Mix_Volume(channel, 128*p);
}