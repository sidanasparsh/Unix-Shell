/**UNIX SHELL & HISTORY FEATURES
**/

#include <unistd.h>   //Needed to define pid
#include <stdlib.h>   //Needed to define exit()
#include <iostream>   //Needed for console output and input
#include <string>     //Stores the user input as string
#include <vector>     //Needed to maintain history
#include <sstream>    //Helper- helps filter the user input
#include <map>        //Using a colormap so as to change color of PS1
#include <sys/wait.h> //Needed to implement wait() for parent process

using namespace std;

#define MAX_INPUT_SIZE  80    //Maximum command length is taken to be 80
#define HISTORY_SIZE   5      //History stored in our project
map<string, string> COLORMAP; //Maps the color name to color value
string args[MAX_INPUT_SIZE];  //User input
string PS1 = "team22shell>>"; //Default PS1
string CLR = "GRN";           //Default PS1 Color

/**
**Displays the PS1 --which is by-default team22shell>>
*/
void displayPS1();

/**
**Gets the user input, parses it and stores the commands in a vector.
** @return the command vector described above
** @param background - Boolean value which sets true if the user inputs ampersand (&) in the command
** @param line - User input taken as a string
*/
vector<string> getArgs(bool& background, string line);

/**
**Appends the user's command into the history
**@param line - User input which is to be pushed into the history
**@param history - History vector which stores a maximum of HISTORY_SIZE values
*/
void appendCmdToHistory(string& line, vector<string>& history);


/**
** Executes the user's command by spawning a new process
** If user enters "&" in the command line - the process runs as a daemon
** Else parent process waits for the child to terminate
** @param args - User's input stored as a vector of commands
** @param background - Boolean value which sets true if the user inputs ampersand (&) in the command
*/
void execute(vector<string> args, bool background);

/**
** Handles the size of history to be Maximum HISTORY_SIZE
** Deletes the oldest history in case the history increases the HISTORY_SIZE
** @return the history element asked by the user
** @param history - the history vector which stores previous user commands
** @param cmd - The current user command asking for a history element
*/
string handleHistory(vector<string>& history, string cmd);

/**
** Maps the color name to color value
** @param  color - the new color name
** @param line - the text whose color we want to change
*/
void printColor(string color, string line);

/**
** Handles the user input which includes variable assignment
** @param str - User input which includes the variable assignment
** @return - returns -1 if the request couldn't be handled successfully
** @return - returns 0 if there was nothing to handled
** @return - returns 1 if everything got handled successfully
*/
int handleVarAssignment(string str);


/**
*
*Main
*
*/
int main(){
    bool background = false;
    vector<string> commandList; //Vector which stores the users commands as a list
    int cmdCount = 0;           //Gives the number of user commands executed
    string line;                //User's input taken as a string
    vector<string> history;     //History vector which stores the values


    while(1){                   //Listens for user input
        displayPS1();
        getline(cin, line);     //Get a array of args
        commandList = getArgs(background, line);
        if(commandList.size() == 0)
        {
            continue;           //If the user doesn't input any command -->reprompt for user input by showing PS1

        }
        if (commandList[0][0] == '!'){ //User is asking for history
            line = handleHistory(history, line);
            if(line== "")
            {
                continue;
            }
            commandList = getArgs(background, line);
        }
        if (handleVarAssignment(commandList[0]) == 1)
            continue;
        if (commandList[0] == "exit"){ //User demands to end the shell
            printColor("GRN", "Exiting shell...\n");
            exit(0);
        }
        else if (commandList[0] == "history")
        {
            appendCmdToHistory(line, history);
            for(int i=0; i< history.size(); i++)
                cout<<cmdCount-history.size()+i+1<<": "<< history[i]<<endl;
        }
        else
            execute(commandList, background); //Executes the pre-defined user commands
        if(commandList[0] != "history")
        {
            appendCmdToHistory(line, history);
        }
        cmdCount++;
    }
}


/**
**Appends the user's command into the history
**@param line - User input which is to be pushed into the history
**@param history - History vector which stores a maximum of HISTORY_SIZE values
*/
void appendCmdToHistory(string& line, vector<string>& history)
{
    if(line[0] == '!' )
    {
        return;
    }
    if(history.size() > HISTORY_SIZE-1)
    {
        history.erase(history.begin());
    }
    history.push_back(line);
}


/**
** Executes the user's command by spawning a new process
** If user enters "&" in the command line - the process runs as a daemon
** Else parent process waits for the child to terminate
** @param args - User's input stored as a vector of commands
** @param background - Boolean value which sets true if the user inputs ampersand (&) in the command
*/
void execute(vector<string> args, bool background){

    int size=args.size();
    char** args1=new char*[size];
    int i,j;
    for(i=0; i<size;i++){
        args1[i] = new char[args[i].size() + 1];
        for(j=0;j<args[i].size();j++){
            args1[i][j]=args[i][j];
        }
        args1[i][j]='\0';
    }

    int status=0;
    pid_t pid;
    pid=fork();
    if(pid<0){ //Error in executing fork
        cout<<"Cannot fork the process\n";
    }

    if (pid==0){ //Child process
        status=execvp(args1[0],args1);
        if(status<0){
            cout<<"Failed to execute the command \n";
            exit(0);
        }
    }
    else{ //Parent process
        if(!background){
            wait(NULL);
        }

    }
}

/**
**Displays the PS1 --which is by-default team22shell>>
*/
void displayPS1(){
    printColor(CLR, PS1);
}

/**
**Gets the user input, parses it and stores the commands in a vector.
** @return the command vector described above
** @param background - Boolean value which sets true if the user inputs ampersand (&) in the command
** @param line - User input taken as a string
*/
vector<string> getArgs(bool& background, string line){
    vector<string> commandList;
    istringstream stream(line);
    string str="";
    while(stream>>str)
    {
       commandList.push_back(str);
    }


    for(int i = 0; i< commandList.size();i++)
    {
        if (commandList[i] == "&")
        {
            background = true;
            commandList.pop_back();
        }
        if (commandList[i][commandList[i].length() -1] == '&')
        {
            background = true;
            commandList[i] = commandList[i].substr(0,commandList[i].length() - 1);
        }

    }
    return commandList;

}

/**
** Handles the size of history to be Maximum HISTORY_SIZE
** Deletes the oldest history in case the history increases the HISTORY_SIZE
** @return the history element asked by the user
** @param history - the history vector which stores previous user commands
** @param cmd - The current user command asking for a history element
*/
string handleHistory(vector<string>& history, string cmd)
{
    if(cmd[1] == '!')
        {
            return history[history.size()-1];
        }
        else
        {
            int history_value = (int)(cmd[1] - '0');
             if (history_value>=history.size()){
            	printColor("RED", "No such commands in history \n");
            	return "";
            }
            else
            {
                return history[history.size()-history_value-1];
            }
        }

}

/**
** Maps the color name to color value
** @param  color - the new color name
** @param line - the text whose color we want to change
*/
void printColor(string color, string line)
{

        COLORMAP["RED"]="\x1B[01;31m";
        COLORMAP["GRN"]="\x1B[01;32m";
        COLORMAP["YEL"]="\x1B[01;33m";
        COLORMAP["BLU"]="\x1B[01;34m";
        COLORMAP["MAG"]="\x1B[01;35m";
        COLORMAP["CYN"]="\x1B[01;36m";
        COLORMAP["WHT"]="\x1B[01;37m";
        COLORMAP["RESET"]="\x1B[0m";
        cout<<COLORMAP[color]<<line<<COLORMAP["RESET"];
}

/**
** Handles the user input which includes variable assignment
** @param str - User input which includes the variable assignment
** @return - returns -1 if the request couldn't be handled successfully
** @return - returns 0 if there was nothing to handled
** @return - returns 1 if everything got handled successfully
*/
int handleVarAssignment(string str){
    string str1, str2;
    int pos=0,i=0;
    if(str.find('=') != -1){
        pos= str.find('=');
        str1= str.substr(0,pos);
    }
    else
    {
        return 0;
    }
    if(str1=="PS1"){
        str2=str.substr(pos+1);
        PS1=str2;
        cout <<"PS1 set to "<<PS1<<endl;
    }
    else if(str1=="CLR"){
        str2=str.substr(pos+1);
        CLR = str2;
    }
    else {
        printColor("RED", "LHS not PS1 or color\n");
        return -1;
    }
    return 1;
}

/**
**************END*************
*/
