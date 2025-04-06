#include<bits/stdc++.h>

#include <sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

__int128 str_to_int128(const std::string& s) {
    __int128 result = 0;
    for (char c : s) {
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        }
    }
    return result;
}

void print128(__int128 x) {
    if (x == 0) {
        std::cout << "0";
        return;
    }
    if (x < 0) {
        std::cout << "-";
        x = -x;
    }
    std::string out;
    while (x > 0) {
        out += '0' + x % 10;
        x /= 10;
    }
    std::reverse(out.begin(), out.end());
    std::cout << out;
}

// Modular exponentiation
__int128 modpow(__int128 base, __int128 exp, __int128 mod) {
    __int128 result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}


class NTT {
    public:
        __int128 p; // prime modulus
        __int128 g; // primitive root
    
        NTT(__int128 prime, __int128 root) : p(prime), g(root) {}
    
        // Modular inverse using Fermat’s little theorem
        __int128 modinv(__int128 a, __int128 mod) {
            return modpow(a, mod - 2, mod);
        }
    
        // Recursive Cooley-Tukey NTT
        std::vector<__int128> ntt_cooley_tukey(const std::vector<__int128>& x, __int128 p, __int128 g) {
            size_t N = x.size();
            if (N == 1)
                return x;
    
            std::vector<__int128> evenPart(N / 2), oddPart(N / 2);
            for (size_t i = 0; i < N / 2; ++i) {
                evenPart[i] = x[2 * i];
                oddPart[i] = x[2 * i + 1];
            }
    
            evenPart = ntt_cooley_tukey(evenPart, p, (g * g) % p);
            oddPart = ntt_cooley_tukey(oddPart, p, (g * g) % p);
    
            std::vector<__int128> result(N);
            __int128 factor = 1;
    
            for (size_t i = 0; i < N / 2; ++i) {
                __int128 term = (factor * oddPart[i]) % p;
                result[i] = (evenPart[i] + term) % p;
                result[i + N / 2] = (evenPart[i] - term + p) % p;
                factor = (factor * g) % p;
            }
    
            return result;
        }
    
        // Inverse NTT
        std::vector<__int128> inverse_ntt(const std::vector<__int128>& x, __int128 p, __int128 g) {
            size_t N = x.size();
            __int128 g_inv = modinv(g, p);
            std::vector<__int128> y = ntt_cooley_tukey(x, p, g_inv);
            __int128 inv_N = modinv(N, p);
            for (size_t i = 0; i < N; ++i) {
                y[i] = (y[i] * inv_N) % p;
            }
            return y;
        }
    };
    
class PrimeHelper {
    public:
        // Check if number is prime
        bool is_prime(__int128_t num) {
            if (num < 2) return false;
            if (num == 2) return true;
            if (num % 2 == 0) return false;
    
            for (__int128_t i = 3; i * i <= num; i += 2) {
                if (num % i == 0)
                    return false;
            }
            return true;
        }
    
        // Find smallest prime p such that p ≡ 1 mod n
        __int128_t find_prime_for_polynomial(__int128_t n, __int128_t start) {
            __int128_t p = start;
            while (true) {
                if (is_prime(p) && (p - 1) % n == 0) {
                    return p;
                }
                --p;
            }
        }
    
        // Return set of prime factors of n
        std::set<__int128_t> prime_factors(__int128_t n) {
            std::set<__int128_t> factors;
            for (__int128_t d = 2; d * d <= n; ++d) {
                while (n % d == 0) {
                    factors.insert(d);
                    n /= d;
                }
            }
            if (n > 1)
                factors.insert(n);
            return factors;
        }
    
        // Check if a is a primitive root mod p
        bool is_primitive_root(__int128_t a, __int128_t p) {
            std::set<__int128_t> factors = prime_factors(p - 1);
            for (auto q : factors) {
                if (modpow(a, (p - 1) / q, p) == 1)
                    return false;
            }
            return true;
        }
    
        // Find a primitive n-th root of unity mod p
        __int128_t find_primitive_nth_root(__int128_t p, __int128_t n) {
            if ((p - 1) % n != 0)
                throw std::invalid_argument("n does not divide p-1");
    
            __int128_t exponent = (p - 1) / n;
            for (__int128_t a = 2; a < p; ++a) {
                if (is_primitive_root(a, p)) {
                    __int128_t omega = modpow(a, exponent, p);
                    if (omega != 1 && modpow(omega, n, p) == 1) {
                        bool is_primitive = true;
                        for (__int128_t k = 1; k < n; ++k) {
                            if (modpow(omega, k, p) == 1) {
                                is_primitive = false;
                                break;
                            }
                        }
                        if (is_primitive)
                            return omega;
                    }
                }
            }
            throw std::runtime_error("No primitive n-th root found");
        }
    
};




void test(){
    string s = "123456786789012345678901234567890";
    __int128 x = str_to_int128(s);
    vector<__int128> a = {1, 2, 76, 40, 74389, 232,2,342};
    PrimeHelper ph;
    __int128 p = ph.find_prime_for_polynomial(a.size(), 1000000007);
    __int128 g = ph.find_primitive_nth_root(p, a.size());
    NTT ntt(p, g);
    vector<__int128> ntt_result = ntt.ntt_cooley_tukey(a, p, g);
    vector<__int128> inv_ntt_result = ntt.inverse_ntt(ntt_result, p, g);
    cout << "Original: ";
    print128(x);
    cout << "\nNTT Result: ";
    for (auto val : ntt_result) {
        print128(val);
        cout << " ";
    }
    cout << "\nInverse NTT Result: ";
    for (auto val : inv_ntt_result) {
        print128(val);
        cout << " ";
    }
    cout << endl;
}

pair<__int128,__int128> process_received_pk(char* buffer){
    string s(buffer);
    __int128 pos = s.find(" ");
    if(pos == string::npos) return {0, 0};
    __int128 pk = stoi(s.substr(pos+1));
    cout << "Received pk: ";
    print128(pk);
    cout << endl;
    return {1, pk};
}

int main(){
    __int128 server_fd, max_sd, new_socket;
    sockaddr_in server_addr;
    __int128 addrlen = sizeof(server_addr);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8111);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    cout << "Server is listening on port 8111..." << endl;

    fd_set readfds;
    std::vector<__int128> client_sockets;

    map<__int128, pair<__int128,__int128>> client_map;
    __int128 client_id = 0;
    while (true) {
        // Clear and set master + client sockets
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (__int128 sd : client_sockets) {
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        // 5. Wait for activity
        __int128 activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            perror("select error");
            continue;
        }

        // 6. New incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            new_socket = accept(server_fd, (struct sockaddr*)&server_addr, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                perror("accept");
                continue;
            }

            std::cout << "New connection: socket fd = ";
            print128(new_socket);
            cout<<endl;
            client_sockets.push_back(new_socket);
            client_map[new_socket] = {client_id, -1};
            client_id++;
            std::cout << "Client ID: ";
            print128(client_map[new_socket].first);
            std::cout << std::endl;
        }

        // 7. IO operation on other sockets
        for (size_t i = 0; i < client_sockets.size(); ++i) {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds)) {
                char buffer[1024] = {0};
                int valread = read(sd, buffer, sizeof(buffer));

                if (valread == 0) {
                    // Client disconnected
                    std::cout << "Client disconnected: socket fd = " << sd << std::endl;
                    close(sd);
                    client_sockets.erase(client_sockets.begin() + i);
                    --i;
                } else {
                    std::cout << "Received from fd " << sd << ": " << buffer << std::endl;
                    auto t = process_received_pk(buffer);
                    if(t.first == 0){
                        if(client_map.find(sd) != client_map.end()){
                            client_map.erase(sd);
                            cout<<"Invalid packet received, client removed"<<endl;
                        }
                        close(sd);
                        cout << "Invalid packet received" << endl;
                        continue;
                    }
                    client_map[sd].second = t.second;
                    cout << "Client ID: ";
                    print128(client_map[sd].first);
                    cout << ", pk: ";
                    print128(client_map[sd].second);
                    cout<< endl;
                    string ok_message = "OK";
                    send(sd, ok_message.c_str(), ok_message.size(), 0); // Echo
                }
            }
        }
    }


    close(server_fd);

    return 0;

}