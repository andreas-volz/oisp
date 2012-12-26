#ifndef SIMULATION_H
#define SIMULATION_H

/* EFL */
#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>
#include <edjexx/Edjexx.h>

#include <OICFControl/ButtonEvent.h>
#include <OICFControl/RotaryEvent.h>
#include <OICFControl/AxisEvent.h>

class Simulation
{
public:
  Simulation();

  int exec(int argc, const char **argv);
  static void quit();  
    
private:
  void keyDownHandler (const Evasxx::KeyDownEvent &key);
  void keyUpHandler (const Evasxx::KeyUpEvent &key);
  void keyEdjeCaller (const std::string &key, const std::string &signal);

  void evasQuit(const Ecorexx::EvasWindow &win);

  void buttonHandler(const std::string emmision, const std::string source, enum ButtonEvent::Number number, enum ButtonEvent::Value value);
  void axisHandler(const std::string emmision, const std::string source, enum AxisEvent::Number number, enum AxisEvent::Value value);
  void rotaryHandler(const std::string emmision, const std::string source, enum RotaryEvent::Number number, enum RotaryEvent::Value value);
  
  Edjexx::Object *edje;
};

#endif // SIMULATION_H
