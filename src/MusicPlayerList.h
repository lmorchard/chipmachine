#ifndef MUSIC_PLAYER_LIST_H
#define MUSIC_PLAYER_LIST_H

#include "SongInfo.h"
#include "MusicPlayer.h"
#ifdef USE_REMOTELISTS
#include "RemoteLists.h"
#endif
#include "RemoteLoader.h"
#include "MusicDatabase.h"
#include "CueSheet.h"
//#include <webutils/webgetter.h>

#include <coreutils/thread.h>
#include <cstdint>
#include <deque>

#define SET_STATE(x) (LOGD("STATE: " #x), state = x)

namespace chipmachine {

class ChipMachine;

#ifdef _WIN32
#undef ERROR
#endif

class MusicPlayerList {
public:
	enum State {
		STOPPED,
		ERROR,
		WAITING,
		LOADING,
		STARTED,
		PLAY_STARTED,
		PLAYING,
		FADING,
		PLAY_NOW,
		PLAY_MULTI
	};

	MusicPlayerList(const std::string &workDir);

	~MusicPlayerList() {
		quitThread = true;
		playerThread.join();
	}

	bool addSong(const SongInfo &si, bool shuffle = false);
	void playSong(const SongInfo &si);
	void clearSongs();
	void nextSong();

	uint16_t *getSpectrum();
	int spectrumSize();
	SongInfo getInfo(int index = 0);
	SongInfo getDBInfo();
	int getLength();
	int getPosition();
	int listSize();

	bool playing() { return mp.playing(); }

	int getTune() {
		if(multiSongs.size())
			return multiSongNo;
		return mp.getTune();
	}

	void pause(bool dopause = true) {
		if(!(permissions & CAN_PAUSE))
			return;
		mp.pause(dopause);
	}
	bool isPaused() { return mp.isPaused(); }

	void seek(int song, int seconds = -1);

	std::string getMeta(const std::string &what) {
		if(what == "sub_title" && cueTitle != "")
			return cueTitle;
		if(what == "screenshot")
			return screenshot;
		return mp.getMeta(what);
	}

	State getState() {
		// LOCK_GUARD(plMutex);
		State rc = state;
		if(rc == PLAY_STARTED)
			SET_STATE(PLAYING);
		return rc;
	}

	bool getAllowed() {
		bool rc = wasAllowed;
		wasAllowed = true;
		return rc;
	}

	bool hasError() { return errors.size() > 0; }

	std::string getError() {
		LOCK_GUARD(plMutex);
		auto e = errors.front();
		errors.pop_front();
		return e;
	}

	enum {
		CAN_SWITCH_SONG = 1,
		CAN_SEEK = 2,
		CAN_PAUSE = 4,
		CAN_ADD_SONG = 8,
		CAN_CLEAR_SONGS = 16,
		PARTYMODE = 0x10000000
	};

	uint32_t getPermissions() { return permissions; }

	void setPermissions(uint32_t p) { permissions = p; }

	void setPartyMode(bool on, int lockSeconds = 60, int graceSec = 3);

	void setReportSongs(bool on) { reportSongs = on; }

	void setVolume(float volume) { mp.setVolume(volume); }

	float getVolume() const { return mp.getVolume(); }

	void stop() {
		SET_STATE(STOPPED);
		mp.stop();
	}

	bool wasFromQueue() const { return playedNext; }
	
	const std::vector<utils::File> &getSongFiles() const { return songFiles; }

private:
	bool handlePlaylist(const std::string &fileName);
	void playCurrent();
	bool playFile(const std::string &fileName);

	void update();
	void updateInfo();

	bool checkPermission(int flags);

	std::deque<std::string> errors;

	MusicPlayer mp;
	std::mutex plMutex;
	std::deque<SongInfo> playList;

	std::atomic<bool> wasAllowed;
	std::atomic<bool> quitThread;

	std::atomic<int> files;
	std::string loadedFile;

	std::atomic<State> state; // = STOPPED;
	SongInfo currentInfo;
	SongInfo dbInfo;

	std::thread playerThread;

	bool changedSong = false;

	bool reportSongs = true;

	std::atomic<uint32_t> permissions;

	bool partyMode = false;
	bool partyLockDown = false;
	int graceSeconds = 3;
	int lockSeconds = 60;

	bool detectSilence = true;

	std::shared_ptr<CueSheet> cueSheet;
	std::string cueTitle;

	int multiSongNo;
	std::vector<std::string> multiSongs;
	bool changedMulti = false;
	// RemoteLists &tracker;
	bool playedNext;
	
	std::vector<utils::File> songFiles;
	
	std::string screenshot;
	
};

} // namespace chipmachine

#endif // MUSIC_PLAYER_LIST_H
