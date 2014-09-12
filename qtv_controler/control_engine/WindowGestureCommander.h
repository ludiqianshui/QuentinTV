#pragma once
class WindowGestureCommander 
{
private:
	static const int maxCommands = 20;
	int  commandCounter;
	char **gesture;
	char **window;
	char **command;

public:
	WindowGestureCommander(void);
	~WindowGestureCommander(void);

	void readConfiguration(char *filename);
	char *getWindow(char *gest);
	char *getCommand(char *gest);

};

