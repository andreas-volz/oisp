#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "OICFMediaProviderImpl.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>
#include <glibmm.h>
#include <stdexcept>
#include "DirectoryList.h"
#include "glibmm.h"
#include "util.h"

//#include <boost/filesystem.hpp>

using namespace std;
//using namespace boost::filesystem;

OICFMediaProviderImpl::OICFMediaProviderImpl(DBus::Connection &connection)
  : OICFMediaProvider(connection),
    m_MediaListenerProvider(NULL),
    m_RootPath("/home/andreas/Musik"),
    m_CurrentPath("/home/andreas/Musik"),
    m_player(NULL),
    m_playingTitleID(Line::InvalidID),
    mStartup(true)
{
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
  filelist.clear();

  cout << "getWindowList()" << endl;

  DirectoryList dirList;
  dirList.setRootPath(m_RootPath);
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
  dirList2.setRootPath(m_RootPath);
  dirList2.setRecursive(DirectoryList::NO_RECURSIVE);
  dirList2.setFullPath(false);  
  dirList2.setFileType(DirectoryList::REGULAR_FILE);
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
    filelist.push_back(lineUp);
  }

  
  // below: split complete list in folder and files list

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
    filelist.push_back(l);
    ++i;
  }

  Line playingTitle;

  i = 0;
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

    filelist.push_back(l);
    ++i;
  }

  m_MediaListenerProvider->getWindowListResult(filelist, 0, 100, 20);

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
    // TODO: handle with boost::path
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

    for (LineVector::const_iterator vs_it = filelist.begin();
         vs_it != filelist.end();
         ++vs_it)
    {
      const Line &l = *vs_it;

      if (l.id == m_playingTitleID)
      {
        if (vs_it + num != filelist.end()) // select next file if available
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
      m_player->open("file://" + m_CurrentPath + "/" + lfound->name);
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

  for (LineVector::const_iterator vs_it = filelist.begin();
       vs_it != filelist.end();
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

  m_player->stop();

  // TODO: check if found is a really existing file!

  if (lfound)
  {
    const string playString("file://" + m_CurrentPath + "/" + lfound->name);
    cout << "Play: " << playString << endl;
    m_player->open(playString);
    m_player->play();
    m_MediaListenerProvider->updateSelectedTitle(*lfound);
    m_PlayingPath = m_CurrentPath;
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

#if 0
  fs::path full_path(fs::initial_path<fs::path>());

  //m_CurrentPath -> TODO: handle it!

  full_path = fs::system_complete(fs::path(m_CurrentPath));

  unsigned long file_count = 0;
  unsigned long dir_count = 0;
  unsigned long other_count = 0;
  unsigned long err_count = 0;

  if (!fs::exists(full_path))
  {
    std::cout << "\nNot found: " << full_path.string() << std::endl;
    return;
  }

  if (fs::is_directory(full_path))
  {
    std::cout << "\nIn directory: "
              << full_path.string() << "\n\n";
    fs::directory_iterator end_iter;
    for (fs::directory_iterator dir_itr(full_path);
         dir_itr != end_iter;
         ++dir_itr)
    {
      try
      {
        if (fs::is_directory(dir_itr->status()))
        {
          std::cout << dir_itr->path() << " [directory]" << endl;
          string file = Glib::path_get_basename(dir_itr->path().string());

          m_Dirs.push_back(file);
          ++dir_count;
        }
        else if (fs::is_regular(dir_itr->status()))
        {
          std::cout << dir_itr->path() << " [title]" << endl;
          string file = Glib::path_get_basename(dir_itr->path().string());

          m_Files.push_back(file);

          ++file_count;
        }
        else
        {
          std::cout << dir_itr->path() << " [other]\n";
          string file = Glib::path_get_basename(dir_itr->path().string());
          ++other_count;
        }
      }
      catch (const std::exception &ex)
      {
        std::cout << dir_itr->path() << " " << ex.what() << std::endl;
        ++err_count;
      }
    }
    std::cout << "\n" << file_count << " files\n"
              << dir_count << " directories\n"
              << other_count << " others\n"
              << err_count << " errors\n";
  }
  else // must be a file
  {
    std::cout << "\nFound: " << full_path.string() << "\n";
  }
#endif