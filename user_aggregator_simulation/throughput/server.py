from http.server import BaseHTTPRequestHandler, HTTPServer
import os, time

part_sz = 100
# Keep track of client states (in-memory, per client IP)
client_state = {}

class Handler(BaseHTTPRequestHandler):
    def do_POST(self):
        client_ip = self.client_address[0]
        content_length = int(self.headers['Content-Length'])
        body = self.rfile.read(content_length)

        if client_ip not in client_state:
            client_state[client_ip] = {"round": 1}

        round_no = client_state[client_ip]["round"]

        if round_no == 1:
            print("Round 1: Received pk:", body[:16].hex(), "...")  # print first 16 bytes
            # mark start time
            client_state[client_ip]["start_time"] = time.time()

            # Server responds with webpage + pk_list
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()

            with open("static/index.html", "rb") as f:
                self.wfile.write(f.read())

            # Generate pk_list
            pk = os.urandom(16 * part_sz)
            print("Sending pk list of size:", len(pk)/1024, "KB")

            self.wfile.write(pk)
            client_state[client_ip]["round"] = 2

        elif round_no == 2:
            elapsed = time.time() - client_state[client_ip]["start_time"]
            print("Round 2: Received polynomial solutions of size:",
                  content_length / 1024.0**2, "MB")
            print(f"Completed one round of communication in {elapsed:.4f} seconds")

            # Send ack
            self.send_response(200)
            self.send_header("Content-type", "text/plain")
            self.end_headers()
            self.wfile.write(b"Server received polynomial solutions")

            # Reset state so client can do another round
            client_state[client_ip]["round"] = 1


def run():
    with HTTPServer(("", 8000), Handler) as server:
        print("Server running at http://localhost:8000/")
        server.serve_forever()

if __name__ == "__main__":
    run()
