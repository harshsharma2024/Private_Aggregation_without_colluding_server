import requests
import os

SERVER_URL = "http://localhost:8000"
ng = 16384

# Round 1: Send pk (16 bytes)
pk = os.urandom(16)  # dummy 16-byte value
print("Round 1: Sending pk...")
r1 = requests.post(SERVER_URL, data=pk) # Receiving HTML + pk_list
print("Received response (Round 1) of size:", len(r1.content)/1024, "KB")


# Round 2: Send polynomial solutions (dynamic size)
poly_solutions = os.urandom(ng * 16)  # simulate n_g * 16 bytes
print("Round 2: Sending polynomial solutions of size:", len(poly_solutions)/1024, "KB")
r2 = requests.post(SERVER_URL, data=poly_solutions)
print("Server response (Round 2):", r2.text[:200], "...")

