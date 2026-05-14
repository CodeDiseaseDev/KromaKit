//
// Created by code on 4/26/26.
//

#ifndef TEST_IOS_APP_DUISTYLENAME_H
#define TEST_IOS_APP_DUISTYLENAME_H



#pragma once

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

class DUIStyleName {
public:
	DUIStyleName();
	DUIStyleName(const char* name);
	DUIStyleName(const std::string& name);
	DUIStyleName(std::string_view name);

	static DUIStyleName FromParts(std::initializer_list<std::string> parts);

	bool Set(std::string_view name);

	const std::string& ToString() const;
	const char* c_str() const;

	bool Empty() const;
	int Count() const;

	const std::vector<std::string>& Parts() const;
	const std::string& operator[](int index) const;

	std::string Root() const;
	std::string Leaf() const;

	bool StartsWith(const DUIStyleName& prefix) const;
	bool StartsWith(std::string_view prefix) const;
	bool IsForControlType(std::string_view controlTypeName) const;

	DUIStyleName Parent() const;
	DUIStyleName Append(std::string_view part) const;

	bool operator==(const DUIStyleName& other) const;
	bool operator!=(const DUIStyleName& other) const;
	bool operator<(const DUIStyleName& other) const;

private:
	std::string raw;
	std::vector<std::string> parts;

	static std::string Normalize(std::string_view name);
	static bool IsValidPart(const std::string& part);

	void Rebuild();
};



// this is to allow usage of DUIStyleName inside of an `std::unordered_map`.
namespace std {
	template<>
	struct hash<DUIStyleName> {
		size_t operator()(const DUIStyleName& name) const noexcept {
			return std::hash<std::string>()(name.ToString());
		}
	};
}

#endif //TEST_IOS_APP_DUISTYLENAME_H
