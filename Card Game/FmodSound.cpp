#include "FmodSound.h"

bool Sound::s_enabled = true; //Is the sound Enabled?
char *Sound::s_current_sound; //current sound to play
//FMOD system stuff

FMOD_RESULT Sound::s_result;
FMOD_SYSTEM *Sound::s_fmod_system;
FMOD_SOUND *Sound::s_sound;
FMOD_CHANNEL *Sound::s_channel;

void Sound::Init(int _channels)
{
	s_result = FMOD_System_Create(&s_fmod_system);
	assert(s_result == FMOD_OK);
	//Initializes the system with 1 channel
	s_result = FMOD_System_Init(s_fmod_system, _channels, FMOD_INIT_NORMAL, 0);
	assert(s_result == FMOD_OK);
	FMOD_Channel_SetVolume(s_channel, 0.0f);
}
void Sound::Update()
{
	FMOD_System_Update(s_fmod_system);
}
int count = 1;

FMOD_SYSTEM* Sound::GetSystem()
{
	return s_fmod_system;
}

void Sound::SetVolume(float vol)
{
	if (s_enabled && vol >= 0.0f && vol <= 1.0f)
	{
		FMOD_Channel_SetVolume(s_channel, vol);
	}
}

void Sound::IncreaseVolume()
{
	if (s_enabled)
	{
		FMOD_Channel_SetVolume(s_channel,0 );
	}
}

//loads a soundfile
void Sound::Load(const char * filename)
{
	s_current_sound = (char *)filename;
	if (s_enabled)
	{
		s_result = FMOD_Sound_Release(s_sound);
		s_result = FMOD_System_CreateStream(s_fmod_system, s_current_sound, FMOD_SOFTWARE, 0, &s_sound);
		assert(s_result == FMOD_OK);
	}
}




void Sound::Unload(void)
{
	s_result = FMOD_Sound_Release(s_sound);
	assert(s_result == FMOD_OK);
}




void Sound::Play(bool pause = false) //No agument is needed to play by default
{
	if (true == s_enabled)
	{
		s_result = FMOD_System_PlaySound(s_fmod_system, FMOD_CHANNEL_FREE, s_sound, pause, &s_channel);
		assert(s_result == FMOD_OK);
		FMOD_Channel_SetMode(s_channel, FMOD_LOOP_NORMAL);
	}
}


bool Sound::GetSoundState()
{
	return s_enabled;
}
//Gets the name of the current sound
char* Sound::GetCurrentSound()
{
	return s_current_sound;
}




//pause or unpause the sound
void Sound::SetPause(bool pause)
{
	FMOD_Channel_SetPaused(s_channel, pause);
}


//turn sound on or off
void Sound::SetSound(bool s)
{
	s_enabled = s;
}


//toggles sound on and off
void Sound::ToggleSound(void)
{
	s_enabled = !s_enabled;
	if (s_enabled == true)
	{
		Load(s_current_sound);
		Play(s_enabled);
	}
	if (s_enabled == false)
	{
		Unload();
	}
}


//toggle pause on and off
void Sound::TogglePause(void)
{
	FMOD_BOOL p;
	FMOD_Channel_GetPaused(s_channel, &p);
	FMOD_Channel_SetPaused(s_channel, !p);
}

//-------------Sound Effects---------------//
SoundEffect::SoundEffect(char* filename)
{
	b_enabled = true;
	m_sound_name = filename;
	m_result = FMOD_System_CreateSound(Sound::GetSystem(), m_sound_name, FMOD_SOFTWARE, 0, &m_sound);
	assert(m_result == FMOD_OK);
}
void SoundEffect::Play()
{
	m_result = FMOD_System_PlaySound(Sound::GetSystem(), FMOD_CHANNEL_FREE, m_sound, false, 0);
	assert(m_result == FMOD_OK);
}