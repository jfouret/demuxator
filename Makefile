CXX=g++

CXXFLAGS = -Wall -Iinclude -Ofast
LDFLAGS = -lz -lboost_iostreams -lboost_filesystem
TESTS_CXXFLAGS = -fprofile-arcs -ftest-coverage
TESTS_LDFLAGS = -lgtest -lpthread

SRC_DIR=src
BUILD_DIR=build
TESTS_DIR=tests
TESTS_SRC_DIR=$(TESTS_DIR)/src
TESTS_BUILD_DIR=$(TESTS_DIR)/build
TESTS_BIN_DIR=$(TESTS_DIR)/bin
TESTS_COV_DIR=$(TESTS_DIR)/coverage_report

SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
TESTS_SOURCES=$(wildcard $(TESTS_SRC_DIR)/*.cpp)
TESTS_OBJECTS=$(patsubst $(TESTS_SRC_DIR)/%.cpp, $(TESTS_BUILD_DIR)/%.o, $(TESTS_SOURCES))
OBJECTS_COVERAGE=$(patsubst $(SRC_DIR)/%.cpp, $(TESTS_BUILD_DIR)/%.o, $(SOURCES))
TESTS_EXECUTABLES=$(patsubst $(TESTS_SRC_DIR)/%.cpp, $(TESTS_BIN_DIR)/%, $(TESTS_SOURCES))

# Create a separate list of object files for test binaries without main.o
TEST_OBJECTS=$(filter-out $(TESTS_BUILD_DIR)/main.o, $(OBJECTS_COVERAGE))

EXECUTABLE=demuxator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TESTS_BIN_DIR)/%: $(TESTS_BUILD_DIR)/%.o $(TEST_OBJECTS)
	mkdir -p $(TESTS_BIN_DIR)
	GCOV_PREFIX=$(TESTS_BUILD_DIR) $(CXX) $(CXXFLAGS) $(TESTS_CXXFLAGS) $^ -o $@ $(LDFLAGS) $(TESTS_LDFLAGS)

$(TESTS_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(TESTS_BUILD_DIR)
	GCOV_PREFIX=$(TESTS_BUILD_DIR) $(CXX) $(CXXFLAGS) $(TESTS_CXXFLAGS) -c $< -o $@

$(TESTS_BUILD_DIR)/%.o: $(TESTS_SRC_DIR)/%.cpp
	mkdir -p $(TESTS_BUILD_DIR)
	GCOV_PREFIX=$(TESTS_BUILD_DIR) $(CXX) $(TESTS_CXXFLAGS) $(CXXFLAGS) -c $< -o $@

test: unit_test e2e_test

unit_test: $(TESTS_EXECUTABLES)
	for test in $^ ; do \
		(cd $(TESTS_BIN_DIR) && ./`basename $$test`) ; \
	done

e2e_test: $(EXECUTABLE)
	bash tests/make_test.sh

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) $(TESTS_BUILD_DIR) $(TESTS_BIN_DIR) $(TESTS_COV_DIR) $(TESTS_DIR)/*fastq*

coverage_report: test
	mkdir -p $(TESTS_DIR)/coverage_report
	lcov --capture --directory $(TESTS_BUILD_DIR) --output-file $(TESTS_COV_DIR)/coverage.info
	lcov --extract $(TESTS_COV_DIR)/coverage.info "$(CURDIR)/$(SRC_DIR)/*" --output-file $(TESTS_COV_DIR)/filtered_coverage.info
	genhtml $(TESTS_COV_DIR)/filtered_coverage.info --output-directory $(TESTS_COV_DIR)

.PHONY: all test e2e_test unit_test clean coverage_report
