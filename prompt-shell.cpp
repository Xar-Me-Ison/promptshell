#include <algorithm>

#include <direct.h>

#include <ctime>
#include <cstdlib>
#include <conio.h>

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>

#include <vector>
#include <unistd.h>  
#include <Windows.h>  

/* --------------------
PREPROCESSOR DIRECTIVES
----------------------- */
#ifdef _WIN32
#define OS_NAME "Windows"
#elif defined(__APPLE__)
#define OS_NAME "Mac OS"
#elif defined(__linux__)
#define OS_NAME "Linux"
#else
#define OS_NAME "Unknown"
#endif

/* -------------
GLOBAL VARIABLES 
---------------- */
std::string APPLICATION_VERSION = "[Version 0.8]";
std::string APPLICATION_DATE_VERSION = "2023.04";

std::string USER_INPUT = "";
std::string USER_LOGGED_DIRECTORY = "";
std::string USER_GUEST_DIRECTORY = "";

bool STATUS_EXIT_ON = false;
bool LINE_SEPERAT_ON = false;
bool TEXT_DELAY_ON = false;
bool ACCOUNT_CREATED_SUCCESSFULLY = false;


/* ---------------
USER-DEFINED TYPES 
------------------ */
enum TT_Input
{
	BACKSPACE = 8,
	RETURN = 32
};

/* ---------------
CLASS DECLARATIONS 
------------------ */
class promptShellUser
{
    /* Class methods */
public:
    promptShellUser(); // Default constructor, to sign up ..
    promptShellUser(int); // Overloaded constructor, to login ..
    ~promptShellUser();
private:
    // Invoked by the default constuctor (signing up) .. 
    void createUsername();
    void createPassword();
    void createUserData();
    bool isUsernameAvailable(std::string);

    std::string getPassword();

    // Invoked by the overloaded constructor (logging in) ..
    bool validateUser();
    void loggedIn();

	/* Class variables */
public:
private:
    std::string username, password, email;

    bool LOGGED_IN = false;
};

/* --------------------------
THREAD FUNCTIONS DECLARATIONS 
----------------------------- */
void promptShellIntroduction();
void promptShellLoginSignIn();

/* ------------------
FUNCTION DECLARATIONS 
--------------------- */
void printStatic(std::string text, int newline);
void printTypewriter(std::string text, int newline, int low_delay = 10, int high_delay = 20);

/* -------------------------
INLINE FUNCTION DECLARATIONS
---------------------------- */
inline void systemClear();

inline void lineSeparator();


int main()
{
    while (!STATUS_EXIT_ON)
    {
        promptShellIntroduction();
        promptShellLoginSignIn();
    }
    
}

/* --------------
CLASS DEFINITIONS 
----------------- */
promptShellUser::promptShellUser()
{
    createUsername();
    createPassword();

    if (ACCOUNT_CREATED_SUCCESSFULLY)
    {
        printTypewriter("\nSUCCESS: Your account has been created successfully, you can now log in.", 2, 20, 30);
        createUserData();
    }
}
promptShellUser::promptShellUser(int code) 
{
    std::cout << std::endl;

    if (validateUser())
    {
        printTypewriter("\nSUCCESS: You are now logged in as \033[1m" + username + "\033[0m.", 1, 30, 40);

        LOGGED_IN = true;
        loggedIn();
    }
    else
    {
        printTypewriter("\nERROR: Your credentials could not be verified.", 1, 30, 40);

        promptShellIntroduction();
        return;
    }
}
promptShellUser::~promptShellUser() 
{ 
    ACCOUNT_CREATED_SUCCESSFULLY = false;
}

void promptShellUser::createUsername()
{
    std::string input;

    printTypewriter("\nUsername: ", 0); getline(std::cin, input);

    while (input.size() > 60 || input.size() == 0 || !isUsernameAvailable(input))
    {
        printTypewriter("Username is taken, please try again.", 2);
        printTypewriter("Username: ", 0); getline(std::cin, input);
    }

    username = input;
}

void promptShellUser::createPassword() 
{ 
    std::string input1, input2;
    bool password_match;
    int password_counter = 0;

    do
    {

        printTypewriter("Password: ", 0); input1 = getPassword();
        while (input1.size() <= 1)
        {
            printTypewriter("Weak password, try again.", 2);
            printTypewriter("Password: ", 0); input1 = getPassword();
        }

        printTypewriter("Retype password: ", 0); input2 = getPassword();

        while (input2.size() <= 1)
        {
            printTypewriter("Weak password, try again.", 2);
            printTypewriter("Password: ", 0); input2 = getPassword();
        }

        if (input1 != input2)
        {
            printTypewriter("\nERROR: Password do not match.", 2);

            if (password_counter >= 2)
            {
                printTypewriter("\nERROR: Too many attempts have been made, try again.", 2);
                ACCOUNT_CREATED_SUCCESSFULLY = false;
                break;
            }
            password_match = false;
        }
        else
        {
            ACCOUNT_CREATED_SUCCESSFULLY = true;
            password_match = true;
            password = input1;
        }

        password_counter++;
    } while (!password_match);
    
}

void promptShellUser::createUserData()
{
    const char* path_userdata = ".userdata"; 
    const char* path_database = ".database";

    _mkdir(path_userdata);
    _mkdir(path_database);

    if (SetFileAttributes(path_userdata, FILE_ATTRIBUTE_HIDDEN) == 0) { std::cout << "Failed to hide directory" << std::endl; }
    if (SetFileAttributes(path_database, FILE_ATTRIBUTE_HIDDEN) == 0) { std::cout << "Failed to hide directory" << std::endl; }
 
    const char* fn_userdatabase = ".database/.usernames.txt";
    std::ofstream outputfile(fn_userdatabase, std::ios::app);

    // In the case of a file error ..
    if (!outputfile) { std::cout << "Error creating file" << std::endl; }
    outputfile << username << std::endl;
    outputfile.close();

    std::string fl_userdata = ".userdata/" + username + ".txt";
    const char* fn_userdata = fl_userdata.c_str();
    std::ofstream outfile(fn_userdata);

    // In the case of a file error ..
    if (!outfile) { std::cout << "Error creating file" << std::endl; }

    outfile << username << std::endl;
    outfile << password << std::endl;
    // outfile << email << std::endl;
    outfile.close();
}

bool promptShellUser::isUsernameAvailable(std::string usrname)
{
   	std::ifstream infile(".database/.usernames.txt");
	std::string search_string = usrname;
	std::string line;

	while (std::getline(infile, line))
	{
		// If it's not available return false ..
		if (line.find(search_string) != std::string::npos)
		{
			return false;
		}
	}

	return true;
}

std::string promptShellUser::getPassword()
{
	std::string password;	
	char input_ch;

	while ((input_ch = getch()) != '\r')
	{
		if (input_ch == '\b' && password.length() > 0)
		{
			password.pop_back();
			std::cout << "\b \b";
		}
		else if (input_ch != '\b')
		{
			password.push_back(input_ch);
			std::cout << '*';
		}
	}
	
	std::cout << std::endl;
	return password;
}

/* Invoked by the overloaded constructor */
bool promptShellUser::validateUser()
{
    std::ifstream username_infile(".database/.usernames.txt");
	std::fstream user_filename;
	std::string usrname, passwd;
	std::string usr_line, pwd_line;
	std::string data_line;
	std::string data;
	bool validated = false;
	int line_number = 1;
	
	
	// In case of a file error ..
	if (!username_infile) { return 0; }

	printTypewriter("Username: ", 0); getline(std::cin, usrname);
	
	while(std::getline(username_infile, usr_line))
	{
		if (usr_line.find(usrname) != std::string::npos || true)
		{
			printTypewriter("Password: ", 0); passwd = getPassword();
			// Here you will look for the corresponding user file and validate the password
			
			// Look in the corresponding userdata file ..
			user_filename.open(".userdata/" + usrname + ".txt");

			// In case of a file error ..
			if (!user_filename) { return 0; }

			while (std::getline(user_filename, pwd_line) && passwd.size() >= 1)
			{
				if (line_number == 2 && pwd_line.compare(passwd) == 0)
				{
					validated = true;
					user_filename.close();
					break;
				}	
				line_number++;
			}

			break;
		}
	}

	if (validated)
	{
		// Assign usernames and so on by reading from the file
		user_filename.open(".userdata/" + usrname + ".txt");
		line_number = 1;

		while (std::getline(user_filename, data_line))
		{
			switch (line_number)
			{
				case 1:
					username = data_line;
					break;
				case 2:
					password = data_line;
					break;
			}
			line_number++;
		}
	}

	username_infile.close();
	

	return validated; 
}

void promptShellUser::loggedIn()
{
    systemClear();
    lineSeparator();
    printTypewriter("PromptShell - Terminal Application " + APPLICATION_VERSION + "  \033[1m<" + username + ">\033[0m" , 1, 1, 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
    printTypewriter("Install the latest PromptShell for new features and improvements! https://bit.ly/3mvnEc2", 2, 1, 1);

    std::string input;
    std::string command;
    std::istringstream iss;
    std::vector<std::string> tokens;

	do 
	{
		// CL-interface for the logged in user..
		printTypewriter(USER_GUEST_DIRECTORY + "> ", 0); 

		// Getting the logged in user's input..
		getline(std::cin, input);

        iss.clear(); // Clear the state of istringstream ..
        iss.str(input); // Set the string to parse in istringstream ..

        tokens.clear(); // Clear the vector of tokens ..

        // Split input into tokens 
        tokens = {std::istream_iterator<std::string>{iss},
              std::istream_iterator<std::string>{}};

        // If the user's input is empty or whitespace, continue..
        if (tokens.empty())
            continue;

        command = tokens[0];

		if (command == "help")
		{
			printTypewriter("\nDELAY           Allow the user to turn the text delay ", 0, 5, 15); printTypewriter((!TEXT_DELAY_ON) ? "on." : "off.", 1, 5, 15);
            printTypewriter("LINE            Allow the user to turn the line seperator ", 0, 5, 15); printTypewriter((!LINE_SEPERAT_ON) ? "on." : "off.", 1, 5, 15);
            printTypewriter("CLEAR           Allow the user to clear up the terminnal.", 1, 5, 15);
			printTypewriter("LOGOUT          Allow the user to logout of their account.", 1, 5, 15);
            printTypewriter("VERSION         Allow the user to see the current running version.", 1, 5, 15);
            printTypewriter("TERMINATE       Allow the user to terminate the terminal application.", 2, 5, 15);
			
		}
		else if (command == "delay") 
		{
			TEXT_DELAY_ON = !TEXT_DELAY_ON;
			printTypewriter("Text delay has been turned ", 0); 
			printTypewriter((TEXT_DELAY_ON) ? "on." : "off.", 2);
		}
        else if (command == "line")
        {
            LINE_SEPERAT_ON = !LINE_SEPERAT_ON;
            printTypewriter("Line seperator has been turned ", 0); 
			printTypewriter((LINE_SEPERAT_ON) ? "on." : "off.", 2);
        }
		else if (command == "logout" || command == "signout")	
		{
			printTypewriter("\nSUCCESS: You have been logged out as \033[1m" + username + "\033[0m.", 1, 30, 40);
            promptShellIntroduction();
			return;
		}
		else if (command == "version" || command == "ver")
		{
            systemClear();
			lineSeparator();
			printTypewriter(APPLICATION_VERSION + " | " + APPLICATION_DATE_VERSION + " - Developed by Harrison L.  \033[1m<" + username + ">\033[0m", 1);
			printTypewriter("Copyright (C) GNB Corporation. All rights reserved.", 2);
		}
		else if (command == "cls" || command == "clear" || command == "clean")
		{
			system("cls");
		}
		else if (command == "terminate" || command == "term" || command == "exit" || command == "quit") 
		{
            exit(EXIT_SUCCESS);
		}
		else
		{

			printTypewriter("'" + input + "'" + " is not a recognized command.", 2); 
		}
	} while (LOGGED_IN);

}
/* -------------------------
THREAD FUNCTIONS DEFINITIONS 
---------------------------- */
void promptShellIntroduction()
{
    systemClear();
    lineSeparator();
    printTypewriter("PromptShell - Terminal Application " + APPLICATION_VERSION, 1, 1, 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
    printTypewriter("Install the latest PromptShell for new features and improvements! https://bit.ly/3mvnEc2", 2, 1, 1);
}

void promptShellLoginSignIn()
{
    // Required for the command parser ..
    std::string input;
    std::string command;
    std::istringstream iss;
    std::vector<std::string> tokens;


    promptShellUser *user_ptr;
    user_ptr = nullptr;


    do
    {
        // CL-interface for the default user..
        printTypewriter(USER_GUEST_DIRECTORY + "> ", 0);

        // Getting the user's input..
        std::getline(std::cin, input);

        iss.clear(); // Clear the state of istringstream ..
        iss.str(input); // Set the string to parse in istringstream ..

        tokens.clear(); // Clear the vector of tokens ..

        // Split input into tokens 
        tokens = {std::istream_iterator<std::string>{iss},
              std::istream_iterator<std::string>{}};

        // If the user's input is empty or whitespace, continue..
        if (tokens.empty())
            continue;

        command = tokens[0];

        // Transforming the user's input to lowercase for easier case matching..
		// std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (command == "help")
        {
            printTypewriter("\nDELAY           Allow the user to turn the text delay ", 0, 5, 15); printTypewriter((!TEXT_DELAY_ON) ? "on." : "off.", 1, 5, 15);
            printTypewriter("LINE            Allow the user to turn the line seperator ", 0, 5, 15); printTypewriter((!LINE_SEPERAT_ON) ? "on." : "off.", 1, 5, 15);
            printTypewriter("CLEAR           Allow the user to clear up the terminnal.", 1, 5, 15);
            printTypewriter("LOGIN           Allow the user to login onto their account.", 1, 5, 15);
            printTypewriter("SIGNUP          Allow the user to create their account.", 1, 5, 15);
            printTypewriter("VERSION         Allow the user to see the current running version.", 1, 5, 15);
            printTypewriter("TERMINATE       Allow the user to terminate the terminal application.", 2, 5, 15);
        }
        else if (command == "delay") 
		{
			TEXT_DELAY_ON = !TEXT_DELAY_ON;
			printTypewriter("Text delay has been turned ", 0); 
			printTypewriter((TEXT_DELAY_ON) ? "on." : "off.", 2);
        }
        else if (command == "line")
        {
            LINE_SEPERAT_ON = !LINE_SEPERAT_ON;
            printTypewriter("Line seperator has been turned ", 0); 
			printTypewriter((LINE_SEPERAT_ON) ? "on." : "off.", 2);
        }
        else if (command == "log" || command == "login")
        {
            user_ptr = new promptShellUser(69);

            if (user_ptr == nullptr)
            {
                printTypewriter("\nERROR: Unable to login at the moment.", 1);
                continue;
            }

            delete user_ptr; user_ptr = nullptr;
        }
        else if (command == "sign" || command == "signup")
        {
            if (user_ptr == nullptr)
            {
                //printTypewriter("SUCCESS", 1);
                user_ptr = new promptShellUser;
            }
            else
            {
                printTypewriter("ERROR: Unable to create an account at this time.", 1);
                continue;
            }
            
        }
        else if (command == "cls" || command == "clear")
        {
            systemClear();
        }
        else if (command == "version" || command == "ver")
        {
            systemClear();
            lineSeparator();
            printTypewriter(APPLICATION_VERSION + " | " + APPLICATION_DATE_VERSION + " - Developed by Harrison L.", 1);
            printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
        }
        else if (command == "terminate" || command == "term" || command == "exit" || input == "quit") 
        {
            STATUS_EXIT_ON = true;
            return;
        }
        else 
        {
            printTypewriter("'" + input + "'" + " is not a recognized command.", 2);
        }

    } while (true);
}

/* -----------------
FUNCTION DEFINITIONS  
-------------------- */
void printStatic(std::string text, int newline)
{
    std::cout << text;

    switch (newline)
    {
    case 1:
        std::cout << std::endl;
        break;
    case 2:
        std::cout << std::endl << std::endl;
        break;
    case 3:
        std::cout << std::endl << std::endl << std::endl;
        break;
    }
}
void printTypewriter(std::string text, int newline, int low_delay, int high_delay)
{
    srand(time(NULL));
    
    int rand_sleep(low_delay + (rand() % high_delay));

    if (TEXT_DELAY_ON == false)
    {
        printStatic(text, newline);
        return; 
    }

    for (std::string::iterator letter = text.begin(); letter != text.end(); letter++)
	{
		std::cout << *letter;
		Sleep(rand_sleep);
	}

    switch (newline)
    {
    case 1:
        std::cout << std::endl;
        break;
    case 2:
        std::cout << std::endl << std::endl;
        break;
    case 3:
        std::cout << std::endl << std::endl << std::endl;
        break;
    }
}

/* ------------------------
INLINE FUNCTION DEFINITIONS
--------------------------- */
inline void systemClear()
{
    std::string OS = OS_NAME;

    if (OS == "Windows")
        system("cls");
    else
        system("clear");
}

inline void lineSeparator()
{
    std::string LINE_SEPARATOR = 
"---------------------------------------------------------------------------------";

    if (LINE_SEPERAT_ON)
    {
        printTypewriter(LINE_SEPARATOR, 1, 1, 1);
    }
    else 
    {

    }
}