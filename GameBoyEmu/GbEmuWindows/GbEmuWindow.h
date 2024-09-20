#pragma once


class GbEmuWindow
{
public:
	virtual void Init() {};
	virtual void ShowWindow() {};
	bool p_open = false;

	virtual void Update() {
		if (p_open) ShowWindow();
	};
};