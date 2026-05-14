//
// Created by code on 4/26/26.
//

#include <kromakit/styling/DUIStyleName.h>

DUIStyleName::DUIStyleName() = default;

DUIStyleName::DUIStyleName(const char *name) {
	Set(name ? std::string_view(name) : std::string_view());
}

DUIStyleName::DUIStyleName(const std::string &name) {
	Set(name);
}

DUIStyleName::DUIStyleName(std::string_view name) {
	Set(name);
}

DUIStyleName DUIStyleName::FromParts(std::initializer_list<std::string> parts) {
	DUIStyleName result;
	result.parts.assign(parts.begin(), parts.end());
	result.Rebuild();
	return result;
}

bool DUIStyleName::Set(std::string_view name) {
	parts.clear();
	raw.clear();

	std::string normalized = Normalize(name);

	if (normalized.empty()) {
		return false;
	}

	std::stringstream ss(normalized);
	std::string part;

	while (std::getline(ss, part, '.')) {
		if (part.empty()) {
			continue;
		}

		if (!IsValidPart(part)) {
			parts.clear();
			raw.clear();
			return false;
		}

		parts.push_back(part);
	}

	Rebuild();
	return !raw.empty();
}

const std::string & DUIStyleName::ToString() const {
	return raw;
}

const char * DUIStyleName::c_str() const {
	return raw.c_str();
}

bool DUIStyleName::Empty() const {
	return raw.empty();
}

int DUIStyleName::Count() const {
	return static_cast<int>(parts.size());
}

const std::vector<std::string> & DUIStyleName::Parts() const {
	return parts;
}

const std::string & DUIStyleName::operator[](int index) const {
	return parts[index];
}

std::string DUIStyleName::Root() const {
	return parts.empty() ? "" : parts.front();
}

std::string DUIStyleName::Leaf() const {
	return parts.empty() ? "" : parts.back();
}

bool DUIStyleName::StartsWith(const DUIStyleName &prefix) const {
	if (prefix.parts.size() > parts.size()) {
		return false;
	}

	for (size_t i = 0; i < prefix.parts.size(); i++) {
		if (parts[i] != prefix.parts[i]) {
			return false;
		}
	}

	return true;
}

bool DUIStyleName::StartsWith(std::string_view prefix) const {
	return StartsWith(DUIStyleName(prefix));
}

bool DUIStyleName::IsForControlType(std::string_view controlTypeName) const {
	return !parts.empty() && parts.front() == controlTypeName;
}

DUIStyleName DUIStyleName::Parent() const {
	DUIStyleName parent;

	if (parts.size() <= 1) {
		return parent;
	}

	parent.parts.assign(parts.begin(), parts.end() - 1);
	parent.Rebuild();
	return parent;
}

DUIStyleName DUIStyleName::Append(std::string_view part) const {
	DUIStyleName result = *this;

	std::string p(part);

	if (!IsValidPart(p)) {
		return result;
	}

	result.parts.push_back(p);
	result.Rebuild();
	return result;
}

bool DUIStyleName::operator==(const DUIStyleName &other) const {
	return raw == other.raw;
}

bool DUIStyleName::operator!=(const DUIStyleName &other) const {
	return !(*this == other);
}

bool DUIStyleName::operator<(const DUIStyleName &other) const {
	return raw < other.raw;
}

std::string DUIStyleName::Normalize(std::string_view name) {
	std::string result;
	result.reserve(name.size());

	bool lastWasDot = false;

	for (char ch : name) {
		if (std::isspace(static_cast<unsigned char>(ch))) {
			continue;
		}

		if (ch == '.') {
			if (!result.empty() && !lastWasDot) {
				result.push_back('.');
				lastWasDot = true;
			}

			continue;
		}

		result.push_back(static_cast<char>(
			std::tolower(static_cast<unsigned char>(ch))
		));

		lastWasDot = false;
	}

	while (!result.empty() && result.back() == '.') {
		result.pop_back();
	}

	return result;
}

bool DUIStyleName::IsValidPart(const std::string &part) {
	if (part.empty()) {
		return false;
	}

	for (char ch : part) {
		const bool ok =
				std::isalnum(static_cast<unsigned char>(ch)) ||
				ch == '_' ||
				ch == '-';

		if (!ok) {
			return false;
		}
	}

	return true;
}

void DUIStyleName::Rebuild() {
	raw.clear();

	for (size_t i = 0; i < parts.size(); i++) {
		if (i > 0) {
			raw += ".";
		}

		raw += parts[i];
	}
}