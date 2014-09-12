#include "StdAfx.h"
#include "WindowGestureCommander.h"
#include <stdio.h>
#include <fstream>

using namespace std;

WindowGestureCommander::WindowGestureCommander(void)
{
	commandCounter=0;
	gesture = new char*[maxCommands];
	window  = new char*[maxCommands];
	command = new char*[maxCommands];
}


WindowGestureCommander::~WindowGestureCommander(void)
{
}



void WindowGestureCommander::readConfiguration(char *filename)
{
	char tmp[255];
	ifstream myInputFile;
	myInputFile.open(filename);
	
	if (myInputFile) {
		while (!myInputFile.eof()) {
			myInputFile>>tmp;
			gesture[commandCounter] = new char[strlen(tmp)+1];
			strcpy(gesture[commandCounter], tmp);
			myInputFile>>tmp;
			window[commandCounter] = new char[strlen(tmp)+1];
			strcpy(window[commandCounter], tmp);
			myInputFile>>tmp;
			command[commandCounter] = new char[strlen(tmp)+1];
			strcpy(command[commandCounter], tmp);
			commandCounter++;
		}
		myInputFile.close();
	} else {
		// output an error!!
	}
}

char *WindowGestureCommander::getWindow(char *gest)
{
	char *result = NULL;
	int found = (0==1);
	int counter = 0;
	while ((!found)&&(counter<commandCounter)) {
		if (strcmp(gest, gesture[counter])==0) {
			result = new char[strlen(window[counter])+1];
			strcpy(result, window[counter]);
			found = (0==0);
		}
		counter++;
	}
	return result;
}
char *WindowGestureCommander::getCommand(char *gest)
{
	char *result = NULL;
	int found = (0==1);
	int counter = 0;
	while ((!found)&&(counter<commandCounter)) {
		if (strcmp(gest, gesture[counter])==0) {
			result = new char[strlen(command[counter])+1];
			strcpy(result, command[counter]);
			found = (0==0);
		}
		counter++;
	}
	return result;
}