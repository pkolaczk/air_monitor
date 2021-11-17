#!/bin/bash
#
# Shell script that runs 'auniter verify ${board} MemoryBenchmark.ino',
# and collects the flash memory and static RAM usage for each of
# the FEATURE (0..13).
#
# Usage: collect.sh {board} {result_file}
#
# Creates a ${board}.out file containing:
#
#  FEATURE flash max_flash ram max_ram
#  0  aa bb cc dd
#  ...
#  13 aa bb cc dd

set -eu

# Assume that https://github.com/bxparks/AUniter is installed as a
# sibling project to AceTime.
AUNITER_CMD='../../../AUniter/tools/auniter.sh'
auniter_out_file=

function usage() {
    echo 'Usage: collect.sh {board} {result_file}'
    exit 1
}

function clean_temp_file() {
    if [[ "$auniter_out_file" != '' ]]; then
        rm -f $auniter_out_file
    fi
}

function create_temp_file() {
    auniter_out_file=
    trap "clean_temp_file" EXIT
    auniter_out_file=$(mktemp /tmp/memory_benchmark.auniter.XXXXXX)
}

# Usage: collect_for_board $board $result_file
# Output: A file named '${board}.out' that contains
function collect_for_board() {
    local board=$1
    local result_file=$2
    local feature_line=$(grep '#define FEATURE [0-9]*' MemoryBenchmark.ino)

    for feature in {0..13}; do
        echo "Collecting flash and ram usage for FEATURE $feature"
        sed -i -e "s/#define FEATURE [0-9]*/#define FEATURE $feature/" \
            MemoryBenchmark.ino

        if ! ($AUNITER_CMD verify $board MemoryBenchmark.ino 2>&1) > \
            $auniter_out_file; then
            # Ignore 'Sketch too big' condition, since we just want to
            # collect the flash and ram usage numbers.
            if ! grep -q 'Sketch too big' $auniter_out_file; then
                cat $auniter_out_file
                exit 1
            fi
        fi
        flash=$(grep 'Sketch uses' $auniter_out_file |
            awk '{print $3, $12}')
        memory=$(grep 'Global variables' $auniter_out_file |
            awk '{print $4, $18}')
        echo $feature $flash $memory >> $result_file
    done

    # Restore the 'FEATURE' line
    sed -i -e "s/#define FEATURE [0-9]*/$feature_line/" MemoryBenchmark.ino
}

trap interrupted INT
trap "clean_temp_file" EXIT

if [[ $# < 2 ]]; then
    usage
fi

rm -f $2
create_temp_file
echo "==== Collecting for board: $1"
collect_for_board $1 $2
