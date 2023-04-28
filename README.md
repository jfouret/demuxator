
# Demuxator

## What is it ?

A small C++ program to demultiplex paired reads.

```
$ ./demuxator -h
Usage: ./demuxator [-e|--expected-barcodes expected_barcodes.txt] [-t|--threshold threshold] [--subset-size subset_size] [--bc-start bc_start] [--bc-length bc_length] [--remove-barcode] read1.fastq.gz read2.fastq.gz
  -e, --expected-barcodes: Path to the expected barcodes file (default: none)
  -t, --threshold: Threshold for selecting barcodes based on their relative occurrence in the subset (default: 0.01)
  --subset-size: Subset size for selecting barcodes (default: 10000)
  --bc-start: Start position of barcodes in read 2 (default: 10)
  --bc-length: Length of barcodes in read 2 (default: 12)
  --remove-barcode: Remove barcode from output reads (default: false)
  -h, --help: Show this help message
Output files are written in the current directory using the barcode sequence as prefix in filenames.
```

## Build

- Simply use `make`.
- `make test` will run tests
- `make coverage_report` will report the coverage of the tests

## Dependancies

### To build the program

- zlib
- libboost-dev
- libboost-iostreams-dev
- libboost-filesystem-dev
- c++ Compiler
- c++ standard libraries

### To run the tests

On debian 11:

```
sudo apt-get install gcovr lcov libgtest-dev
```