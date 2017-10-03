#pragma once
class EventHandler
{
public:
	EventHandler();
	~EventHandler();
	void leftClick(int event, int x, int y, int flags, void* userdata);
};
