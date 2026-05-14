// #include "KeyboardHandler.h"
// #include <map>

// std::map<KeySym, SpecialCharacterKey>
//     KeyboardHandler::specialCharacters =
// {
//     { XK_space,       { ' '       } },
//     { XK_minus,       { '-', '_'  } },
//     { XK_equal,       { '=', '+'  } },
//     { XK_semicolon,   { ';', ':'  } },
//     { XK_slash,       { '/', '?'  } },
//     { XK_grave,       { '`', '~'  } },
//     { XK_bracketleft, { '[', '{'  } },
//     { XK_backslash,   { '\\','|'  } },
//     { XK_bracketright,{ ']', '}'  } },
//     { XK_apostrophe,  { '\'', '"' } },
//     { XK_comma,       { ',', '<'  } },
//     { XK_period,      { '.', '>'  } }
// };

// char KeyboardHandler::OnKeyPressed(KeySym key)
// {
//     char c = ToSpecialCharacter(key, isShiftHeld);

//     bool capitalLetter = isShiftHeld ^ isCapsLock; // XOR like real keyboards

//     // numbers
//     if (key >= XK_0 && key <= XK_9 && !isShiftHeld)
//     {
//         c = '0' + (key - XK_0);
//     }

//     // letters
//     if (key >= XK_a && key <= XK_z)
//     {
//         c = capitalLetter ? ('A' + (key - XK_a)) : ('a' + (key - XK_a));
//     }

//     if (OnCharacterAdd != nullptr && c != 0)
//         OnCharacterAdd(c);

//     return c;
// }

// char KeyboardHandler::OnKeyDown(KeySym key)
// {
//     if (key == XK_Shift_L || key == XK_Shift_R)
//         isShiftHeld = true;

//     if (key == XK_Caps_Lock)
//         isCapsLock = !isCapsLock;

//     if (key == XK_BackSpace)
//     {
//         if (OnBackspace != nullptr)
//             OnBackspace();
//     }

//     return OnKeyPressed(key);
// }

// void KeyboardHandler::OnKeyUp(KeySym key)
// {
//     if (key == XK_Shift_L || key == XK_Shift_R)
//         isShiftHeld = false;
// }

// char KeyboardHandler::ToSpecialCharacter(KeySym key, bool shiftKey)
// {
//     char c = 0;

//     // shifted number row
//     if (key >= XK_0 && key <= XK_9 && shiftKey)
//     {
//         char arr[10] = {
//             ')', '!', '@', '#', '$',
//             '%', '^', '&', '*', '('
//         };
//         c = arr[key - XK_0];
//     }

//     if (specialCharacters.contains(key))
//     {
//         c = specialCharacters[key].a;
//         if (shiftKey && specialCharacters[key].b != 0)
//         {
//             c = specialCharacters[key].b;
//         }
//     }

//     return c;
// }