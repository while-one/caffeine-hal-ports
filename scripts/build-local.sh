#!/bin/bash
PRESET=${1:-linux-native}
CMAKE_TARGET=${2:-all}
shift 2
EXTRA_ARGS="$@" # All remaining arguments

# Determine the appropriate Docker stage based on the preset name
if [[ "$PRESET" == *"arm"* || "$PRESET" == *"stm32"* ]]; then
  STAGE="build-arm"
elif [[ "$PRESET" == *"riscv"* || "$PRESET" == *"gd32"* ]]; then
  STAGE="build-riscv"
else
  STAGE="build-native"
fi

# The image is built and pushed by the caffeine-hal repository
IMAGE_NAME="ghcr.io/${GITHUB_REPOSITORY_OWNER}/caffeine-hal/$STAGE:latest"

# Pull the image to ensure it's up-to-date
echo "Pulling Docker image: $IMAGE_NAME"
docker pull "$IMAGE_NAME" || { echo "Failed to pull image $IMAGE_NAME. Please ensure it's built and pushed from caffeine-hal."; exit 1; }

# Run preset build in container
docker run --rm -v "$(pwd)":/work -w /work "$IMAGE_NAME" \
    bash -c "rm -rf build && cmake --preset $PRESET -DFETCHCONTENT_FULLY_DISCONNECTED=ON $EXTRA_ARGS && cmake --build build/$PRESET --target $CMAKE_TARGET"
