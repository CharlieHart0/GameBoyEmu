#pragma once


class GbEmuWindow
{
public:
	virtual void CPUInspector() {};
	virtual void ShowWindow() {};
	bool p_open = false;

	virtual void Update() {
		if (p_open) ShowWindow();
	};
};