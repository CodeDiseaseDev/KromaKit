//
// Created by code on 5/8/26.
//

#include <kromakit/Logging.h>

bool Logging::TerminalColor::Enable() {
  if (enabled_) return false;
  return enabled_ = true;
}

auto Logging::TerminalColor::Reset() -> const char * { return enabled_ ? "\033[0m" : ""; }
auto Logging::TerminalColor::Dim() -> const char * { return enabled_ ? "\033[2m" : ""; }
auto Logging::TerminalColor::BrightBlue() -> const char * { return enabled_ ? "\033[94m" : ""; }
auto Logging::TerminalColor::BrightMagenta() -> const char * { return enabled_ ? "\033[95m" : ""; }
auto Logging::TerminalColor::BrightGreen() -> const char * { return enabled_ ? "\033[92m" : ""; }
auto Logging::TerminalColor::BrightYellow() -> const char * { return enabled_ ? "\033[93m" : ""; }
auto Logging::TerminalColor::BrightRed() -> const char * { return enabled_ ? "\033[91m" : ""; }
auto Logging::TerminalColor::BrightCyan() -> const char * { return enabled_ ? "\033[96m" : ""; }
auto Logging::TerminalColor::BrightBlack() -> const char * { return enabled_ ? "\033[90m" : ""; }
