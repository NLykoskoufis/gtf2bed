#ifndef GTF_ITERATOR_HPP
#define GTF_ITERATOR_HPP

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>

//BOOST INCLUDES
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

// -----------------------------
// FileFormat: Enum for different file formats
// -----------------------------
enum class FileFormat {
    GTF,
    GFF,
    GFF3
};

// -----------------------------
// GTFLine: Represents a line in a GTF/GFF file
// -----------------------------
struct GTFLine {
    std::string seqname;
    std::string source;
    std::string feature;
    int start = 0;
    int end = 0;
    std::string score;
    char strand = '.';
    std::string frame;
    std::unordered_map<std::string, std::string> attributes;
};

// -----------------------------
// GTFIterator: Input iterator for GTF/GFF files
// -----------------------------
class GTFIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type        = GTFLine;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const GTFLine*;
    using reference         = const GTFLine&;

    GTFIterator() : stream_(nullptr), format_(FileFormat::GTF) {}

    explicit GTFIterator(std::ifstream& stream, FileFormat format = FileFormat::GTF) 
        : stream_(&stream), format_(format) {
        ++(*this); // Load first valid line
    }

    reference operator*() const { return current_; }
    pointer operator->() const { return &current_; }

    // Prefix increment
    GTFIterator& operator++() {
        std::string line;
        while (std::getline(*stream_, line)) {
            if (line.empty() || line[0] == '#') continue; // skip comments and empty lines
            current_ = parse_line(line);
            return *this;
        }
        stream_ = nullptr; // EOF
        return *this;
    }

    // Postfix increment
    GTFIterator operator++(int) {
        GTFIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const GTFIterator& a, const GTFIterator& b) {
        return a.stream_ == b.stream_;
    }

    friend bool operator!=(const GTFIterator& a, const GTFIterator& b) {
        return !(a == b);
    }

private:
    std::ifstream* stream_;
    GTFLine current_;
    FileFormat format_;

    // Parses a line into a GTFLine structure based on format
    GTFLine parse_line(const std::string& line) {
        std::istringstream iss(line);
        GTFLine gtf;
        std::string attr_field;

        iss >> gtf.seqname >> gtf.source >> gtf.feature
            >> gtf.start >> gtf.end >> gtf.score
            >> gtf.strand >> gtf.frame;

        std::getline(iss, attr_field); // Get attribute field

        switch (format_) {
            case FileFormat::GTF:
                parse_gtf_attributes(attr_field, gtf.attributes);
                break;
            case FileFormat::GFF:
                parse_gff_attributes(attr_field, gtf.attributes);
                break;
            case FileFormat::GFF3:
                parse_gff3_attributes(attr_field, gtf.attributes);
                break;
        }

        return gtf;
    }

    // Parse GTF format attributes: key "value"; key "value";
    void parse_gtf_attributes(const std::string& attr_field, std::unordered_map<std::string, std::string>& attributes) {
        std::istringstream attr_stream(attr_field);
        std::string token;
        while (std::getline(attr_stream, token, ';')) {
            std::istringstream pair_stream(token);
            std::string key, value;
            pair_stream >> key;

            if (!key.empty()) {
                pair_stream >> std::ws; // skip whitespace
                std::getline(pair_stream, value, '"'); // skip to quote
                std::getline(pair_stream, value, '"'); // actual value
                attributes[key] = value;
            }
        }
    }

    // Parse GFF format attributes: key value; key value;
    void parse_gff_attributes(const std::string& attr_field, std::unordered_map<std::string, std::string>& attributes) {
        std::istringstream attr_stream(attr_field);
        std::string token;
        while (std::getline(attr_stream, token, ';')) {
            size_t eq_pos = token.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = token.substr(0, eq_pos);
                std::string value = token.substr(eq_pos + 1);
                
                // Trim leading/trailing whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // URL decode value for GFF3 (handle %20, %3D, etc.)
                attributes[key] = url_decode(value);
            }
        }
    }

    // Parse GFF3 format attributes: key=value;key=value;
    void parse_gff3_attributes(const std::string& attr_field, std::unordered_map<std::string, std::string>& attributes) {
        std::istringstream attr_stream(attr_field);
        std::string token;
        while (std::getline(attr_stream, token, ';')) {
            size_t eq_pos = token.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = token.substr(0, eq_pos);
                std::string value = token.substr(eq_pos + 1);
                
                // Trim leading/trailing whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // URL decode value for GFF3 (handle %20, %3D, etc.)
                attributes[key] = url_decode(value);
            }
        }
    }

    // Simple URL decoder for GFF3 format
    std::string url_decode(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '%' && i + 2 < str.length()) {
                int value;
                std::istringstream hex_stream(str.substr(i + 1, 2));
                if (hex_stream >> std::hex >> value) {
                    result += static_cast<char>(value);
                    i += 2;
                } else {
                    result += str[i];
                }
            } else {
                result += str[i];
            }
        }
        return result;
    }
};

// -----------------------------
// GTFFile: Range wrapper for using GTFIterator in for-loops
// -----------------------------
class GTFFile: public boost::iostreams::filtering_istream {

protected:
    std::ifstream file_descriptor;
    FileFormat format_;

public:
    explicit GTFFile(const std::string& filename, FileFormat format = FileFormat::GTF) 
        : format_(format) {
        if (filename.substr(filename.find_last_of(".") + 1) == "gz") {
            file_descriptor.open(filename.c_str(), std::ios::in | std::ios::binary);
            push(boost::iostreams::gzip_decompressor());
        } else if (filename.substr(filename.find_last_of(".") + 1) == "bz2") {
            file_descriptor.open(filename.c_str(), std::ios::in | std::ios::binary);
            push(boost::iostreams::bzip2_decompressor());
        } else {
            file_descriptor.open(filename.c_str());
        }
        if (!file_descriptor.fail()) push(file_descriptor);
    }

    GTFIterator begin() {
        return GTFIterator(file_descriptor, format_);
    }

    GTFIterator end() {
        return GTFIterator();
    }
};

#endif // GTF_ITERATOR_HPP