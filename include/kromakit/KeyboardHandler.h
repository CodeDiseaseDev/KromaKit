// #pragma once
// #include <functional>
// // #include <Windows.h>
// #include <map>
// // #include <sys/types.h>


// struct SpecialCharacterKey {
// 	// a : character without shift key held
// 	// b : character WITH shift key held
// 	char a, b;
// };

// class KeyboardHandler
// {
// public:
// 	std::function<void(char)> OnCharacterAdd;
// 	std::function<void()> OnBackspace;

// 	static std::map<KeySym, SpecialCharacterKey> specialCharacters;

// 	bool isShiftHeld;
// 	bool isCapsLock;

// 	char OnKeyPressed(KeySym key);

// 	char OnKeyDown(KeySym key);
// 	void OnKeyUp(KeySym key);

// 	char ToSpecialCharacter(KeySym key, bool shiftKey);
// };

