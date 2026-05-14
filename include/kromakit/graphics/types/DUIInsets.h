//
// Created by code on 4/29/26.
//

#ifndef TEST_IOS_APP_DUIINSETS_H
#define TEST_IOS_APP_DUIINSETS_H

struct DUIRect;
struct DUIPoint;
struct DUIRect;
struct DUISize;

struct DUIInsets {
	float left, top, right, bottom;

	DUIInsets();

	explicit DUIInsets(float all);
	DUIInsets(float horizontal, float vertical);
	DUIInsets(float left, float top, float right, float bottom);

	static DUIInsets Zero();

	float Horizontal() const;
	float Vertical() const;

	bool IsZero() const;
	bool IsEmpty() const;

	DUIInsets operator+(const DUIInsets& other) const;
	DUIInsets operator-(const DUIInsets& other) const;
	DUIInsets& operator+=(const DUIInsets& other);
	DUIInsets& operator-=(const DUIInsets& other);

	bool operator==(const DUIInsets& other) const;
	bool operator!=(const DUIInsets& other) const;

	DUISize GetAddedSize() const;
	DUIRect InflateRect(const DUIRect& rect) const;
	DUIRect DeflateRect(const DUIRect& rect) const;

	template<typename... TInsets>
	static DUIInsets Add(const DUIInsets& first, const TInsets&... rest) {
		DUIInsets result = first;
		((result += rest), ...);
		return result;
	}


	static DUIInsets LeftInset(float x);
	static DUIInsets TopInset(float x);
	static DUIInsets RightInset(float x);
	static DUIInsets BottomInset(float x);
};



#endif //TEST_IOS_APP_DUIINSETS_H
