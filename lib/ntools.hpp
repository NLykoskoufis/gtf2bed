#ifndef _NTOOLS_HPP
#define _NTOOLS_HPP


//INCLUDE STANDARD TEMPLATE LIBRARY USEFULL STUFFS (STL)
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <string>
#include <exception>
#include <iostream>

//INCLUDE BOOST USEFULL STUFFS (BOOST)
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>


//INCLUDE BASE STUFF 
#include "compression_io.h"
#include "GTFIterator.hpp"
#include <verbose.hpp>


//MAKE SOME TOOLS FULLY ACCESSIBLE THROUGHOUT THE SOFTWARE
#ifdef _DECLARE_TOOLBOX_HERE 
    verbose vrb; // Verbose
#else
    extern verbose vrb; 
#endif 

#endif // NTOOLS_HPP