#!/bin/bash

ELF="./e2e_tests/queens.elf"
MODULES="off"
PROFILING="off"
VALGRIND_MEMCHECK="off"
VALGRIND_CALLGRIND="off"
USE_PGO="on"     
FORCE_GENERATE_PGO="off"
CMAKE_ARGS=""

USAGE="Usage: $0 [-e|--elf <elf_file>] [-m|--modules] [-p|--profile] [--valgrind-memcheck] [--valgrind-callgrind] [--no-pgo] [--generate-pgo]"

while [[ $# -gt 0 ]]; do
  case $1 in
    -e|--elf)
      ELF="$2"
      shift 2
      ;;
    -m|--modules)
      MODULES="on"
      CMAKE_ARGS="$CMAKE_ARGS -DENABLE_MODULES=ON"
      shift
      ;;
    -p|--profile)
      PROFILING="on"
      CMAKE_ARGS="$CMAKE_ARGS -DPROFILING=ON"
      shift
      ;;
    --valgrind-memcheck)
      VALGRIND_MEMCHECK="on"
      CMAKE_ARGS="$CMAKE_ARGS -DPROFILING=ON"
      shift
      ;;
    --valgrind-callgrind)
      VALGRIND_CALLGRIND="on"
      CMAKE_ARGS="$CMAKE_ARGS -DPROFILING=ON"
      shift
      ;;
    --no-pgo)
      USE_PGO="off"
      shift
      ;;
    --generate-pgo)
      FORCE_GENERATE_PGO="on"
      USE_PGO="on"
      shift
      ;;
    -h|--help)
      echo "$USAGE"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo "$USAGE"
      exit 1
      ;;
  esac
done

echo "Resulting Configuration:"
echo "ELF file:              $ELF"
echo "Modules support:       $MODULES"
echo "Perf Profiling:        $PROFILING"
echo "Valgrind Memcheck:     $VALGRIND_MEMCHECK"
echo "Valgrind Callgrind:    $VALGRIND_CALLGRIND"
echo "PGO mode:              $USE_PGO (force generate: $FORCE_GENERATE_PGO)"
echo ""


if [ "$FORCE_GENERATE_PGO" = "on" ] || [ "$USE_PGO" = "off" ]; then
  echo "Cleaning build directory..."
  rm -rf ./build
else
  echo "Trying to reuse existing build (will clean only if needed)"
fi

BASE_CMAKE="cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -UGENERATE_PGO -UOPTIMIZE_PGO $CMAKE_ARGS"


build_with_pgo() {
  local pgo_flag="$1"
  local mode_desc="$2"

  echo "Configuring CMake ($mode_desc)..."
  echo "- Using: $BASE_CMAKE -D${pgo_flag}=ON"

  if ! $BASE_CMAKE -D${pgo_flag}=ON; then
    echo "CMake configuration failed."
    exit 1
  fi

  echo "Building..."
  if ! cmake --build build -j$(nproc); then
    return 1
  fi
  return 0
}


if [ "$USE_PGO" = "on" ]; then
  if [ "$FORCE_GENERATE_PGO" = "on" ]; then
    echo "Forced PGO PROFILE GENERATION mode (--generate-pgo)"
    
    if ! build_with_pgo "GENERATE_PGO" "instrumentation"; then
      echo "Failed to build instrumentation version"
      exit 1
    fi

    echo "Running simulator to collect profiles..."
    SIM_CMD="build/bin/riscv_sim --config=./config/configx86.conf --input=$ELF"
    [ "$MODULES" = "on" ] && SIM_CMD="$SIM_CMD --module=example"

    if ! $SIM_CMD; then
      echo "Simulator run failed - profiles not collected"
      exit 1
    fi

    echo "Profile collection complete."
    echo "Now you can run the script without --generate-pgo to use optimized build."
    exit 1
  else
    echo "Smart PGO: attempting to use existing profiles first..."

    if build_with_pgo "OPTIMIZE_PGO" "optimization using profiles"; then
      echo "Success: built with PGO profiles"
    else
      echo "Optimization build failed (probably no/missing profiles)."
      echo "Falling back to profile generation..."

      echo "Cleaning build directory for instrumentation..."
      rm -rf ./build

      if ! build_with_pgo "GENERATE_PGO" "instrumentation"; then
        echo "Instrumentation build failed"
        exit 1
      fi

      echo "Collecting profiles by running simulator..."
      SIM_CMD="build/bin/riscv_sim --config=./config/configx86.conf --input=$ELF"
      [ "$MODULES" = "on" ] && SIM_CMD="$SIM_CMD --module=example"

      if ! $SIM_CMD; then
        echo "Profile collection failed"
        exit 1
      fi

      echo "Profiles collected. Rebuilding with optimization..."

      if ! build_with_pgo "OPTIMIZE_PGO" "final optimization"; then
        echo "Final PGO-optimized build failed"
        exit 1
      fi

      echo "PGO-optimized build completed successfully"
    fi
  fi

else
  echo "Building without PGO..."
  if ! $BASE_CMAKE; then
    echo "CMake configuration failed"
    exit 1
  fi
  if ! cmake --build build -j$(nproc); then
    echo "Build failed"
    exit 1
  fi
fi


SIM_CMD="build/bin/riscv_sim --config=./config/configx86.conf --input=$ELF"
[ "$MODULES" = "on" ] && SIM_CMD="$SIM_CMD --module=example"

ANY_ANALYSIS="off"
[ "$PROFILING" = "on" ] && ANY_ANALYSIS="on"
[ "$VALGRIND_MEMCHECK" = "on" ] && ANY_ANALYSIS="on"
[ "$VALGRIND_CALLGRIND" = "on" ] && ANY_ANALYSIS="on"

if [ "$ANY_ANALYSIS" = "off" ]; then
  echo "Running simulation..."
  $SIM_CMD
fi

if [ "$PROFILING" = "on" ]; then
  [ ! -d "FlameGraph" ] && git clone https://github.com/brendangregg/FlameGraph.git
  perf record -g --call-graph dwarf $SIM_CMD
  perf script > out.perf
  ./FlameGraph/stackcollapse-perf.pl out.perf > out.folded
  ./FlameGraph/flamegraph.pl out.folded > flamegraph.svg
  echo "Perf profiling done - flamegraph.svg"
fi

if [ "$VALGRIND_MEMCHECK" = "on" ]; then
  valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes \
    $SIM_CMD > valgrind_memcheck.log 2>&1
  echo "Memcheck done - valgrind_memcheck.log"
fi

if [ "$VALGRIND_CALLGRIND" = "on" ]; then
  rm -rf callgrind*
  valgrind --tool=callgrind --callgrind-out-file=callgrind.out.%p $SIM_CMD
  callgrind_annotate --auto=yes callgrind.out.* > callgrind_annotate.txt
  echo "Callgrind done - callgrind_annotate.txt / kcachegrind"
fi

echo "Done."