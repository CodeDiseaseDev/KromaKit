#pragma once

enum [[deprecated("Alignment.h is deprecated")]] XAlignment
{
  BeforeLeft, Left, XCenter, Right, AfterRight
};

enum [[deprecated("Alignment.h is deprecated")]] YAlignment
{
  BeforeTop,
  Top,
  YCenter,
  Bottom,
  AfterBottom
};

enum [[deprecated("Alignment.h is deprecated")]] Stretch
{
  NoStretch,
  Fill
};

struct [[deprecated("Alignment.h is deprecated")]] Alignment
{
  XAlignment x;
  YAlignment y;
};


struct [[deprecated("Alignment.h is deprecated")]] StretchMode
{
  Stretch
    x,
    y;
};
