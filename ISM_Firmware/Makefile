# =============================================================================
# Makefile — ISM_Firmware
# עובד על WSL / Linux
# =============================================================================

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS  := -lpthread

SRC_DIR   := src
TEST_DIR  := tests
BUILD_DIR := build

# ── Sources (ללא main_sim.cpp — נקמפל בנפרד) ─────────────────────────────────
LIB_SRCS := $(SRC_DIR)/ISM.cpp $(SRC_DIR)/FccStub.cpp $(SRC_DIR)/IsmEmgLink.cpp
LIB_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(LIB_SRCS))

# ── Targets ───────────────────────────────────────────────────────────────────
TARGET_TEST := $(BUILD_DIR)/fault_injection_tests
TARGET_SIM  := $(BUILD_DIR)/ism_sim

TEST_OBJ    := $(BUILD_DIR)/FaultInjectionTests.o
SIM_OBJS    := $(BUILD_DIR)/IsmSimCan.o $(BUILD_DIR)/IsmSimEth.o $(BUILD_DIR)/main_sim.o

# ── Default — בנה הכל ─────────────────────────────────────────────────────────
.PHONY: all
all: $(BUILD_DIR) $(TARGET_TEST) $(TARGET_SIM)
	@echo ""
	@echo "Build OK:"
	@echo "  $(TARGET_TEST)  — make test"
	@echo "  $(TARGET_SIM)   — make sim"

# ── Link: tests ───────────────────────────────────────────────────────────────
$(TARGET_TEST): $(LIB_OBJS) $(TEST_OBJ)
	$(CXX) $^ $(LDFLAGS) -o $@

# ── Link: ISM simulator ───────────────────────────────────────────────────────
$(TARGET_SIM): $(LIB_OBJS) $(SIM_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# ── Compile ───────────────────────────────────────────────────────────────────
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJ): $(TEST_DIR)/FaultInjectionTests.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Create build dir ──────────────────────────────────────────────────────────
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ── Run tests ─────────────────────────────────────────────────────────────────
.PHONY: test
test: $(TARGET_TEST)
	$(TARGET_TEST)

# ── Run ISM simulator (vcan0) ─────────────────────────────────────────────────
.PHONY: sim
sim: $(TARGET_SIM)
	$(TARGET_SIM) vcan0

# ── Clean ─────────────────────────────────────────────────────────────────────
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned."

# ── Help ──────────────────────────────────────────────────────────────────────
.PHONY: help
help:
	@echo "Targets:"
	@echo "  make       — build tests + ism_sim"
	@echo "  make test  — run SWR-SAFE-001..008 tests"
	@echo "  make sim   — run ISM simulator on vcan0"
	@echo "  make clean — remove build artifacts"
