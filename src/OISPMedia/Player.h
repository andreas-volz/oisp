#ifndef PLAYER_H
#define PLAYER_H

/* sigc++ */
#include <sigc++/sigc++.h>

/* EFLxx */
#include <emotionxx/Emotionxx.h>

/* STD */
#include <cstdlib>
#include <stdint.h>

/* common */
#include "../common/Logger.h"

class Player
{
public:
  Player(Emotionxx::AudioObject *emotion);

  ~Player();

  sigc::signal <void, const int64_t &, const int64_t &> signalUpdatePlayPosition;
  sigc::signal <void> signalNextTitle;

  virtual void play();
  virtual void pause();
  virtual void stop();
  virtual void rewind();
  virtual void forward();
  virtual void open(std::string uri);

protected:


private:
  Logger mLogger;
  Emotionxx::AudioObject *mEmotion;

  void playBackFinished(Evasxx::Object &obj, void *event_info);
  void decodeStop(Evasxx::Object &obj, void *event_info);
  void positionUpdate(Evasxx::Object &obj, void *event_info);

  enum PlayState
  {
    PLAYING,
    STOPPED,
    PAUSED
  };

  PlayState mPlayState;
};

#endif /* PLAYER_H */
