//
// Created by code on 4/29/26.
//

#ifndef TEST_IOS_APP_DUIRECT_H
#define TEST_IOS_APP_DUIRECT_H


struct DUIRect;
struct DUIPoint;
struct DUIInsets;
struct DUISize;


struct DUIRect {
	float x, y;
	float width, height;

	DUIRect();
	DUIRect(DUIPoint location, DUISize size);
	DUIRect(float all);
	DUIRect(float x, float y, float width, float height);

	// Move rect by a point
	DUIRect operator+(const DUIPoint& p) const;
	DUIRect operator-(const DUIPoint& p) const;
	DUIRect& operator+=(const DUIPoint& p);

	// Resize
	DUIRect operator+(const DUISize& s) const;
	DUIRect operator-(const DUISize& s) const;

	DUIRect Inset(const DUIInsets& insets) const;

	float Right() const;
	float Bottom() const;
	bool IsEmpty() const;

	// Containment check
	bool Contains(const DUIPoint& p) const;
	bool ContainsRect(const DUIRect& rect) const;

	// Intersection
	bool Intersects(const DUIRect& other) const;

	DUIRect IntersectRects(const DUIRect& b);
};

#endif //TEST_IOS_APP_DUIRECT_H
