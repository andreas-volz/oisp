#ifndef SEARCHFILE_H
#define SEARCHFILE_H

#include <string>
#include <vector>

// public search functions
const std::string searchThemeFile(const std::string &theme);
const std::string searchPixmapFile(std::string pixmap);
const std::string searchNaviDir();
const std::string searchDataDir();
const std::string getHomeDir();

/** @return the Users Working directory where the load and save dialog should be
    point to. On Linux and Window this will be the same as getHomeDir() but
    on a OS X Box this will point to the Users Desktop because it is not common
    to store files which are supposed for further processing on the users home
    dir. */
const std::string getUserWorkDir();


// private
const std::string searchFile(std::vector <std::string> &name_vector);

#endif // SEARCHFILE_H
