#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>
#include <glibmm.h>
#include <stdexcept>
#include "DirectoryList.h"
#include "glibmm.h"
#include "../common/util.h"

/* local */
#include "OICFMediaProviderImpl.h"
#include "Preferences.h"

using namespace std;

OICFMediaProviderImpl::OICFMediaProviderImpl(DBus::Connection &connection)
  : OICFMediaProvider(connection),
    m_MediaListenerProvider(NULL),
    m_player(NULL),
    m_playingTitleID(Line::InvalidID),
    mStartup(true)
{
  Preferences &preferences = Preferences::instance ();
  
  m_RootPath = preferences.getMusicRootFolder();
  m_CurrentPath = m_RootPath;
}

void OICFMediaProviderImpl::setPlayer(Player *p)
{
  m_player = p;
  m_player->signalUpdatePlayPosition.connect(sigc::mem_fun(this, &OICFMediaProviderImpl::updatePlayPositionWrap));
  m_player->signalNextTitle.connect(sigc::mem_fun(this, &OICFMediaProviderImpl::nextTitle));
}

void OICFMediaProviderImpl::getWindowList(const int32_t &start, const int32_t &end)
{
  m_Files.clear();
  m_Dirs.clear();
  mFilelist.clear();

  cout << "getWindowList()" << endl;

  DirectoryList dirList;
  dirList.setRootPath(m_CurrentPath);
  dirList.setRecursive(DirectoryList::NO_RECURSIVE);
  dirList.setFileType(DirectoryList::DIRECTORY);
  dirList.setFullPath(false);
  m_Dirs = dirList.getDirectoryList();
  for (list<string>::const_iterator dir_it = m_Dirs.begin();
       dir_it != m_Dirs.end();
       ++dir_it)
  {
    const string &dir = *dir_it;
    cout << "D: " << dir << endl;
  }

  DirectoryList dirList2;
  dirList2.setRootPath(m_CurrentPath);
  dirList2.setRecursive(DirectoryList::NO_RECURSIVE);
  dirList2.setFullPath(false);  
  dirList2.setFileType(DirectoryList::REGULAR_FILE);
  // TODO: add better generic support for this (e.g. regex)
  dirList2.addFileFilter("ogg");
  dirList2.addFileFilter("mp3");
  dirList2.addFileFilter("MP3");
  dirList2.addFileFilter("OGG");
  m_Files = dirList2.getDirectoryList();
  for (list<string>::const_iterator dir_it = m_Files.begin();
       dir_it != m_Files.end();
       ++dir_it)
  {
    const string &dir = *dir_it;
    cout << "F:" << dir << endl;
  }
  
  m_Dirs.sort();
  m_Files.sort();

  // insert 'Up'-Folder, but not in root level
  if (m_CurrentPathVector.size())
  {
    Line lineUp;
    lineUp.name = "Up";
    lineUp.type = Line::FolderUp;
    lineUp.id = Line::InvalidID;
    mFilelist.push_back(lineUp);
  }

  
  // copy raw data to folder/file containers
  unsigned int i = 0;
  for (list<string>::const_iterator dir_it = m_Dirs.begin();
       dir_it != m_Dirs.end();
       ++dir_it)
  {
    const string &dir = *dir_it;
    Line l;

    l.name = dir;
    l.type = Line::Folder;
    l.id = i;
    cout << "Dir: " << dir << endl;
    mFilelist.push_back(l);
    ++i;
  }

  Line playingTitle;

  //i = 0; // TODO: folders and files have one index or each one starts at 0?
  for (list<string>::const_iterator file_it = m_Files.begin();
       file_it != m_Files.end();
       ++file_it)
  {
    const string &file = *file_it;
    Line l;

    l.name = file;
    l.type = Line::Title;
    l.id = i;
    cout << "File: " << file << endl;
    if ((m_playingTitleID == i) && (m_PlayingPath == m_CurrentPath))
    {
      playingTitle = l;
    }

    if (mStartup)
    {
      // play first file at startup
      cout << "play at startup: " << l.name << endl;
      m_player->open(m_RootPath + "/" + file);
      m_player->play();
      m_playingTitleID = l.id;
      mStartup = false;
    }

    mFilelist.push_back(l);
    ++i;
  }

  m_MediaListenerProvider->getWindowListResult(mFilelist, 0, 100, 20);

  if ((playingTitle.type == Line::Title) && (playingTitle.id != Line::InvalidID))
  {
    m_MediaListenerProvider->updateSelectedTitle(playingTitle);
  }

  cout << "OICFMediaProviderImpl::getWindowList" << endl;
}

void OICFMediaProviderImpl::selectPath(const LineVector &path)
{
  cout << "OICFMediaProviderImpl::selectPath" << endl;

  // TODO: later use device as root
  m_CurrentPath = m_RootPath;

  m_CurrentPathVector = path;

  for (LineVector::const_iterator lv_it = path.begin();
       lv_it != path.end();
       ++lv_it)
  {
    const Line &l = *lv_it;

    // TODO: check if found is a really existing path!
    m_CurrentPath = m_CurrentPath + "/" + l.name;
  }

  // TODO: call empty path if path not found to jump to highest level
  m_MediaListenerProvider->updateSelectedPath(path);
}

void OICFMediaProviderImpl::changeDevice(const int32_t &device)
{

}

void OICFMediaProviderImpl::nextTitle()
{
  incrementTitle(1);
}

void OICFMediaProviderImpl::incrementTitle(const int32_t &num)
{
  cout << "OICFMediaProviderImpl::incrementTitle" << endl;

  if (m_playingTitleID >= 0)
  {
    const Line *lfound = NULL;

    for (LineVector::const_iterator vs_it = mFilelist.begin();
         vs_it != mFilelist.end();
         ++vs_it)
    {
      const Line &l = *vs_it;

      if (l.id == m_playingTitleID)
      {
        if (vs_it + num != mFilelist.end()) // select next file if available
        {
          lfound = &(*(vs_it + num));
        }
        break;
      }
    }

    if (lfound)
    {
      m_player->stop();
      cout << "Play: " << lfound->name << endl;
      m_player->open(m_CurrentPath + "/" + lfound->name);
      m_playingTitleID = lfound->id;
      m_player->play();
      m_MediaListenerProvider->updateSelectedTitle(*lfound);
    }
    else
    {
      cout << "File not found in media list!" << endl;
    }

    m_MediaListenerProvider->updateSelectedTitle(*lfound);
  }
}

void OICFMediaProviderImpl::decrementTitle(const int32_t &num)
{
  cerr << "decrementTitle not yet implemented!" << endl;
}

void OICFMediaProviderImpl::selectTitle(const Line &title)
{
  const Line *lfound = NULL;
  m_playingTitleID = Line::InvalidID;

  cout << "OICFMediaProviderImpl::selectTitle" << endl;

  for (LineVector::const_iterator vs_it = mFilelist.begin();
       vs_it != mFilelist.end();
       ++vs_it)
  {
    const Line &l = *vs_it;

    if ((l.id == title.id) && (l.type == title.type))
    {
      m_playingTitleID = title.id;
      lfound = &l;
      
      break;
    }
  }

  // TODO: check if found is a really existing file!

  if (lfound)
  {
    if(title.type == Line::Title)
    {
      const string playString(m_CurrentPath + "/" + lfound->name);
      cout << "Line::Title" << endl;

      cout << "Play: " << playString << endl;

      m_player->stop();
      m_player->open(playString);
      m_player->play();
      m_MediaListenerProvider->updateSelectedTitle(*lfound);
      //m_PlayingPath = m_CurrentPath;
    }
    else if(title.type == Line::Folder)
    {
      cout << "Line::Folder" << endl;

      m_CurrentPathVector.push_back(title);

      selectPath(m_CurrentPathVector);
      
      getWindowList(0, 100);
    }
    else if(title.type == Line::FolderUp)
    {
      m_CurrentPathVector.pop_back();

      selectPath(m_CurrentPathVector);
      
      getWindowList(0, 100);
      
      cout << "Line::FolderUp" << endl;
    }
  }
  else
  {
    cerr << "File not found in media list!" << endl;
  }
}

void OICFMediaProviderImpl::updatePlayPositionWrap(const int64_t &pos, const int64_t &duration)
{
  m_MediaListenerProvider->updatePlayPosition(pos, duration);
}

std::map< std::string, std::string > OICFMediaProviderImpl::Info()
{
  std::map< std::string, std::string > info;
  char hostname[HOST_NAME_MAX];

  gethostname(hostname, sizeof(hostname));
  info["hostname"] = hostname;
  info["username"] = getlogin();

  return info;
}
