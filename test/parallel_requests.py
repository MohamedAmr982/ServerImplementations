import requests
from multiprocessing import Process
import random
import time
from sys import argv

def send_requests(n, ports):
    BASE_URL = f"http://localhost:"
    files = [
        "hello.html",
        "hello_16.html",
        "server.jpg",
        "abc.html"
    ]

    durations = []

    for i in range(n):
        file = random.choice(files)
        port = random.choice(ports)
        url = BASE_URL + port + "/" + file
        
        t1 = time.monotonic_ns()
        requests.get(url)
        t2 = time.monotonic_ns() - t1

        durations.append(t2)

    avg_duration_ms = (sum(durations) / len(durations)) * 1e-6
    print(f"Average response time: {avg_duration_ms} ms.")

def send_parallel_requests(n_proc, n_req, ports):
    processes = [Process(
        target=lambda: send_requests(n_req, ports)) for x in range(n_proc)]
    for p in processes:
        p.start()
    for p in processes:
        p.join()

if __name__ == "__main__":
    if len(argv) < 5:
        print("Usage: python3 parallel_requests.py <n_processes> <n_requests_per_process> <n_ports> <target_ports>")
        exit(1)
    n_proc = int(argv[1])
    n_req = int(argv[2])
    n_ports = argv[3]
    ports = argv[4:]
    send_parallel_requests(n_proc, n_req, ports)
