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
  mLogger("oisp.Media.Player"),
  mPlayState(STOPPED)
{
  mEmotion->getEventSignal("playback_finished")->connect(sigc::mem_fun(this, &Player::playBackFinished));
  mEmotion->getEventSignal("decode_stop")->connect(sigc::mem_fun(this, &Player::decodeStop));
}

Player::~Player()
{

}

void Player::play()
{
  LOG4CXX_TRACE(mLogger,  "play");
  mPlayState = PLAYING;
  mEmotion->setPlay(true);
}

void Player::pause()
{
  LOG4CXX_TRACE(mLogger,  "pause");

  if(mPlayState == PAUSED)
  {
    play();
  }
  else if(mPlayState == PLAYING)
  {
    mPlayState = PAUSED;
    mEmotion->setPlay(false);
  }
}

void Player::stop()
{
  LOG4CXX_TRACE(mLogger,  "stop");
  mPlayState = STOPPED;
  mEmotion->setPlay(false);
  // TODO: rewind?
}

void Player::rewind()
{

}

void Player::forward()
{

}

void Player::open(std::string uri)
{
  mEmotion->setFile(uri);
}

void Player::playBackFinished(Evasxx::Object &obj, void *event_info)
{
  LOG4CXX_TRACE(mLogger,  "playBackFinished: " << mPlayState);
  
  if(mPlayState == PLAYING)
  {
    //signalNextTitle.emit();
  }
}

void Player::decodeStop(Evasxx::Object &obj, void *event_info)
{
  LOG4CXX_TRACE(mLogger,  "decodeStop: " << mPlayState);
  
  if(mPlayState == PLAYING)
  {
    signalNextTitle.emit();
  }
}
