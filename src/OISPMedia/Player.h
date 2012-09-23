#ifndef PLAYER_H
#define PLAYER_H

/* EFLxx */
#include <emotionxx/Emotionxx.h>

/* STD */
#include <cstdlib>
#include <stdint.h>

class Player
{
public:
  Player(Emotionxx::AudioObject *emotion);

  ~Player();

  sigc::signal <void, const int64_t &, const int64_t &> signalUpdatePlayPosition;
  sigc::signal <void> signalNextTitle;

  virtual void play(void);
  virtual void pause(void);
  virtual void stop(void);
  virtual void rewind(void);
  virtual void forward(void);
  virtual void open(std::string uri);

protected:


private:
  Emotionxx::AudioObject *mEmotion;

  void playBackFinished(Evasxx::Object &obj, void *event_info);
};

#endif /* PLAYER_H */


