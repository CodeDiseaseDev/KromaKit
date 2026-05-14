//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_LOGGING_H
#define TEST_IOS_APP_LOGGING_H
#include <cstdarg>
#include <cstdio>


namespace Logging {
  struct SourceLoc {
    SourceLoc(
      const char* file = __builtin_FILE(),
      int line = __builtin_LINE(),
      const char* function = __builtin_FUNCTION()
    ) : file(file), line(line), function(function) {}

    const char* file;
    int line;
    const char* function;
  };

  struct LogMessage {
    LogMessage(
      const char* fmt,
      SourceLoc source = SourceLoc()
    ) : fmt(fmt), source(source) {}

    const char* fmt;
    SourceLoc source;
  };

  struct Condition {
    Condition(
      bool value,
      SourceLoc source = SourceLoc()
    ) : value(value), source(source) {}

    bool value;
    SourceLoc source;
  };

  class TerminalColor {
  public:
    static bool Enable();

    static const char* Reset();
    static const char* Dim();
    static const char* BrightBlue();
    static const char* BrightMagenta();
    static const char* BrightGreen();
    static const char* BrightYellow();
    static const char* BrightRed();
    static const char* BrightCyan();
    static const char* BrightBlack();

  private:
    inline static bool enabled_ = false;
  };

  inline void VLog(const char* fmt, va_list args) {
    std::vfprintf(stderr, fmt, args);
    fflush(stderr);
  }

  inline void Log(LogMessage msg, ...) {
    fprintf(
      stderr,
      "%s%s:%s%d: %sLog: %s",
      TerminalColor::BrightBlue(),
      msg.source.file,
      TerminalColor::BrightMagenta(),
      msg.source.line,
      TerminalColor::Dim(),
      TerminalColor::Reset()
    );

    va_list args;
    va_start(args, msg);
    VLog(msg.fmt, args);
    va_end(args);
  }

  inline void Assert(Condition cond, LogMessage msg, ...) {
    if (cond.value) {
      return;
    }

    fprintf(
      stderr,
      "%s:%d: Assert failed: ",
      cond.source.file,
      cond.source.line
    );

    va_list args;
    va_start(args, msg);
    VLog(msg.fmt, args);
    va_end(args);
  }
};



#endif //TEST_IOS_APP_LOGGING_H
