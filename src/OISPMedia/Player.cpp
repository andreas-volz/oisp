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
  mEmotion->getEventSignal("position_update")->connect(sigc::mem_fun(this, &Player::positionUpdate));
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

void Player::positionUpdate(Evasxx::Object &obj, void *event_info)
{
  static int old_position;
  
  double position = mEmotion->getPosition();
  double duration = mEmotion->getPlayLength();

  int p_sec = (int) position % 60;
  int p_min = position / 60;
  int p_hour = position / 3600;
  
  int d_sec = (int) duration % 60;
  int d_min = duration / 60;
  int d_hour = duration / 3600;

  if(old_position != static_cast<int>(position))
  {
    LOG4CXX_INFO(mLogger, "duration = " << d_hour << "h " << d_min << "m " << d_sec << "s");
    LOG4CXX_INFO(mLogger, "position = " << p_hour << "h " << p_min << "m " << p_sec << "s");
    signalUpdatePlayPosition.emit(static_cast<int64_t>(position), static_cast<int64_t>(duration));
  }

  old_position = position;
}
