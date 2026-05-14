#include <kromakit/TestControl.h>

void TestControl::OnRender(Graphics* rendTarget)
{
	rendTarget->FillRoundedRectangle(
		0, 0, size.width, size.height,
		background, 6
	);
}

void TestControl::DoLayout(Graphics* renderTarget)
{
}

void TestControl::OnMouseDown()
{

}
