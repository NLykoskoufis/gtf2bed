#include "gtf2bed.hpp"

#include <iostream>

int main(int argc, char ** argv) {
    
    //1. Start timing
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
	std::cout << "\n" << "\x1B[35;1m"  << "GTF2BED" << "\033[0m" << std::endl; 
    std::cout << " * Authors : Nikolaos M.R. LYKOSKOUFIS" << std::endl;
    std::cout << " * Contact : nikolaos.lykoskoufis@gmail.com" << std::endl;
    std::cout << " * Version : version 0.1" << std::endl;

	//2. Open LOG file if necessary

	for (int a = 1 ; a < argc - 1 ; a ++) {
		if ((strcmp(argv[a], "--log") == 0) && !vrb.open_log(string(argv[a+1]))) vrb.error("Impossible to open log file!");
		if (strcmp(argv[a], "--silent") == 0) vrb.set_silent();
	}

	// MODES 
    std::vector < std::string > args;
    if(argc < 2){
        exit(0);
    }
    for(int a= 1; a < argc ; a++) args.push_back(std::string(argv[a]));

    // GTF 2 BED 
    gtf2BedMain(args);

        
}