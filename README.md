# GTF2BED

A C++ command-line tool for converting GTF/GFF/GFF3 files to BED format with customizable feature filtering and attribute preservation.

```
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
```

## Overview

GTF2BED is a specialized bioinformatics tool that converts Gene Transfer Format (GTF), General Feature Format (GFF), and GFF3 files into Browser Extensible Data (BED) format. The tool preserves all original attributes as additional columns and allows selective filtering by feature types.

## Features

- **Multiple Format Support**: Handles GTF, GFF, and GFF3 input files
- **Feature Type Filtering**: Convert only specific feature types (exon, gene, transcript, etc.)
- **Attribute Preservation**: All original attributes are maintained as additional BED columns
- **Progress Tracking**: Real-time progress updates during file processing
- **Flexible Output**: Customizable output file specification
- **Error Validation**: Comprehensive input validation and error handling

## Installation

### Prerequisites

- C++ compiler with C++11 support or higher
- Boost libraries (specifically `boost::program_options`)
- Custom `ntools` library (included in `../lib/ntools.hpp`)

### Building

```bash
# Compile the program
mkdir build && cd build 
cmake ..
make 
```

## Usage

### Basic Syntax

```bash
./gtf2bed [OPTIONS]
```

### Required Arguments

- `-i, --input <file>`: Input GTF/GFF/GFF3 file
- `-o, --output <file>`: Output BED file
- `-f, --format <format>`: Input file format (GTF, GFF, or GFF3)

### Optional Arguments

- `-t, --feature-type <types>`: Feature types to include (default: "all")
- `-h, --help`: Show help message
- `--log <file>`: Redirect output to log file
- `--silent`: Disable screen output

### Examples

#### Convert entire GTF file to BED

```bash
./gtf2bed -i input.gtf -o output.bed -f gtf
```

#### Convert only exon features

```bash
./gtf2bed -i input.gtf -o exons.bed -f gtf -t exon
```

#### Convert multiple feature types

```bash
./gtf2bed -i input.gff3 -o features.bed -f gff3 -t gene transcript exon
```

#### Run with logging

```bash
./gtf2bed -i input.gtf -o output.bed -f gtf --log conversion.log
```

#### Silent mode

```bash
./gtf2bed -i input.gtf -o output.bed -f gtf --silent
```

## Output Format

The output BED file contains the following columns:

1. **chr**: Chromosome/sequence name
2. **start**: Start position (0-based, converted from 1-based GTF)
3. **end**: End position (1-based)
4. **id**: Gene ID from the `gene_id` attribute
5. **info**: Feature type (exon, gene, transcript, etc.)
6. **strand**: Strand information
7. **[attributes]**: Additional columns for each attribute found in the input file

### Example Output

```
#chr    start   end     id              info    strand  gene_id         transcript_id   gene_name
chr1    1000    2000    ENSG00000001    exon    +       ENSG00000001    ENST00000001    GENE1
chr1    1500    2500    ENSG00000002    exon    -       ENSG00000002    ENST00000002    GENE2
```

## File Format Support

### GTF (Gene Transfer Format)
- Standard GTF format with tab-separated fields
- Attributes in key-value pairs separated by semicolons

### GFF (General Feature Format)
- GFF2 format support
- Standard 9-column format

### GFF3 (General Feature Format Version 3)
- GFF3 format with enhanced attribute handling
- URL-encoded attribute values supported

## Error Handling

The program includes comprehensive error checking:

- **Input Validation**: Verifies all required arguments are provided
- **File Existence**: Checks if input files exist and are readable
- **Format Validation**: Ensures specified format matches file content
- **Feature Type Validation**: Verifies requested feature types exist in input file
- **Memory Management**: Efficient handling of large files

## Performance

- **Memory Efficient**: Processes files line by line with caching
- **Progress Tracking**: Shows progress every 10,000 lines processed
- **Scalable**: Handles large genomic files efficiently

## Troubleshooting

### Common Issues

1. **"Not all features specified could be found"**
   - Check that your feature types exist in the input file
   - Use `--feature-type all` to include all features

2. **"Input file needs to be specified"**
   - Ensure you're using the `-i` flag with a valid file path

3. **Build errors**
   - Verify Boost libraries are installed and accessible
   - Check that `ntools.hpp` is in the correct location

### Getting Help

For additional help, run:
```bash
./gtf2bed --help
```

## Author

**Nikolaos M.R. LYKOSKOUFIS**
- Email: nikolaos.lykoskoufis@gmail.com
- Version: 0.1

## License

Please refer to the license file or contact the author for licensing information.

## Contributing

For bug reports, feature requests, or contributions, please contact the author via email.

---

*This tool is designed for bioinformatics workflows requiring conversion between GTF/GFF formats and BED format while preserving all original annotation information.*