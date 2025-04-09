# Configuration
PROJECT = GAUS_app
LIBPROJECT = lib$(PROJECT).a
TESTPROJECT = test-$(PROJECT)

# Option
USE_BLAS = 1
ONE = 1

# Debug print: show the value Make is actually using
$(info Value of USE_BLAS detected by make: [$(USE_BLAS)])

# Directories
SRC_DIR = src
TEST_DIR = tests
INC_DIR = include
EXT_DIR = externals

# Tools
CXX = g++
AR = ar 
ARFLAGS = crs

# Flags
CXXFLAGS = -I. -I$(INC_DIR) -I$(EXT_DIR)/eigen -I$(EXT_DIR)/lazycsv/include
CXXFLAGS += -std=c++17 -Wall -g -fPIC -Werror -Wpedantic -O3
CXXFLAGS += -DPROJECT_NAME="\"$(PROJECT)\"" 

LDLIBS_BASE = -lpthread

# --- BLAS/LAPACK ---
LDLIBS_BLAS = 
ifeq ($(USE_BLAS),$(ONE))
    CXXFLAGS += -DEIGEN_USE_BLAS -DEIGEN_USE_LAPACKE
    LDLIBS_BLAS += -lopenblas -llapacke # Adds BLAS/LAPACK libs
    INFO_MSG = " BLAS_ENABLED"          
else
    INFO_MSG = " BLAS_DISABLED"         
endif

# Линковка приложения
LDAPPFLAGS = $(LDLIBS_BASE) $(LDLIBS_BLAS)
LDGTESTFLAGS = $(LDLIBS_BASE) $(LDLIBS_BLAS) -lgtest -lgtest_main

# Source Files
APP_SRC = $(SRC_DIR)/main.cpp
LIB_SRC = $(filter-out $(APP_SRC), $(wildcard $(SRC_DIR)/*.cpp))
TEST_SRC = $(wildcard $(TEST_DIR)/*.cpp)

APP_OBJ = $(notdir $(APP_SRC:.cpp=.o))
LIB_OBJ = $(notdir $(LIB_SRC:.cpp=.o))
TEST_OBJ = $(notdir $(TEST_SRC:.cpp=.o))

# Dependencies 
DEPS = $(wildcard $(INC_DIR)/*.h)

# Targets
.PHONY: all test clean cleanall info

all: info $(PROJECT)

info:
	@echo "Building project $(PROJECT) $(INFO_MSG)..."

# Build application
$(PROJECT): $(APP_OBJ) $(LIBPROJECT)
	@echo "Linking application $(PROJECT)..."
	$(CXX) -o $@ $(APP_OBJ) $(LIBPROJECT) $(LDAPPFLAGS)

$(LIBPROJECT): $(LIB_OBJ)
	@echo "Creating library $(LIBPROJECT)..."
	$(AR) $(ARFLAGS) $@ $^

$(TESTPROJECT): $(TEST_OBJ) $(LIBPROJECT)
	@echo "Linking test executable $(TESTPROJECT)..."
	$(CXX) -o $@ $(TEST_OBJ) $(LIBPROJECT) $(LDGTESTFLAGS)

test: $(TESTPROJECT)
	@echo "Running unit tests..."
	@./$(TESTPROJECT)

$(APP_OBJ) $(LIB_OBJ): %.o: $(SRC_DIR)/%.cpp $(DEPS)
	@echo "Compiling $< -> $@..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TEST_OBJ): %.o: $(TEST_DIR)/%.cpp $(DEPS)
	@echo "Compiling $< -> $@..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<


# Clean rules
clean:
	@echo "Cleaning object files..."
	rm -f $(APP_OBJ) $(LIB_OBJ) $(TEST_OBJ)

cleanall: clean
	@echo "Cleaning executables and library..."
	rm -f $(PROJECT) $(LIBPROJECT) $(TESTPROJECT)
	rm -rf *.csv