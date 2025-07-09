#define _DECLARE_TOOLBOX_HERE
#include "gtest/gtest.h"
#include "../lib/ntools.hpp"
#include "../src/gtf2bed.hpp"
#include "../src/gtf2bed.cpp"
#include <iomanip>
#include <openssl/sha.h>  // make sure OpenSSL is installed and linked

#include <fstream>
#include <cstdio>


std::string sha256_of_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open file: " + filename);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    // last chunk
    if (file.gcount() > 0) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (unsigned char c : hash)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)c;

    return ss.str();
}



// ---------- TESTS FOR is_default_feature_type ---------- //

TEST(GTF2BedTests, DefaultFeatureType_AllOnly_ReturnsTrue) {
    std::unordered_set<std::string> features = {"all"};
    EXPECT_TRUE(is_default_feature_type(features));
}

TEST(GTF2BedTests, DefaultFeatureType_EmptySet_ReturnsFalse) {
    std::unordered_set<std::string> features;
    EXPECT_FALSE(is_default_feature_type(features));
}

TEST(GTF2BedTests, DefaultFeatureType_MultipleItems_ReturnsFalse) {
    std::unordered_set<std::string> features = {"all", "exon"};
    EXPECT_FALSE(is_default_feature_type(features));
}

TEST(GTF2BedTests, DefaultFeatureType_NotAll_ReturnsFalse) {
    std::unordered_set<std::string> features = {"exon"};
    EXPECT_FALSE(is_default_feature_type(features));
}

// ---------- TESTS FOR is_present ---------- //

TEST(GTF2BedTests, IsPresent_AllItemsPresent_ReturnsTrue) {
    std::unordered_set<std::string> x = {"exon", "gene"};
    std::unordered_set<std::string> y = {"exon", "gene", "transcript"};
    EXPECT_TRUE(is_present(x, y));
}

TEST(GTF2BedTests, IsPresent_SomeItemsMissing_ReturnsFalse) {
    std::unordered_set<std::string> x = {"exon", "CDS"};
    std::unordered_set<std::string> y = {"exon", "gene"};
    EXPECT_FALSE(is_present(x, y));
}

TEST(GTF2BedTests, IsPresent_EmptyXSet_ReturnsTrue) {
    std::unordered_set<std::string> x;
    std::unordered_set<std::string> y = {"exon", "gene"};
    EXPECT_TRUE(is_present(x, y));  // Empty set is trivially present
}

TEST(GTF2BedTests, IsPresent_EmptyYSet_ReturnsFalse) {
    std::unordered_set<std::string> x = {"exon"};
    std::unordered_set<std::string> y;
    EXPECT_FALSE(is_present(x, y));
}


TEST(GTF2BedTest, CacheGTFFileWorksCorrectly) {
    // Setup test object
    GTF2Bed converter;
    converter.featureTypes = {"all"};  // What we expect to be present

    // Call the function on a real or mock file
    std::string input_file = "../data/example.gtf";
    FileFormat format = FileFormat::GTF;  // Adjust this to match your enum/type

    converter.cacheGTFFile(input_file, format);


    // Check that some lines were cached
    EXPECT_EQ(converter.cachedFile.size(), 10000);

    // Check that attributes were stored
    EXPECT_TRUE(converter.attribute_keys.find("gene_id") != converter.attribute_keys.end());
    EXPECT_TRUE(converter.attribute_keys.find("transcript_id") != converter.attribute_keys.end());

    // Line count should match
    //EXPECT_EQ(converter.linecount, 2);
}


TEST(GTF2BedTest, WriteToBedCreatesCorrectOutput) {
    GTF2Bed converter;

    // Setup output file path
    std::string tempFilename = "test_output.bed";
    converter.outFile = tempFilename;

    // Prepare cachedFile with fake data
    GTFLine line;
    line.seqname = "chr1";
    line.start = 100;
    line.end = 200;
    line.feature = "exon";
    line.score = "1000";
    line.attributes = {
        {"gene_id", "gene1"},
        {"transcript_id", "tx1"},
        {"gene_name", "GeneName1"}
    };
    converter.cachedFile.push_back(line);

    // Sorted keys to write (attributes)
    std::vector<std::string> sortedKeys = {"transcript_id", "gene_name", "missing_key"};

    // Call the function
    converter.writeToBed(sortedKeys);

    // Read the output file
    std::ifstream infile(tempFilename);
    ASSERT_TRUE(infile.is_open());

    std::string content((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    infile.close();

    // Expected string (tab separated)
    std::string expected =
        "#chr\tstart\tend\tid\tinfo\tstrand\ttranscript_id\tgene_name\tmissing_key\n"
        "chr1\t99\t200\tgene1\texon\t1000\ttx1\tGeneName1\t.\n";

    EXPECT_EQ(content, expected);

    // Clean up
    std::remove(tempFilename.c_str());
}


TEST(GTF2BedTest, WriteToBedMatchesGoldenFile) {
    GTF2Bed converter;

    // Call the function on a real or mock file
    std::string input_file = "../data/example.gtf";
    FileFormat format = FileFormat::GTF;  // Adjust this to match your enum/type
    converter.cacheGTFFile(input_file, format);

    // Setup output file path
    std::string outputFile = "test_output.bed";
    converter.outFile = outputFile;

    // Get attribute keys && sort them 
    std::vector < std::string > sortedKeys(converter.attribute_keys.begin(), converter.attribute_keys.end());
    std::sort(sortedKeys.begin(), sortedKeys.end());

    converter.writeToBed(sortedKeys);

    // Compute hashes
    std::string outputHash = sha256_of_file(outputFile);
    std::string goldenHash = sha256_of_file("../tests/expected_output.bed");

    EXPECT_EQ(outputHash, goldenHash);

    std::remove(outputFile.c_str());
}