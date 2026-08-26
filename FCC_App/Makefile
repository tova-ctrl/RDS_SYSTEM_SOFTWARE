# =============================================================================
# Makefile — FCC_App
# עובד על WSL / Linux / Jetson
# =============================================================================

CXX      := g++
CC       := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
CFLAGS   := -std=c11 -Wall -Wextra -Iinclude
LDFLAGS  := -lpthread

SRC_DIR  := src
BUILD_DIR := build
TARGET   := $(BUILD_DIR)/fcc_app

SRCS   := $(wildcard $(SRC_DIR)/*.cpp)
OBJS   := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
SRCS_C := $(wildcard $(SRC_DIR)/*.c)
OBJS_C := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS_C))

# ── Default target ────────────────────────────────────────────────────────────
.PHONY: all
all: $(BUILD_DIR) $(TARGET)
	@echo ""
	@echo "Build OK → $(TARGET)"
	@echo ""
	@echo "Next steps (WSL):"
	@echo "  1. sudo modprobe vcan"
	@echo "  2. sudo ip link add dev vcan0 type vcan && sudo ip link set up vcan0"
	@echo "  3. make run"

# ── Link (g++ driver so the C++ runtime links correctly) ─────────────────────
$(TARGET): $(OBJS) $(OBJS_C)
	$(CXX) $(OBJS) $(OBJS_C) $(LDFLAGS) -o $@

# ── Compile ───────────────────────────────────────────────────────────────────
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ── Create build dir ──────────────────────────────────────────────────────────
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ── Run with vcan0 (WSL/Linux test) ──────────────────────────────────────────
# GCS IP: auto-detect Windows host gateway from /proc/net/route (WSL2 default route)
GCS_IP := $(shell awk '$$2=="00000000" && $$3!="00000000" {printf "%d.%d.%d.%d", strtonum("0x"substr($$3,7,2)), strtonum("0x"substr($$3,5,2)), strtonum("0x"substr($$3,3,2)), strtonum("0x"substr($$3,1,2)); exit}' /proc/net/route)

.PHONY: run
run: all
	@echo "GCS target: $(GCS_IP):5200"
	$(TARGET) vcan0 $(GCS_IP) 5200

# ── Build for Jetson (GStreamer + nvv4l2decoder) ──────────────────────────────
.PHONY: jetson
jetson: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -DJETSON_PLATFORM" \
	        LDFLAGS="$(LDFLAGS) $$(pkg-config --libs gstreamer-1.0 gstreamer-app-1.0)"
	@echo "Jetson build OK — uses GStreamer + nvv4l2decoder"

# ── Setup virtual CAN (WSL) ───────────────────────────────────────────────────
.PHONY: vcan
vcan:
	@echo "Setting up vcan0..."
	sudo modprobe vcan
	sudo ip link add dev vcan0 type vcan 2>/dev/null || true
	sudo ip link set up vcan0
	@echo "vcan0 ready:"
	@ip link show vcan0

# ── Monitor CAN traffic (requires can-utils) ──────────────────────────────────
.PHONY: monitor
monitor:
	@echo "Monitoring vcan0 (Ctrl+C to stop)..."
	candump vcan0

# ── Install dependencies (WSL Ubuntu) ────────────────────────────────────────
.PHONY: deps
deps:
	sudo apt-get update
	sudo apt-get install -y build-essential can-utils

# ── Clean ─────────────────────────────────────────────────────────────────────
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned."

# ── Help ──────────────────────────────────────────────────────────────────────
.PHONY: help
help:
	@echo "Targets:"
	@echo "  make          — build fcc_app"
	@echo "  make run      — build + run on vcan0"
	@echo "  make vcan     — setup virtual CAN interface (WSL)"
	@echo "  make monitor  — candump vcan0 traffic"
	@echo "  make deps     — install build tools + can-utils"
	@echo "  make clean    — remove build artifacts"
