#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <string>

class Preferences
{
public: // Singleton
  static Preferences& instance();

private: // Singleton
  Preferences() {}
  Preferences(const Preferences&);
  Preferences(Preferences&);
  virtual ~Preferences() {}
  void operator = (Preferences&);
  
public:
  void init();

  void setGPSHost(const std::string &host);
  std::string getGPSDHost();
  
  void setGPSDPort(int port);
  int getGPSDPort();

  void setDesktopLayer(bool desktop);
  bool getDesktopLayer();

  void setNaviMapFolder(const std::string &folder);
  std::string getNaviMapFolder(); 

private:
  std::string mGPSDHost;
  int mGPSDPort;
  bool mDesktopLayer;
  std::string mNaviMapFolder;
};

#endif // PREFERENCES_H
