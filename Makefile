# Compiler and Linker
CXX = g++
LD = g++

# Windows-specific settings
ifeq ($(OS), Windows_NT)
    # Use MinGW for Windows builds, assuming you have it installed
    CC = gcc
    EXE = app.exe
    RM = del
    MKDIR = mkdir
else
    # Linux-specific settings
    RM = rm -f
    MKDIR = mkdir -p
	EXE = app
endif

# Directories
SRC_DIR = src
API_DIR = $(SRC_DIR)/api
IMGUI_DIR = $(SRC_DIR)/imgui
LIB_DIR = libs
BUILD_DIR = build
API_BUILD_DIR = $(BUILD_DIR)\api
IMGUI_BUILD_DIR = $(BUILD_DIR)\imgui

# Libraries
GLAD_INCLUDE = $(LIB_DIR)/glad/inc
GLFW_INCLUDE = $(LIB_DIR)/glfw/inc
GLM_INCLUDE = $(LIB_DIR)/glm/inc
PUGIXML_INCLUDE = $(LIB_DIR)/pugixml/include
SDL_INCLUDE = $(LIB_DIR)/SDL/include
VULKAN_INCLUDE = $(LIB_DIR)/vulkan/include

# Include directories
INCLUDES = -I$(GLAD_INCLUDE) -I$(GLFW_INCLUDE) -I$(SDL_INCLUDE) -I$(VULKAN_INCLUDE) -I$(GLM_INCLUDE) -I$(PUGIXML_INCLUDE) -I$(SRC_DIR)

# Sources
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(API_DIR)/*.cpp) $(wildcard $(IMGUI_DIR)/*.cpp)

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable
EXEC = $(BUILD_DIR)/$(EXE)

# Flags
CXXFLAGS = -Wall -g -std=c++20 $(INCLUDES) 

# WINDOWS LIBRARIES
# LDFLAGS = -L$(LIB_DIR)/glfw/lib -L$(LIB_DIR)/SDL/lib -lshaderc_combined -L$(LIB_DIR)/vulkan/lib -L$(LIB_DIR)/pugixml/lib -lpthread
# LDFLAGS = -L$(LIB_DIR)/glfw/lib -L$(LIB_DIR)/SDL/lib -L$(LIB_DIR)/vulkan/lib -L$(LIB_DIR)/pugixml/lib -lpthread
LDFLAGS = -L$(LIB_DIR)/glfw/lib -L$(LIB_DIR)/SDL/lib -L$(LIB_DIR)/vulkan/lib -L$(LIB_DIR)/pugixml/lib -lshaderc_combined -lvulkan-1 -lglfw3 -lpugixml
# LINUX LIBRARIES
# LDFLAGS = -lvulkan -lglfw -lpugixml -lpthread -lshaderc

# Default target
all: $(EXEC)

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)
	$(MKDIR) $(API_BUILD_DIR)
	$(MKDIR) $(IMGUI_BUILD_DIR)

# Compile the sources
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# $(CXX) $(OBJECTS) $(LDFLAGS) -o $(EXEC) -lshaderc_combined -lvulkan-1 -lglfw3 -lpugixml
# Link the object files to create the executable
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(EXEC)

# Clean up the build directory
clean:
	$(RM) $(BUILD_DIR)

.PHONY: all clean
