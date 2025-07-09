#define _DECLARE_TOOLBOX_HERE
#include "gtf2bed.hpp"

void gtf2BedMain(std::vector < std::string > & argv)
{
    GTF2Bed P;
    
    boost::program_options::options_description opt_basic ("\x1B[35mBasics\33[0m");
	opt_basic.add_options()
		("help,h", "Produces option description")
        ("log", boost::program_options::value< std::string >(), "Output on screen goes to this file.")
		("silent", "Disable screen output");
    
    boost::program_options::options_description opt_files ("\x1B[35mI/O\33[0m");
	opt_files.add_options()
		("input,i", boost::program_options::value< std::string >(), "Input GTF/GFF/GFF3 file")
		("output,o", boost::program_options::value< std::string >(), "Output file to write bed file")
        ("format,f", boost::program_options::value< std::string > (), "file format [GFF/GTF/GFF3]")
        ("feature-type,t",
         boost::program_options::value<std::vector<std::string>>()->multitoken()->composing()->default_value({"all"}, "all"), "What feature types to include. \033[1mMake sure the feature type specified exists in your input file!!\033[0m");
        
    P.option_descriptions.add(opt_basic).add(opt_files);

    //-------------------
	// 2. PARSE OPTIONS
	//-------------------
	try {
		boost::program_options::store(boost::program_options::command_line_parser(argv).options(P.option_descriptions).run(), P.options);
		boost::program_options::notify(P.options);
	} catch ( const boost::program_options::error& e ) {
		std::cerr << "Error parsing [null] command line :" << std::string(e.what()) << std::endl;
		exit(0);
	}

    //---------------------
	// 3. PRINT HELP/HEADER
	//---------------------
    std::cout << "\n" << "\x1B[35;1m"  << "CONVERT GTF TO BED " << "\033[0m" << std::endl;
    if(P.options.count("help")) {
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

    if (P.options.count("feature-type"))
    {
        const std::vector<std::string>& ft_vector = P.options["feature-type"].as<std::vector<std::string>>();
        P.featureTypes.insert(ft_vector.begin(), ft_vector.end());
    }


    P.outFile = P.options["output"].as< std::string >();
    
    P.input_file = P.options["input"].as<std::string>();
    FileFormat format_;

    if (P.options["format"].as<std::string>() == "gtf") format_ = FileFormat::GTF;
    if (P.options["format"].as<std::string>() == "gff") format_ = FileFormat::GFF;
    if (P.options["format"].as<std::string>() == "gff3") format_ = FileFormat::GFF3;

    
    //-------------
    // RUN ANALYSIS
    //-------------
    P.cacheGTFFile(P.input_file, format_);

    std::vector < std::string > sortedKeys(P.attribute_keys.begin(), P.attribute_keys.end());
    std::sort(sortedKeys.begin(), sortedKeys.end());

    P.writeToBed(sortedKeys);
    
}

void GTF2Bed::cacheGTFFile(std::string input_file, FileFormat format_){
    GTFFile gtf(input_file, format_);

    //TMP check that requested feature types are present in GTF file.
    std::unordered_set < std::string > tmpFeatureSet;

    for (const GTFLine& line: gtf)
    {
        if (linecount % 10000 == 0) vrb.bullet("Read" + std::to_string(linecount));
        linecount++;
        for (const auto& [key, value] : line.attributes)
        {
            attribute_keys.insert(key);
        }
        // Add feature type to unordered_set 
        tmpFeatureSet.insert(line.feature);
        cachedFile.push_back(line);
    }

    // Make sure that selected feature types are present. If not throw error and exit.
    if (!is_default_feature_type(featureTypes) && !is_present(featureTypes, tmpFeatureSet)) 
    {
        vrb.error("Not all features specified could be found in your input file. Exiting...");
    }

    
}

void GTF2Bed::writeToBed(std::vector< std::string >& sortedKeys)
{
    output_file fdo (outFile.c_str());

    // write header first 
    fdo << "#chr\tstart\tend\tid\tinfo\tstrand";
    for (const std::string& item : sortedKeys)
    {
        fdo << "\t" << item;
    }
    fdo << "\n";

    // Write the GTF file in BED format
    for (const GTFLine& line : cachedFile)
    {
        fdo << line.seqname << "\t";
        fdo << std::to_string(line.start - 1) << "\t";
        fdo << std::to_string(line.end) << "\t";
        fdo << line.attributes.at("gene_id") << "\t";
        fdo << line.feature << "\t"; 
        fdo << line.score;
        // Add all attributes if present 
        for (const std::string& key : sortedKeys)
        {
            if (line.attributes.count(key))
            {
                fdo << "\t" << line.attributes.at(key);
            }
            else 
            {
                fdo << "\t.";
            }
        }
        fdo << "\n";
    }
}