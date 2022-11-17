#include <Arduino.h>

bool checkTimeoutOrLinkOff();
void parseCommand();
void executeCommand();
void echoPrompt();
void echoHelp();
bool isValidNumber(String str);