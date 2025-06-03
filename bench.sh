#!/usr/bin/env bash
set -euo pipefail

# Benchmark settings
EXEC=./main
INPUT=big_image.bmp
OUTPUT_DIR=bench_results
THREADS=(1 2 4 8 16)
REPEATS=5

mkdir -p "$OUTPUT_DIR"
echo "threads,avg_ms" > "$OUTPUT_DIR/results.csv"

echo "Running benchmark on $INPUT"
for t in "${THREADS[@]}"; do
  echo "Threads: $t"
  total=0
  for i in $(seq 1 "$REPEATS"); do
    # Run and capture filter time
    line=$($EXEC -i "$INPUT" -o tmp/ -n "$t" -k 5 -s 1 2>&1 | grep "Filter time:")
    # Extract the milliseconds value (assumes format: "Filter time: X ms")
    ms=$(echo "$line" | awk '{print $3}')
    echo "  Run $i: $ms ms"
    total=$((total + ms))
  done
  avg=$((total / REPEATS))
  echo "Average for $t threads: $avg ms"
  echo "$t,$avg" >> "$OUTPUT_DIR/results.csv"
done

echo "Benchmark complete. Results in $OUTPUT_DIR/results.csv" 