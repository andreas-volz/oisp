#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include "searchFile.h"
#include "../common/FileNotFoundException.h"

using namespace std;

const std::string searchPixmapFile(const std::string &pixmap)
{
  vector <string> name_vector;

#ifdef HAVE_CONFIG_H  
  name_vector.push_back(string(PACKAGE_PIXMAPS_DIR) + "/" + pixmap);
  name_vector.push_back(string(PACKAGE_SOURCE_DIR) + "/pixmaps/" + pixmap);
#endif

  name_vector.push_back("pixmaps/" + pixmap);

  const string &file = searchFile(name_vector);
  
  if (file.empty())
  {
    throw FileNotFoundException(pixmap);
  }
  
  return file;
}

const std::string searchDataFile(const std::string &data)
{
  vector <string> name_vector;

#ifdef HAVE_CONFIG_H  
  name_vector.push_back(string(PACKAGE_DATA_DIR) + "/" + data);
  name_vector.push_back(string(PACKAGE_SOURCE_DIR) + "/data/" + data);
#endif

  name_vector.push_back("data/" + data);

  const string &file = searchFile(name_vector);
  
  if (file.empty())
  {
    throw FileNotFoundException(data);
  }
  
  return file;
}

const std::string searchFile(std::vector <std::string> &name_vector)
{
  struct stat buf;

  for (unsigned int i = 0; i < name_vector.size(); i++)
  {
    string &try_name = name_vector[i];

    bool found = !(stat(try_name.c_str(), &buf));
    //cout << "try_name: " << try_name << endl;

    if (found)
    {
      return try_name;
    }
  }

  return "";
}
