#ifndef OICF_MEDIA_PROVIDER_IMPL_H
#define OICF_MEDIA_PROVIDER_IMPL_H

#include <dbus-c++/dbus.h>
#include <OICFMedia/OICFMediaProvider.h>
#include <OICFMedia/OICFMediaListenerProvider.h>
#include <vector>
#include "Player.h"

class OICFMediaProviderImpl : public OICFMediaProvider
{
public:
  OICFMediaProviderImpl(DBus::Connection &connection);

  void setOICFMediaListenerProvider(OICFMediaListenerProvider *mediaListenerProvider)
  {
    m_MediaListenerProvider = mediaListenerProvider;
  }
  void setPlayer(Player *p);

  std::map< std::string, std::string > Info();

  // OICF functions
  void getWindowList(const int32_t &start, const int32_t &end);
  void selectPath(const LineVector &path);
  void changeDevice(const int32_t &device);
  void incrementTitle(const int32_t &num);
  void decrementTitle(const int32_t &num);
  void selectTitle(const Line &title);

private:
  void updatePlayPositionWrap(const int64_t &pos, const int64_t &duration);
  void nextTitle();

  OICFMediaListenerProvider *m_MediaListenerProvider;
  std::list <std::string> m_Dirs;
  std::list <std::string> m_Files;
  std::string m_CurrentPath;
  std::string m_PlayingPath;
  LineVector m_CurrentPathVector;
  std::string m_RootPath;
  Player *m_player;
  int m_playingTitleID;
  LineVector mFilelist;
  bool mStartup;
};

#endif // OICF_MEDIA_PROVIDER_IMPL_H
