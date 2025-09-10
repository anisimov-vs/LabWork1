#!/usr/bin/env python3
import csv
import matplotlib.pyplot as plt

def main():
    csv_path = 'bench_results/results.csv'
    threads = []
    times = []

    # Read CSV file
    with open(csv_path, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            threads.append(int(row['threads']))
            times.append(int(row['avg_ms']))

    # Plotting
    plt.figure(figsize=(8, 5))
    plt.plot(threads, times, marker='o', linestyle='-')
    plt.xticks(threads)
    plt.xlabel('Number of Threads')
    plt.ylabel('Average Filter Time (ms)')
    plt.title('Gaussian Filter Performance Scaling')
    plt.grid(True)

    # Save plot
    output_file = 'bench_results/plot.png'
    plt.tight_layout()
    plt.savefig(output_file)
    print(f'Plot saved to {output_file}')

if __name__ == '__main__':
    main() 