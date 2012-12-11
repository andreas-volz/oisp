#ifndef SEARCHFILE_H
#define SEARCHFILE_H

#include <string>
#include <vector>

// public search functions
const std::string searchPixmapFile(const std::string &pixmap);
const std::string searchDataFile(const std::string &data);

// private
const std::string searchFile(std::vector <std::string> &name_vector);

#endif // SEARCHFILE_H
