
/*
void audio_play_sound(sound* s,  s8 channel)
{
	if (!s->loaded) return;
	if (s->is_music) Mix_PlayMusic(s->music, 1);
	else Mix_PlayChannel(channel, s->chunk, 0);
}

void audio_set_sound_volume(sound* s, float p)
{
	if (!s->loaded) return;
	if (s->is_music) Mix_VolumeMusic(128*p);
	else Mix_VolumeChunk(s->chunk, 128*p);
}

void audio_set_music_volume(float p)
{
	Mix_VolumeMusic(128*p);
}

void audio_set_mixer_volume(s8 channel, float p)
{
	Mix_Volume(channel, 128*p);
}

bool audio_music_is_playing()
{
	return Mix_PlayingMusic();
}

*/