#include <iostream>
#include <sstream>
#include <iomanip>
#include "Player.h"
#include "MathUtil.h"

using namespace std;

Player::Player(/*Emotionxx::AudioObject *emotion*/) //:
  //mEmotion(emotion)
{
  //mEmotion->getEventSignal("playback_finished")->connect(sigc::mem_fun(this, &Player::playBackFinished));
}

Player::~Player()
{

}

void Player::play(void)
{
  //mEmotion->setPlay(true);
}

void Player::pause(void)
{
  //mEmotion->setPlay(false);
}

void Player::stop(void)
{
  //mEmotion->setPlay(false);
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
  //mEmotion->setFile(uri);
}

void Player::playBackFinished(/*Evasxx::Object &obj, void *event_info*/)
{
  //cout << "Position: " << mEmotion->getPosition () << endl;
  signalNextTitle.emit();
}
