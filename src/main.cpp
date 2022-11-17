//zoomkat 12-18-10
//routerbot code
//for use with IDE 0021
//open serial monitor to see what the arduino receives
// 
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "define.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address
byte ip[] = { 192, 168, 200, 135 }; // ip in lan
byte gateway[] = { 192, 168, 200, 1 }; // internet access via router
byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(23); //server port
EthernetClient client;

String command = "";
String serialResponse = "";
bool firstConnection = true;
bool commandTerminated = false;
bool deviceCommand = true;
String error = "";

/* not used yet **************
// todo: implement ethernet cable deconnexion
bool timeoutConection = false;
int timeout=0;
#define TIMEOUT_CONNEXION 5 * 1000 //timout 5 seconds
/*****************************/
 
 

bool isValidNumber(String str){
    for(byte i=0; i < str.length(); i++){
        if(!isDigit(str.charAt(i))){
            return false;
        }
    }
    return true;
}

bool checkTimeoutOrLinkOff(){
    if(Ethernet.linkStatus() == LinkOFF)
        return false;
    return true;
}

void echoHelp(){
    if(client.connected()){
        client.println("PURE_LINK_UHDS telnet server");
        client.println("Available commands: ");
        client.println("AUDIO EDID SWITCH:");
        client.println("\ta1(!)      -> \tAudio auto EDID");
        client.println("\ta2(!)      -> \tStereo Audio 2.0");
        client.println("\ta3(!)      -> \tDolby/DTS 5.1");
        client.println("\ta4(!)      -> \tHD Audio 7.1");
        client.println("AUDIO RETURN CHANNEL (ARC):");
        client.println("\tonarc(!)   -> \tARC on");
        client.println("\toffarc(!)  -> \tARC off");
        client.println("INPUT AUTO SWITCHING:");
        client.println("\tonauto(!)  -> \tAudio auto EDID");
        client.println("\toffauto(!) -> \tAudio auto EDID");
        client.println("INPUT SWITCHING COMMAND:");
        client.println("\tin1(!)      -> \tInput HDMI 1");
        client.println("\tin2(!)      -> \tInput HDMI 2");
        client.println("\tin3(!)      -> \tInput HDMI 3");
        client.println("\tin4(!)      -> \tInput HDMI 4");
        client.println("--INFOS--\tCommands are not case sensitive");
        client.println("--INFOS--\t\"!\" break character not required");
    }
}

void echoPrompt(){
    client.print("PURE_LINK_UHDS TERMINAL $: ");
}

void executeCommand(){
    command.toUpperCase();
    // Commands not related to UHDS********************************
    if(command.equals("CL") || command.equals("QUIT")){
        client.println("BYE...");
        client.stop();
        firstConnection = true;
        deviceCommand = false;
    }
    if(command.equals("?") || command.equalsIgnoreCase("help")){
        echoHelp();
        echoPrompt();
        deviceCommand = false;
        return;
    }
    //**************************************************************
    if(!command.endsWith("!"))
        command += "!";
    //check if video or audio input is supported
    if( command.startsWith("IN") != NULL || command.startsWith("A") != NULL && command.endsWith("!")){
        String inpStr = command.substring(1);
        if (command.startsWith("IN") != NULL)
            inpStr = inpStr.substring(1);
        inpStr.remove(inpStr.length()-1);
        int input = -1;
        if(isValidNumber(inpStr))
            input = inpStr.toInt();
        if(input < 1 || input > 4){
            error += "invalid input " + inpStr + " in command " + command;
            return;
        }  
    }
        // Send the serial command to UHDS
        Serial.println(command);
}

void parseCommand(){
    executeCommand();
    //device is slow
    delay(500);
    if(error.length() == 0){    
        if(Serial.available()){
            while(Serial.available()){
                char charResp = Serial.read();
                serialResponse += charResp;
            }
        }else{
            error += " Serial is not Available check Rx/TX connection and reboot this device";
            serialResponse = "Serial not available";
        }
    }
    else{
        serialResponse = "ERROR";
    }
    if(client.connected() && deviceCommand){
        serialResponse.trim();
        client.print("{\"command\" : \"");
        client.print(command);
        client.print("\", \"response\" : \"");
        client.print(serialResponse);
        if (error.length() > 0){
            client.print("\", \"error\" :\"");
            client.print(error);
        }
        client.println("\"}");
        echoPrompt();
    }
    //clearing string for next read
    serialResponse = ""; 
    command = "";
    error = "";
    deviceCommand = true;
}



void setup(){
//enable serial data print 
Serial.begin(19200);
delay(100);
Serial.println("Serial ready ...");
Ethernet.begin(mac, ip, gateway, subnet);
while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    delay(100);
}
//start Ethernet
server.begin();
Serial.println("server ready ...");

}

void loop() {

        // Create a client connection
    client = server.available();
    if (client) {
        client.flush();
        if(firstConnection){
            server.write('\n');
            while(client.available()){
                client.read();
            }
            firstConnection = false;
            echoPrompt();
        }else{
            while (client.connected()) {
                if (client.available()) {
                    char in = (char) client.read();
                    
                    //read char by char HTTP request
                    if (command.length() < 63) {
                        //store characters to string 
                        if (in == '\n'){
                            // in case of /r
                            commandTerminated = true;
                        }
                        else{
                            if(in != '\r'){
                                command += in;
                            }
                        }
                    }
                    if (commandTerminated) {
                    delay(1);
                    commandTerminated = false;
                    parseCommand();
                    }
                }
            }
        }
    }



}