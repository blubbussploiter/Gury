#include "../Gury/Game/Services/soundservice.h"
#include "../Gury/Game/World/sounds.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::Sound>("Sound")
		 .constructor<>()
		 .property("SoundId", &RBX::Sound::getSoundId, &RBX::Sound::setSoundId)
		 .method("Play", &RBX::Sound::play)
		 .method("play", &RBX::Sound::play)
		 .method("Stop", &RBX::Sound::stop)
		 .method("stop", &RBX::Sound::stop);
}

void RBX::Sound::stop()
{
	channel->stop();
}

void RBX::Sound::updateSound()
{
	FMOD_CREATESOUNDEXINFO* createSoundInfo = new FMOD_CREATESOUNDEXINFO();

	createSoundInfo->cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	createSoundInfo->length = soundId.contentLength;

	SoundService::get()->mpSystem->createSound((const char*)soundId.content, FMOD_OPENMEMORY | FMOD_CREATESAMPLE, createSoundInfo, &sound);
}

void RBX::Sound::setStartPosition(double value)
{
	double oldPosition = getStartPosition();
	double soundLength = getLength();
	double newPosition = fmod(value, (soundLength ? soundLength : 1.0));
	if (oldPosition != newPosition)
	{
		startPosition = newPosition * 1000.0f;
	}
}

void RBX::Sound::setEndPosition(double value)
{

}

void RBX::Sound::playOnce()
{
	if (!isPlaying())
	{
		play();
	}
}

void RBX::Sound::play()
{
	if (sound) {
		sound->setMode(FMOD_LOOP_OFF);

		SoundService::get()->mpSystem->playSound(sound, 0, 0, &channel);

		channel->setPosition(startPosition, FMOD_TIMEUNIT_MS);
		channel->setVolume(volume);
	}
}