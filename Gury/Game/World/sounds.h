#pragma once

#include "fmod.hpp"

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Services/content.h"
#include "../Gury/Game/Services/stdout.h"

#include <vector>
#include <string>

#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fmodstudio_vc.lib")

namespace RBX
{
	class Sound : public RBX::Instance
	{
	private:
		FMOD::Sound* sound;
		FMOD::Channel* channel;
		bool b3d;
	public:
		bool isLooping;
		bool stream;

		float startPosition, length;
		float volume;

		Content soundId;

		void playOnce();
		void play();
		void stop();

		void updateSound();

		void setSoundId(Content content) {
			soundId = content;
			if (soundId.resolve()) {
				updateSound();
			}
		}

		Content getSoundId() {
			return soundId;
		}

		bool isPlaying()
		{ 
			bool b; 
			channel->isPlaying(&b);
			return b; 
		}
		double getStartPosition() 
		{ 
			unsigned int position; 
			channel->getPosition(&position, FMOD_TIMEUNIT_MS); 
			return ((double)position) / 1000.0; 
		}
		void setStartPosition(double value);

		void setEndPosition(double value);

		double getLength() 
		{ 
			unsigned int len;
			sound->getLength(&len, FMOD_TIMEUNIT_MS);
			return ((double)len) / 1000.0;
		}

		void setVolume(float vol)
		{ 
			volume = vol; 
		}

		float getVolume()
		{ 
			return volume;
		}

		static RBX::Sound* fromFile(std::string file, bool isLooped = 0)
		{
			RBX::Sound* s = new RBX::Sound();
			s->setSoundId(Content::fromStoredContent(file));
			s->name = file;
			s->isLooping = isLooped;
			return s;
		}

		Sound()
		{
			className = "Sound";
			name = "Sound";
			volume = 1.0f;
		}

		virtual ~Sound() { 
			sound->release();
		}

		RTTR_ENABLE(Instance);
	};

}
