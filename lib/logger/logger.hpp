#include <Arduino.h>

class Logger {
  public:
    Logger(const char* stage);
    void log(const char* format, ...);
    void error (const char* format, ...);

  private:
    const char* stageLogger;
};