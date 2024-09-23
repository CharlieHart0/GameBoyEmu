#pragma once


class GbEmuWindow
{
public:
	// this shouldnt be used? should call inherited class constructor. too scared to commit to removing this method atm.
	virtual void Init() {};
	virtual void ShowWindow() {};
	bool p_open = false;

	virtual void Update() {
		if (p_open) ShowWindow();
	};
};