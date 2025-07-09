// ============================================================================
// main.cpp
// ============================================================================

#include "gtf2bed.hpp"
#include <iostream>

/**
 * @brief Main entry point for GTF2BED application
 * 
 * This function serves as the main entry point for the GTF2BED tool. It performs
 * the following initialization tasks:
 * 1. Displays the application banner and version information
 * 2. Processes command line arguments for logging and silent mode
 * 3. Validates minimum argument requirements
 * 4. Calls the main conversion function
 * 
 * The function handles basic command line parsing for global options like
 * --log and --silent before passing control to the main conversion routine.
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return 0 on success, exits with error code on failure
 */
int main(int argc, char** argv) {
    
    // Display application banner with ASCII art
    std::cout << "\x1B[35;1m" << R"(

  ▄████ ▄▄▄█████▓  █████▒   ▄▄▄█████▓ ▒█████      ▄▄▄▄   ▓█████ ▓█████▄ 
 ██▒ ▀█▒▓  ██▒ ▓▒▓██   ▒    ▓  ██▒ ▓▒▒██▒  ██▒   ▓█████▄ ▓█   ▀ ▒██▀ ██▌
▒██░▄▄▄░▒ ▓██░ ▒░▒████ ░    ▒ ▓██░ ▒░▒██░  ██▒   ▒██▒ ▄██▒███   ░██   █▌
░▓█  ██▓░ ▓██▓ ░ ░▓█▒  ░    ░ ▓██▓ ░ ▒██   ██░   ▒██░█▀  ▒▓█  ▄ ░▓█▄   ▌
░▒▓███▀▒  ▒██▒ ░ ░▒█░         ▒██▒ ░ ░ ████▓▒░   ░▓█  ▀█▓░▒████▒░▒████▓ 
 ░▒   ▒   ▒ ░░    ▒ ░         ▒ ░░   ░ ▒░▒░▒░    ░▒▓███▀▒░░ ▒░ ░ ▒▒▓  ▒ 
  ░   ░     ░     ░             ░      ░ ▒ ▒░    ▒░▒   ░  ░ ░  ░ ░ ▒  ▒ 
░ ░   ░   ░       ░ ░         ░      ░ ░ ░ ▒      ░    ░    ░    ░ ░  ░ 
      ░                                  ░ ░      ░         ░  ░   ░    
                                                       ░         ░      

)" << "\033[0m" << std::endl;

    // Display application information
    std::cout << "\n" << "\x1B[35;1m" << "GTF2BED" << "\033[0m" << std::endl; 
    std::cout << " * Authors : Nikolaos M.R. LYKOSKOUFIS" << std::endl;
    std::cout << " * Contact : nikolaos.lykoskoufis@gmail.com" << std::endl;
    std::cout << " * Version : version 0.1" << std::endl;

    // Process global options (log file and silent mode)
    // These are processed before the main option parsing to set up logging
    for (int a = 1; a < argc - 1; a++) {
        if ((strcmp(argv[a], "--log") == 0) && !vrb.open_log(string(argv[a+1]))) {
            vrb.error("Impossible to open log file!");
        }
        if (strcmp(argv[a], "--silent") == 0) {
            vrb.set_silent();
        }
    }

    // Convert command line arguments to vector format
    std::vector<std::string> args;
    if (argc < 2) {
        exit(0);  // Exit if no arguments provided
    }
    
    // Convert C-style argv to vector of strings (skip program name)
    for (int a = 1; a < argc; a++) {
        args.push_back(std::string(argv[a]));
    }

    // Call main GTF to BED conversion function
    gtf2BedMain(args);
    
    return 0;
}