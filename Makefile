CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude
TARGET   = alf_ewg

SRCS = src/TelemetryBuffer.cpp \
       src/Entities.cpp \
       src/SensorHierarchy.cpp \
       src/PersistenceAndSTL.cpp \
       src/main.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) incidents.bin

.PHONY: all clean
