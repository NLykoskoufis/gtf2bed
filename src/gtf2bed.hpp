#ifndef GTF2BED_HPP
#define GTF2BED_HPP


#include "../lib/ntools.hpp"



class GTF2Bed {
    public: 
        GTF2Bed()
        {
            linecount = 0;
        }
        ~GTF2Bed()
        {
            linecount = 0;
        }

        std::string input_file; 
        std::string outFile; 
        std::unordered_set<std::string> featureTypes;
        
        unsigned int linecount; 

        std::vector < GTFLine > cachedFile;
        std::unordered_set < std::string >  attribute_keys;


     // OPTIONS
    boost::program_options::options_description option_descriptions;
	boost::program_options::variables_map options;

    void cacheGTFFile(std::string, FileFormat);
    void writeToBed(std::vector < std::string>&);

};

//--------------------------//
//  FUNCTION DECLARATION   //
//--------------------------//

void gtf2BedMain(std::vector < std::string > & argv);


//--------------------//
//  INLINE FUNCTIONS  //
//--------------------//

inline bool is_default_feature_type(const std::unordered_set<std::string>& s)
{
    // returns 1 if true else 0 (false)
    return (s.size() == 1 && *s.begin() == "all");
}

inline bool is_present(const std::unordered_set<std::string>& x,
                       const std::unordered_set<std::string>& y)
{
    for (const std::string& x_item : x)
    {
        if (y.find(x_item) == y.end()) 
        {
            return false; // Missing values
        }
    }
    return true; // All values found
}


#endif 