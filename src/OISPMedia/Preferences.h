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

  void setMusicRootFolder(const std::string &folder);
  std::string getMusicRootFolder();

  void setEmotionEngine(const std::string &engine);
  std::string getEmotionEngine();

private:
  std::string mMusicRootFolder;
  std::string mEmotionEngine;
};

#endif // PREFERENCES_H
