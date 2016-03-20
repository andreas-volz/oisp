#ifndef DIRECTORYLIST_H
#define DIRECTORYLIST_H

#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <list>

/*! \brief Represents a subset of files in a directory hierarchy

To use this class:

-# instantiate it
-# set its parameters with setRootPath(), setRecursive(), setFileFilter() and setFileType()
-# access the list of matching files with getDirectoryList()

 */
class DirectoryList
{
public:
  DirectoryList();

  enum
  {
    NO_LIMIT = -1,   /*!< read directories recursive without limit */
    NO_RECURSIVE = 0 /*!< read only the base directory without recursion */
  };

  enum FileType
  {
    REGULAR_FILE, /*!< only regular files */
    DIRECTORY     /*!< only directories */
  };

  /// set a pattern of files to pay attention
  /*!
   *
   * \param file_filter Only files with those endings are paid attention. If a
   *                    dot is in the extension, it need to be included.
   *			(e.g. ".target")
   * \todo support regex style
   */
  void addFileFilter(const std::string &file_filter);

  void clearFileFilter();

  /// set the root path where to start file listing
  /*!
   * \param path the root path
   */
  void setRootPath(const std::string &path);

  /// set the recursive level
  /*!
   * \param recursive_level The number of maximim recursive directories to
   *			    search for files. Set to NO_LIMIT if the directory
   * 			    should be read without recursive limit. And set to
   *                        NO_RECURSIVE if only the files in the root
   *			    directory should be listed.
   */
  void setRecursive(int recursive_level);

  /*!
   * \return the complete list of all matching files
   */
  const std::list <std::string> &getDirectoryList();

  /// set the file types of interest (default: REGULAR_FILE)
  /*!
   * \param file_type the file type
   */
  void setFileType(FileType file_type);

  void setFullPath(bool full);

private:
  /// recursive function to read directory list
  void readDirRecursive(const std::string &dir_str);
  bool hasFileFilterEnding(const std::string &file) const;

  std::list <std::string> file_list;
  std::list <std::string> fileFilterList;
  std::string path;
  int recursive_level;
  int recursive_counter;
  int file_type;
  bool mFullPath;
};

#endif	// DIRECTORYLIST_H
