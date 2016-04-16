#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Player.h"
#include "MathUtil.h"

using namespace std;

Player::Player(Emotionxx::AudioObject *emotion) :
  mEmotion(emotion),
  mPlayState(STOPPED)
{
  mEmotion->getEventSignal("playback_finished")->connect(sigc::mem_fun(this, &Player::playBackFinished));
}

Player::~Player()
{

}

void Player::play(void)
{
  cout << "play" << endl;
  mPlayState = PLAYING;
  mEmotion->setPlay(true);
}

void Player::pause(void)
{
  cout << "pause" << endl;
  mPlayState = PAUSED;
  mEmotion->setPlay(false);
}

void Player::stop(void)
{
  cout << "stop" << endl;
  mPlayState = STOPPED;
  mEmotion->setPlay(false);
  // TODO: rewind?
}

void Player::rewind(void)
{

}

void Player::forward(void)
{

}

void Player::open(std::string uri)
{
  mEmotion->setFile(uri);
}

void Player::playBackFinished(Evasxx::Object &obj, void *event_info)
{
  cout << "Position: " << mEmotion->getPosition () << " state: " << mPlayState << endl;

  if(mPlayState == PLAYING)
  {
    signalNextTitle.emit();
  }
}
