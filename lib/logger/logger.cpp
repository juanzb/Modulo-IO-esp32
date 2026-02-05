#include <logger.hpp>

Logger::Logger(const char* stage) {
  stageLogger = stage;
}

void Logger::log(const char* format, ...) {
  char buffer[128];

  va_list args;
  va_start(args, format);
  vsniprintf(buffer, sizeof(buffer), format, args);
  Serial.printf("[", stageLogger, "]: ", buffer);
}

void Logger::error(const char* format, ...) {
    char buffer[128];

  va_list args;
  va_start(args, format);
  vsniprintf(buffer, sizeof(buffer), format, args);
  Serial.printf("[", stageLogger, "] ERROR: ", buffer);
}
