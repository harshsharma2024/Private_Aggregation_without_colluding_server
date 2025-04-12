# Compiler and flags
CXX = g++
CXXFLAGS = 

# Source files
AGGREGATOR_SRC = aggregator.cpp
USER_SRC = user.cpp

# Output binaries
AGGREGATOR_BIN = aggregator
USER_BIN = user

# Default target
all: $(AGGREGATOR_BIN) $(USER_BIN)

# Compile aggregator
$(AGGREGATOR_BIN): $(AGGREGATOR_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile user
$(USER_BIN): $(USER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lcrypto

# Clean build artifacts
clean:
	rm -f $(AGGREGATOR_BIN) $(USER_BIN)

# Run targets (optional)
run_aggregator: $(AGGREGATOR_BIN)
	./$(AGGREGATOR_BIN)

run_user: $(USER_BIN)
	./$(USER_BIN)
