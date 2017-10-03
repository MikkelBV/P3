#include "EventHandler.h"
#include <iostream>

using namespace std;

EventHandler::EventHandler() {
	
}

EventHandler::~EventHandler(){}

void EventHandler::leftClick(int event, int x, int y, int flags, void* userdata) {
	cout << x << ", " << y << endl;
}