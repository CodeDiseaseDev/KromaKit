#pragma once
#include <string>
#include <map>

// #include "Control.h"

enum class FontWeight {
	Thin = 100,
	ExtraLight = 200,
	Light = 300,
	Regular = 400,
	Medium = 500,
	SemiBold = 600,
	Bold = 700,
	ExtraBold = 800,
	Black = 900
};

enum class FontStyle {
	Normal,
	Italic
};

struct FontFaceKey {
	std::string family;
	FontWeight weight;
	FontStyle style;

	bool operator<(const FontFaceKey& other) const {
		if (family != other.family) return family < other.family;
		if (weight != other.weight) return weight < other.weight;
		return style < other.style;
	}
};

class DUIFont {
public:
	DUIFont(
		std::string family,
		float size = 15,
		FontWeight weight = FontWeight::Regular,
		FontStyle style = FontStyle::Normal);

	std::string Family = "default";
	float FontSize = 20;

	FontWeight Weight = FontWeight::Regular;
	FontStyle Style = FontStyle::Normal;

	bool operator==(const DUIFont& b) const {
		return Family == b.Family &&
			FontSize == b.FontSize &&
			Weight == b.Weight &&
			Style == b.Style;
	}

	bool operator!=(const DUIFont & font) const {
		return !(*this == font);
	}
};

class FontRegistry {
public:
	static void Register(
		const std::string& family,
		FontWeight weight,
		FontStyle style,
		const std::string& nanoVGName
	) {
		GetMap()[{ family, weight, style }] = nanoVGName;
	}

	static std::string Resolve(const DUIFont& font) {
		auto& map = GetMap();

		FontFaceKey exact {
			font.Family,
			font.Weight,
			font.Style
		};

		auto it = map.find(exact);
		if (it != map.end()) {
			return it->second;
		}

		FontFaceKey regular {
			font.Family,
			FontWeight::Regular,
			FontStyle::Normal
		};

		it = map.find(regular);
		if (it != map.end()) {
			return it->second;
		}

		return "default";
	}

private:
	static std::map<FontFaceKey, std::string>& GetMap() {
		static std::map<FontFaceKey, std::string> map;
		return map;
	}
};