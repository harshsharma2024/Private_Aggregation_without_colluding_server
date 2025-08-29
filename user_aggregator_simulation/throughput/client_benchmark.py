import requests, os, time
import numpy as np
import matplotlib.pyplot as plt

SERVER_URL = "http://localhost:8000"

def run_one_round(n_g=1024, part_sz=100):
    # Round 1: Send pk (16 bytes)
    pk = os.urandom(16)
    start = time.time()
    r1 = requests.post(SERVER_URL, data=pk)
    
    # Extract webpage + pk_list (we just discard them here)
    _ = r1.content

    # Round 2: Send polynomial solutions
    poly_solutions = os.urandom(n_g * 16)
    r2 = requests.post(SERVER_URL, data=poly_solutions)
    _ = r2.text
    end = time.time()

    return end - start


def benchmark(iterations=100, n_g=1024, part_sz=100):
    times = []
    for i in range(iterations):
        t = run_one_round(n_g=n_g, part_sz=part_sz)
        times.append(t)
        print(f"Iteration {i+1}/{iterations}: {t:.4f} s")

    times = np.array(times)
    print("\n=== Benchmark Results ===")
    print(f"Mean: {times.mean():.4f} s")
    print(f"Std : {times.std():.4f} s")
    print(f"Min : {times.min():.4f} s")
    print(f"Max : {times.max():.4f} s")

    # Plot histogram
    plt.hist(times, bins=20, edgecolor='black')
    plt.xlabel("Round-trip time (s)")
    plt.ylabel("Frequency")
    plt.title("Distribution of Round-Trip Times")
    plt.show()


if __name__ == "__main__":
    benchmark(iterations=50, n_g=1024)  # run 50 full cycles
