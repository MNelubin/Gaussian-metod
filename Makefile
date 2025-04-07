# Configuration
PROJECT = GAUS_app
LIBPROJECT = $(PROJECT).a
TESTPROJECT = test-$(PROJECT)
TEST_SCRIPT = test/test.sh

# Directories
SRC_DIR = src
TEST_DIR = tests
INC_DIR = include

# Tools
CXX = g++
A = ar
AFLAGS = crs

# Flags
CXXFLAGS = -I. -std=c++17 -Wall -g -fPIC -Werror -Wpedantic -Iinclude
LDXXFLAGS = $(CXXFLAGS) -lpthread
LDGTESTFLAGS = $(CXXFLAGS) -lgtest -lgtest_main -lpthread

# Source Files
SRC = $(wildcard $(SRC_DIR)/*.cpp) 

# Split sources: Application (main.cpp) and Library (others)
APP_SRC = $(SRC_DIR)/main.cpp
LIB_SRC = $(filter-out $(APP_SRC), $(SRC))
LIB_OBJ = $(LIB_SRC:.cpp=.o)
APP_OBJ = $(APP_SRC:.cpp=.o)

TEST_SRC = $(TEST_DIR)/test-$(PROJECT).cpp
TEST_OBJ = $(TEST_SRC:.cpp=.o)

# Dependencies
DEPS = $(wildcard $(INC_DIR)/*.h)

# Targets
.PHONY: all test integration-test clean cleanall

all: $(PROJECT)

# Build application
$(PROJECT): $(APP_OBJ) $(LIBPROJECT)
	$(CXX) -o $@ $(APP_OBJ) $(LIBPROJECT) $(LDXXFLAGS)


# Build static library
$(LIBPROJECT): $(LIB_OBJ)
	$(A) $(AFLAGS) $@ $^

# Build test
$(TESTPROJECT): $(TEST_OBJ) $(LIBPROJECT)
	$(CXX) -o $@ $(TEST_OBJ) $(LIBPROJECT) $(LDGTESTFLAGS)

# Run tests
test: $(TESTPROJECT)
	@echo "Running unit tests..."
	@./$(TESTPROJECT)

integration-test: $(PROJECT) $(TEST_SCRIPT)
	@echo "Running integration tests..."
	@chmod +x $(TEST_SCRIPT)
	@./$(TEST_SCRIPT)

testall: test integration-test

# Compile C++ files
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean rules
clean:
	rm -f $(APP_OBJ) $(LIB_OBJ) $(TEST_OBJ)

cleanall: clean
	rm -f $(PROJECT) $(LIBPROJECT) $(TESTPROJECT)
	rm -rf test_data
