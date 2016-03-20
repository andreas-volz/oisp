#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include "DirectoryList.h"
#include "util.h"

using namespace std;

DirectoryList::DirectoryList()
  : file_list(),
    path(),
    recursive_level(NO_LIMIT),
    recursive_counter(0),
    file_type(REGULAR_FILE),
    mFullPath(true)
{
}

void DirectoryList::addFileFilter(const string &file_filter)
{
  fileFilterList.push_back(file_filter);
}

void DirectoryList::clearFileFilter()
{
  fileFilterList.clear();
}

bool DirectoryList::hasFileFilterEnding(const string &file) const
{
  // if no file filter is set catch all files
  if(fileFilterList.size() > 0)
  {
    for (list<string>::const_iterator list_iter = fileFilterList.begin();
         list_iter != fileFilterList.end();
         list_iter++)
    {
      const string &ending(*list_iter);

      if (ending.length() > file.length())
        return false;

      return (file.find(ending, file.length() - ending.length()) != string::npos);
    }

    return false;
  }
  else
  {
    return true;
  }
}

void DirectoryList::setRecursive(int recursive_level)
{
  this->recursive_level = recursive_level;
}

void DirectoryList::setRootPath(const string &path)
{
  this->path = path;
}

#define kUndetermined 0
#define kDirectory    1
#define kRegularFile  2

inline static bool fileExists(const string &inFilename, unsigned char *outFlags)
{
  struct stat fileStat;

  *outFlags = kUndetermined;
  if (::stat(inFilename.c_str(), &fileStat) != 0)
    return false;

  *outFlags = (S_ISDIR(fileStat.st_mode)) ? kDirectory   :
              (S_ISREG(fileStat.st_mode)) ? kRegularFile : kUndetermined;

  return true; // file exists
}

void DirectoryList::readDirRecursive(const string &dir_str)
{
  DIR *dir;
  struct dirent *dir_entry;
  string dname;

  dir = ::opendir(dir_str.c_str());

  if (!dir)
  {
    return;
  }

  do
  {
    dir_entry = readdir(dir);

    if (dir_entry)
    {
      dname = dir_entry->d_name;
      
      string dir_file;
      dir_file = dir_str + "/" + dname;
            
      unsigned char flags;

      if ((dname.at(0) != '.') && fileExists(dir_file, &flags))
      {
        if (flags == kDirectory)
        {
          if (file_type == DIRECTORY)
          {
            if(mFullPath)
            {
              file_list.push_back(dir_file);
            }
            else
            {
              file_list.push_back(dname);
            }
          }

          if ((recursive_counter < recursive_level) ||
              (recursive_level == NO_LIMIT))
          {
            recursive_counter++;
            readDirRecursive(dir_file);
            recursive_counter--;
          }
        }
        else if (flags == kRegularFile)
        {
          if (file_type == REGULAR_FILE)
          {
            if (hasFileFilterEnding(dir_file))
            {
              if(mFullPath)
              {
                file_list.push_back(dir_file);
              }
              else
              {
                file_list.push_back(dname);
              }
            }
          }
        }
      }
    }
  }
  while (dir_entry);
  ::closedir(dir);
}

const list <string> &DirectoryList::getDirectoryList()
{
  readDirRecursive(path);
  recursive_counter = 0;

  return file_list;
}

void DirectoryList::setFileType(FileType file_type)
{
  this->file_type = file_type;
}

void DirectoryList::setFullPath(bool full)
{
  mFullPath = full;
}
