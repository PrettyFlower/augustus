#include "Video.h"

extern "C" {
  #include "Video/smacker.h"
}

#include <cstdio> // debug
#include <stdint.h>

#include "FileSystem.h"
#include "Graphics.h"
#include "Sound.h"
#include "Time.h"
#include "Data/Screen.h"
//#include "AllWindows.h"

static struct {
	int isPlaying;
	int isEnded;

	struct {
		smk s;
		int width;
		int height;
		int microsPerFrame;
		TimeMillis startRenderMillis;
		int currentFrame;
		int totalFrames;
	} video;
	struct {
		smk s;
		int cur;
		int len;
		char *data;
	} audio;
} data;

static void closeSmk(smk *s)
{
	smk_close(*s);
	*s = 0;
}

static void closeAll()
{
	if (data.video.s) {
		closeSmk(&data.video.s);
	}
	if (data.audio.s) {
		closeSmk(&data.audio.s);
		data.audio.cur = data.audio.len; // force flush of buffer
	}
	data.isEnded = 1;
}

static int loadSmkVideo(const char *filename)
{
	printf("Opening video file %s\n", filename);
	data.video.s = smk_open_file(filename, SMK_MODE_DISK);
	if (!data.video.s) {
		printf("Unable to open video file %s\n", filename);
		return 0;
	}
	
	unsigned long width, height, frames;
	double microsPerFrame;
	smk_info_all(data.video.s, NULL, &frames, &microsPerFrame);
	smk_info_video(data.video.s, &width, &height, NULL);
	
	data.video.width = width;
	data.video.height = height;
	data.video.currentFrame = 0;
	data.video.totalFrames = frames;
	data.video.microsPerFrame = (int) (microsPerFrame);
	printf("width: %d, height: %d, usec/frame: %d, frames: %d\n",
		   data.video.width, data.video.height, data.video.microsPerFrame, data.video.totalFrames);

	smk_enable_all(data.video.s,SMK_VIDEO_TRACK);
	if (smk_first(data.video.s) < 0) {
		closeSmk(&data.video.s);
		return 0;
	}
	return 1;
}

static int loadSmkAudio(const char *filename)
{
	printf("Opening video file %s for audio\n", filename);
	data.audio.s = smk_open_file(filename, SMK_MODE_DISK);
	if (!data.audio.s) {
		printf("Unable to open video file %s\n", filename);
		return 0;
	}
	
	unsigned char tracks, channels[7], bitdepths[7];
	unsigned long bitrates[7];
	smk_info_audio(data.audio.s, &tracks, channels, bitdepths, bitrates);
	
	if (tracks != 1) {
		printf("Video has alternate audio tracks, not supported: %x\n", tracks);
		closeSmk(&data.audio.s);
		return 0;
	}
	smk_enable_all(data.audio.s, tracks);
	if (smk_first(data.audio.s) < 0) {
		closeSmk(&data.audio.s);
		return 0;
	}
	/*
	// set up audio	audio;
	SDL_AudioSpec audioSpec;
	audioSpec.freq = a_r[0];
	audioSpec.format = AUDIO_U8;
	audioSpec.channels = channels[0];
	audioSpec.samples = 4096;
	audioSpec.callback = PlayAudio;
	
	int device = SDL_OpenAudio(&audioSpec, 0);
	if (device < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open audio device: %s\n", SDL_GetError());
		CloseVideo(&audio.s);
		return;
	}
	*/
	return 1;
}

int loadSmk(const char *filename)
{
	const char *path = FileSystem_getCaseSensitiveFile(filename);
	if (!path) {
		return 0;
	}
	if (loadSmkVideo(path) && loadSmkAudio(path)) {
		return 1;
	} else {
		closeAll();
		return 0;
	}
}

static void endVideo()
{
	Sound_Music_reset();
	Sound_Music_update();
}

int Video_start(const char *filename)
{
	data.isPlaying = 0;
	data.isEnded = 0;
	
	if (loadSmk(filename)) {
		Sound_stopMusic();
		Sound_stopSpeech();
		// refresh video buffer?
		data.isPlaying = 1;
		return 1;
	} else {
		return 0;
	}
}

void Video_stop()
{
	if (data.isPlaying) {
		if (data.video.s) {
			closeAll();
		}
		data.isPlaying = 0;
		if (!data.isEnded) {
			endVideo();
		}
	}
}

void Video_shutdown()
{
	if (data.isPlaying) {
		if (data.video.s) {
			closeAll();
		}
		data.isPlaying = 0;
	}
}

int Video_isFinished()
{
	return data.isEnded;
}

void Video_init()
{
	data.video.startRenderMillis = Time_getMillis();
	// TODO start audio
}

void Video_draw(int xOffset, int yOffset)
{
	if (!data.video.s) {
		return;
	}
	TimeMillis nowMillis = Time_getMillis();
	
	int frameNo = (nowMillis - data.video.startRenderMillis) * 1000 / data.video.microsPerFrame;
	if (frameNo > data.video.currentFrame) {
		if (smk_next(data.video.s) == SMK_DONE) {
			closeSmk(&data.video.s);
			data.isEnded = 1;
			data.isPlaying = 0;
			endVideo();
			return;
		}
		data.video.currentFrame++;
	}
	
	unsigned char *frame = smk_get_video(data.video.s);
	unsigned char *pal = smk_get_palette(data.video.s);
	if (frame && pal) {
		for (int y = 0; y < data.video.height; y++) {
			for (int x = 0; x < data.video.width; x++) {
				uint32_t color = 0xFF000000 |
					(pal[frame[(y * data.video.width) + x] * 3] << 16) |
					(pal[frame[(y * data.video.width) + x] * 3 + 1] << 8) |
					(pal[frame[(y * data.video.width) + x] * 3 + 2]);
				ScreenPixel(xOffset + x, yOffset + y) = color;
			}
		}
	}
}
