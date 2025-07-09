#define _DECLARE_TOOLBOX_HERE
#include "gtf2bed.hpp"

/**
 * @brief Main function for GTF to BED conversion
 * 
 * This function handles the complete workflow of converting GTF/GFF/GFF3 files
 * to BED format. It performs the following steps:
 * 1. Parses command line arguments using boost::program_options
 * 2. Validates required parameters and input files
 * 3. Caches the entire GTF/GFF file in memory
 * 4. Extracts and sorts all attribute keys
 * 5. Writes the data in BED format to output file
 * 
 * @param argv Vector of command line arguments passed from main()
 * 
 * Exit codes:
 * - 0: Success or help requested
 * - 1: Missing required parameters or validation errors
 */
void gtf2BedMain(std::vector<std::string>& argv)
{
    GTF2Bed P;
    
    // Define basic command line options
    boost::program_options::options_description opt_basic("\x1B[35mBasics\33[0m");
    opt_basic.add_options()
        ("help,h", "Produces option description")
        ("log", boost::program_options::value<std::string>(), "Output on screen goes to this file.")
        ("silent", "Disable screen output");
    
    // Define input/output file options
    boost::program_options::options_description opt_files("\x1B[35mI/O\33[0m");
    opt_files.add_options()
        ("input,i", boost::program_options::value<std::string>(), "Input GTF/GFF/GFF3 file")
        ("output,o", boost::program_options::value<std::string>(), "Output file to write bed file")
        ("format,f", boost::program_options::value<std::string>(), "file format [GFF/GTF/GFF3]")
        ("feature-type,t",
         boost::program_options::value<std::vector<std::string>>()->multitoken()->composing()->default_value({"all"}, "all"), 
         "What feature types to include. \033[1mMake sure the feature type specified exists in your input file!!\033[0m");
        
    P.option_descriptions.add(opt_basic).add(opt_files);

    //-------------------
    // 2. PARSE OPTIONS
    //-------------------
    try {
        boost::program_options::store(boost::program_options::command_line_parser(argv).options(P.option_descriptions).run(), P.options);
        boost::program_options::notify(P.options);
    } catch (const boost::program_options::error& e) {
        std::cerr << "Error parsing [null] command line :" << std::string(e.what()) << std::endl;
        exit(0);
    }

    //---------------------
    // 3. PRINT HELP/HEADER
    //---------------------
    std::cout << "\n" << "\x1B[35;1m" << "CONVERT GTF TO BED " << "\033[0m" << std::endl;
    if (P.options.count("help")) {
        std::cout << P.option_descriptions << std::endl;
        exit(0);
    }

    //-----------------
    // 4. COMMON CHECKS
    //-----------------
    bool hasErrors = false;
    if (!P.options.count("input")) {
        std::cout << "Input file needs to be specified with --input" << std::endl;
        hasErrors = true;
    }
    if (!P.options.count("output")) {
        std::cout << "Output needs to be specified with --output [file.out]" << std::endl;
        hasErrors = true;
    }
    if (!P.options.count("format")) {
        std::cout << "You need to specify the file format." << std::endl;
        hasErrors = true;
    }

    if (hasErrors) {
        std::cout << P.option_descriptions << std::endl;
        exit(1);
    }

    // Process feature types if specified
    if (P.options.count("feature-type")) {
        const std::vector<std::string>& ft_vector = P.options["feature-type"].as<std::vector<std::string>>();
        P.featureTypes.insert(ft_vector.begin(), ft_vector.end());
    }

    P.outFile = P.options["output"].as<std::string>();
    P.input_file = P.options["input"].as<std::string>();
    
    // Determine file format
    FileFormat format_;
    if (P.options["format"].as<std::string>() == "gtf") format_ = FileFormat::GTF;
    if (P.options["format"].as<std::string>() == "gff") format_ = FileFormat::GFF;
    if (P.options["format"].as<std::string>() == "gff3") format_ = FileFormat::GFF3;

    //-------------
    // RUN ANALYSIS
    //-------------
    P.cacheGTFFile(P.input_file, format_);

    // Sort attribute keys for consistent output column ordering
    std::vector<std::string> sortedKeys(P.attribute_keys.begin(), P.attribute_keys.end());
    std::sort(sortedKeys.begin(), sortedKeys.end());

    P.writeToBed(sortedKeys);
}

/**
 * @brief Cache GTF/GFF file contents in memory and validate feature types
 * 
 * This function reads the entire GTF/GFF/GFF3 file line by line, storing each
 * line in memory and collecting all unique attribute keys. It also validates
 * that any requested feature types are actually present in the input file.
 * 
 * Processing steps:
 * 1. Opens and iterates through GTF/GFF file
 * 2. Displays progress every 10,000 lines
 * 3. Extracts all attribute keys from each line
 * 4. Collects all unique feature types
 * 5. Validates requested feature types exist in file
 * 
 * @param input_file Path to input GTF/GFF/GFF3 file
 * @param format_ File format enum (GTF, GFF, or GFF3)
 * 
 * @throws std::runtime_error via vrb.error() if requested feature types not found
 */
void GTF2Bed::cacheGTFFile(std::string input_file, FileFormat format_) {
    GTFFile gtf(input_file, format_);

    // Temporary set to collect all feature types present in file
    std::unordered_set<std::string> tmpFeatureSet;

    // Process each line in the GTF/GFF file
    for (const GTFLine& line : gtf) {
        // Display progress every 10,000 lines
        if (linecount % 10000 == 0) vrb.bullet("Read" + std::to_string(linecount));
        linecount++;
        
        // Extract all attribute keys from this line
        for (const auto& [key, value] : line.attributes) {
            attribute_keys.insert(key);
        }
        
        // Add feature type to set for validation
        tmpFeatureSet.insert(line.feature);
        
        // Cache the line for later processing
        cachedFile.push_back(line);
    }

    // Validate that all requested feature types are present in the file
    if (!is_default_feature_type(featureTypes) && !is_present(featureTypes, tmpFeatureSet)) {
        vrb.error("Not all features specified could be found in your input file. Exiting...");
    }
}

/**
 * @brief Write cached GTF data to BED format file
 * 
 * Converts the cached GTF/GFF data to BED format and writes to the output file.
 * The BED format includes standard columns (chr, start, end, id, info, strand)
 * plus additional columns for each attribute found in the input file.
 * 
 * BED format details:
 * - Start coordinates are converted from 1-based (GTF) to 0-based (BED)
 * - End coordinates remain 1-based
 * - Gene ID is used as the BED name field
 * - Feature type is stored in the info field
 * - All attributes are preserved as additional columns
 * - Missing attributes are filled with "."
 * 
 * @param sortedKeys Vector of sorted attribute keys for consistent column ordering
 */
void GTF2Bed::writeToBed(std::vector<std::string>& sortedKeys)
{
    output_file fdo(outFile.c_str());

    // Write header with standard BED columns plus all attributes
    fdo << "#chr\tstart\tend\tid\tinfo\tstrand";
    for (const std::string& item : sortedKeys) {
        fdo << "\t" << item;
    }
    fdo << "\n";

    // Write each GTF line in BED format
    for (const GTFLine& line : cachedFile) {
        fdo << line.seqname << "\t";
        fdo << std::to_string(line.start - 1) << "\t";  // Convert to 0-based start
        fdo << std::to_string(line.end) << "\t";        // Keep 1-based end
        fdo << line.attributes.at("gene_id") << "\t";   // Use gene_id as BED name
        fdo << line.feature << "\t";                    // Feature type in info field
        fdo << line.score;                              // Score field
        
        // Add all attributes in sorted order
        for (const std::string& key : sortedKeys) {
            if (line.attributes.count(key)) {
                fdo << "\t" << line.attributes.at(key);
            } else {
                fdo << "\t.";  // Fill missing attributes with "."
            }
        }
        fdo << "\n";
    }
}
