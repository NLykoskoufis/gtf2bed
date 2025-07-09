#ifndef GTF2BED_HPP
#define GTF2BED_HPP

#include "../lib/ntools.hpp"

/**
 * @class GTF2Bed
 * @brief A class for converting GTF/GFF/GFF3 files to BED format
 * 
 * This class provides functionality to read GTF, GFF, or GFF3 files and convert
 * them to BED format while preserving all original attributes as additional columns.
 * It supports filtering by feature types and maintains all annotation information.
 */
class GTF2Bed {
    public: 
        /**
         * @brief Default constructor
         * 
         * Initializes the GTF2Bed object with default values.
         * Sets linecount to 0 and prepares internal data structures.
         */
        GTF2Bed()
        {
            linecount = 0;
        }
        
        /**
         * @brief Destructor
         * 
         * Cleans up resources and resets linecount to 0.
         */
        ~GTF2Bed()
        {
            linecount = 0;
        }

        std::string input_file;                          ///< Path to input GTF/GFF/GFF3 file
        std::string outFile;                             ///< Path to output BED file
        std::unordered_set<std::string> featureTypes;    ///< Set of feature types to include in output
        
        unsigned int linecount;                          ///< Counter for processed lines (for progress tracking)

        std::vector<GTFLine> cachedFile;                 ///< Cached GTF/GFF lines for processing
        std::unordered_set<std::string> attribute_keys;  ///< Set of all attribute keys found in input file

        // OPTIONS
        boost::program_options::options_description option_descriptions;  ///< Command line option descriptions
        boost::program_options::variables_map options;                   ///< Parsed command line options

        /**
         * @brief Cache GTF/GFF file contents in memory
         * 
         * Reads the entire GTF/GFF/GFF3 file into memory, extracts all attribute keys,
         * and validates that requested feature types are present in the file.
         * 
         * @param input_file Path to the input GTF/GFF/GFF3 file
         * @param format_ File format (GTF, GFF, or GFF3)
         * 
         * @throws std::runtime_error if requested feature types are not found in file
         */
        void cacheGTFFile(std::string input_file, FileFormat format_);

        /**
         * @brief Write cached GTF data to BED format file
         * 
         * Converts the cached GTF/GFF data to BED format and writes to output file.
         * Creates a header with standard BED columns plus all attribute columns.
         * Coordinates are converted from 1-based (GTF) to 0-based (BED) for start position.
         * 
         * @param sortedKeys Vector of sorted attribute keys for consistent column ordering
         */
        void writeToBed(std::vector<std::string>& sortedKeys);
};

//--------------------------//
//  FUNCTION DECLARATION   //
//--------------------------//

/**
 * @brief Main function for GTF to BED conversion
 * 
 * Parses command line arguments, validates input parameters, and orchestrates
 * the conversion process from GTF/GFF/GFF3 to BED format.
 * 
 * @param argv Vector of command line arguments
 * 
 * Command line options:
 * - --input, -i: Input GTF/GFF/GFF3 file (required)
 * - --output, -o: Output BED file (required)
 * - --format, -f: Input file format [GTF/GFF/GFF3] (required)
 * - --feature-type, -t: Feature types to include (default: "all")
 * - --help, -h: Show help message
 * - --log: Output log file
 * - --silent: Disable screen output
 */
void gtf2BedMain(std::vector<std::string>& argv);

//--------------------//
//  INLINE FUNCTIONS  //
//--------------------//

/**
 * @brief Check if feature type set contains only the default "all" value
 * 
 * @param s Unordered set of feature types to check
 * @return true if set contains only "all", false otherwise
 */
inline bool is_default_feature_type(const std::unordered_set<std::string>& s)
{
    // returns 1 if true else 0 (false)
    return (s.size() == 1 && *s.begin() == "all");
}

/**
 * @brief Check if all elements in set x are present in set y
 * 
 * Validates that all requested feature types are available in the input file.
 * 
 * @param x Set of requested feature types
 * @param y Set of available feature types from input file
 * @return true if all elements in x are found in y, false otherwise
 */
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
