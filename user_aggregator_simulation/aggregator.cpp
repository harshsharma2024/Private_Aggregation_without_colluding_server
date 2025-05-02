#include<bits/stdc++.h>

#include <sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;


struct time_profile{
    double network_time;
    double intt_ploynomial_time;
    double masking_process_time;

    time_profile(){
        network_time = 0;
        intt_ploynomial_time = 0;
        masking_process_time = 0;
    }

};

time_profile time_data;

// Global variables
const string INVALID_STRING = "INVALID";
const string OK_STRING = "OK";\
int return_val = 0;

#define N_G 128 // Power of 2
#define P_S 140 // Even number of users at a time
#define PK_LST_SIZE 16384 // Vary depending on the need (N_G)
#define MAX_PRIME_VAL 99999999999 // Vary depending on the need (N_G)
__int128 Primitive_root = 1165819352762;
__int128 Prime = 9999999998977;

__int128 str_to_int128(const string& s) {
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
        //cout << "0";
        return;
    }
    if (x < 0) {
        //cout << "-";
        x = -x;
    }
    string out;
    while (x > 0) {
        out += '0' + x % 10;
        x /= 10;
    }
    reverse(out.begin(), out.end());
    //cout << out;
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
        vector<__int128> ntt_cooley_tukey(const vector<__int128>& x, __int128 p, __int128 g) {
            size_t N = x.size();
            if (N == 1)
                return x;
    
            vector<__int128> evenPart(N / 2), oddPart(N / 2);
            for (size_t i = 0; i < N / 2; ++i) {
                evenPart[i] = x[2 * i];
                oddPart[i] = x[2 * i + 1];
            }
    
            evenPart = ntt_cooley_tukey(evenPart, p, (g * g) % p);
            oddPart = ntt_cooley_tukey(oddPart, p, (g * g) % p);
    
            vector<__int128> result(N);
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
        vector<__int128> inverse_ntt(const vector<__int128>& x, __int128 p, __int128 g) {
            size_t N = x.size();
            __int128 g_inv = modinv(g, p);
            // //cout << "g_inv: ";
            // print128(g_inv);
            // //cout<<" ";
            // print128(g);
            // //cout<<" ";
            // print128(p);
            // //cout << endl;
            vector<__int128> y = ntt_cooley_tukey(x, p, g_inv);
            // //cout << "NTT result: ";
            // for(auto i : y){
            //     print128(i);
            //     //cout << " ";
            // }
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
        set<__int128_t> prime_factors(__int128_t n) {
            set<__int128_t> factors;
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
            set<__int128_t> factors = prime_factors(p - 1);
            for (auto q : factors) {
                if (modpow(a, (p - 1) / q, p) == 1)
                    return false;
            }
            return true;
        }
    
        // Find a primitive n-th root of unity mod p
        __int128_t find_primitive_nth_root(__int128_t p, __int128_t n) {
            if ((p - 1) % n != 0)
                throw invalid_argument("n does not divide p-1");
    
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
            throw runtime_error("No primitive n-th root found");
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
    // //cout << "Original: ";
    // print128(x);
    // //cout << "\nNTT Result: ";
    // for (auto val : ntt_result) {
    //     print128(val);
    //     //cout << " ";
    // }
    //cout << "\nInverse NTT Result: ";
    for (auto val : inv_ntt_result) {
        print128(val);
        //cout << " ";
    }
    //cout << endl;
}

pair<__int128,__int128> process_received_pk(char* buffer){
    string s(buffer);
    __int128 pos = s.find(" ");
    if(pos == string::npos) return {0, 0};
    string pk_str = s.substr(0, pos);
    if(pk_str != "Hello") return {0, 0};
    __int128 pk = str_to_int128(s.substr(pos+1));
    // //cout << "Received pk: ";
    // print128(pk);
    // //cout << endl;
    return {1, pk};
}

string to_string(__int128 x) {
    if (x == 0) return "0";
    string result;
    while (x > 0) {
        result += '0' + (x % 10);
        x /= 10;
    }
    reverse(result.begin(), result.end());
    return result;
}

int process_received_masked_value(char* buffer, vector<__int128>& evaluation_points){
    string s(buffer);
    vector<string> tokens;
    string temp = "";
    for(auto i : s){
        if(i == ' '){
            tokens.push_back(temp);
            temp = "";
        } else {
            temp += i;
        }
    }
    if(!temp.empty()){
        tokens.push_back(temp);
    }
    if(tokens.size() < 2 || tokens[0] != "masked_points"){
        //cout << "Invalid masked value packet" << endl;
        return -1;
    }
    int sz = tokens.size();
    int masked_points_sz = str_to_int128(tokens[1]);
    if(sz-2 != masked_points_sz){
        //cout << "Masked points size does not match" << endl;
        return -1;
    }
    if(sz-2 != N_G){
        //cout << "Masked points size does not match N_G" << endl;
        return -1;
    }
    for(int i = 2; i < sz; i++){
        __int128 masked_value = str_to_int128(tokens[i]);
        evaluation_points[i-2] = (masked_value + evaluation_points[i-2]) % Prime;
    }

    // //cout << "Received masked value: ";
    // for(int i = 0; i < N_G; i++){
    //     print128(evaluation_points[i]);
    //     //cout << " ";
    // }
    // //cout << endl;

    return 0;

}

int public_key_broadcast_n_polynomial_generation(map<__int128, pair<__int128,__int128>>& client_map, __int128 server_fd) {
    //cout << "Broadcasting public key and polynomial generation..." << endl;
    // Placeholder for actual implementation
    chrono::high_resolution_clock::time_point start, end;
    start = chrono::high_resolution_clock::now();

    string pk_lst_str = "pk_list ";
    int pk_sz = client_map.size();
    pk_lst_str += to_string(pk_sz);

    for (const auto& client : client_map) {
        __int128 client_fd = client.first;
        __int128 pk = client.second.second;
        pk_lst_str += " " + to_string(pk);
    }

    int ind = 0;
    // //cout << "Broadcasting to clients: " << pk_lst_str << endl;
    for (const auto& client : client_map) {
        __int128 client_fd = client.first;
        // //cout << "Sending to client fd: ";
        // print128(client_fd);
        // //cout << endl;
        string ind_str = " " + to_string(ind);
        ind++;
        return_val = send(client_fd, (pk_lst_str + ind_str).c_str(), pk_lst_str.size()+ind_str.size(), 0);
        if (return_val <= 0) {
            //cout << "Error sending pk list to client fd: ";
            print128(client_fd);
            //cout << endl;
            close(client_fd);
            continue;
        }
    }

    end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    time_data.network_time += duration.count();

    //cout << "Broadcasting complete" << endl;

    // Use Select to wait for client responses

    vector<__int128>evaluation_points(N_G,0);

    fd_set readfds;
    __int128 max_sd = server_fd;
    
    while(true){
        if(client_map.size() == 0){
            //cout << "No clients connected, exiting..." << endl;
            break;
        }
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        
        for (const auto& client : client_map) {
            __int128 client_fd = client.first;
            if (client_fd > 0)
                FD_SET(client_fd, &readfds);
            if (client_fd > max_sd)
                max_sd = client_fd;
        }
        __int128 activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            perror("select error");
            return -1;
        }
        vector<__int128>temp_user_fds;
        for (const auto& client : client_map) {
            __int128 client_fd = client.first;
            if (FD_ISSET(client_fd, &readfds)) {
                start = chrono::high_resolution_clock::now();
                char buffer[PK_LST_SIZE] = {0};
                int valread = read(client_fd, buffer, sizeof(buffer));
                end = chrono::high_resolution_clock::now();
                duration = chrono::duration_cast<chrono::milliseconds>(end - start);
                time_data.network_time += duration.count();
                if (valread == 0) {
                    // //cout << "Client disconnected: socket fd = ";
                    // print128( client_fd);
                    // //cout << endl;
                    // close(client_fd);
                    temp_user_fds.push_back(client_fd);
                    continue;
                } else {
                    // //cout << "Received from client " << buffer << endl;
                    start = chrono::high_resolution_clock::now();
                    int stats = process_received_masked_value(buffer, evaluation_points);
                    end = chrono::high_resolution_clock::now();
                    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
                    time_data.masking_process_time += duration.count();
                    if(stats != 0){
                        //cout << "Error processing masked value" << endl;
                        close(client_fd);
                        continue;
                    }
                }
            }
        }

        start = chrono::high_resolution_clock::now();
        for(auto i : temp_user_fds){
            if(client_map.find(i) != client_map.end()){
                client_map.erase(i);
            }
        }

        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        time_data.network_time += duration.count();
    }

    // //cout << "Sum of Received evaluation points: ";

    // for(int i = 0; i < N_G; i++){
    //     print128(evaluation_points[i]);
    //     //cout << " ";
    // }
    // //cout << endl;
    // //cout << "Polynomial generation..." << endl;

    start = chrono::high_resolution_clock::now();
    NTT ntt(Primitive_root, Prime); // Placeholder for NTT object

    auto res = ntt.inverse_ntt(evaluation_points, Prime, Primitive_root);
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    time_data.intt_ploynomial_time += duration.count();
    //cout << "Inverse NTT result: ";
    __int128 sum = 0;
    for (auto val : res) {
        sum += val;
        print128(val);
        //cout << " ";
    }
    //cout<<endl;
    if(sum == P_S){
        //cout<<"Generated Polynomial verified"<<endl;
    }
    else{
        //cout<<"Generated polynomial Sum did not match, some issue!!"<<endl;
    }

    //cout << endl;
    //cout << "Polynomial generation complete" << endl;
    //cout << "Exiting the process..." << endl;
    return 0;
}

int main(){

    PrimeHelper prime_helper;
    Prime = prime_helper.find_prime_for_polynomial(N_G, MAX_PRIME_VAL);
    Primitive_root = prime_helper.find_primitive_nth_root(Prime, N_G);

    chrono::high_resolution_clock::time_point start, end;
    start = chrono::high_resolution_clock::now();
    __int128 server_fd, max_sd, new_socket;
    sockaddr_in server_addr;
    __int128 addrlen = sizeof(server_addr);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    chrono::high_resolution_clock::time_point main_start, main_end;
    main_start = chrono::high_resolution_clock::now();

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
    //cout << "Server is listening on port 8111..." << endl;
    end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    time_data.network_time += duration.count();
    fd_set readfds;
    vector<__int128> client_sockets;

    map<__int128, pair<__int128,__int128>> client_map;
    __int128 client_id = 0;
    while (true) {
        // Clear and set master + client sockets

        start = chrono::high_resolution_clock::now();


        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (__int128 sd : client_sockets) {
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        time_data.network_time += duration.count();
        
        if(client_map.size() == P_S){
            //cout << "P_S (partition size) clients reached, to do the Aggregation" << endl;
            int stats = public_key_broadcast_n_polynomial_generation(client_map, server_fd);
            if(stats == 0){
                //cout<<"Public key broadcasted and polynomial generation done"<<endl;
            } else {
                //cout<<"Error in broadcasting public key and polynomial generation"<<endl;
                //cout<<"Exiting the process..."<<endl;
                close(server_fd);
                exit(EXIT_FAILURE);
            }
            client_map.clear();
            client_sockets.clear();
            //cout << "Cleared client map and sockets" << endl;
            client_id = 0;
            // continue;
            cout<<"Successfully completed one iteration!!!!"<<endl;
            break;
        }

        start = chrono::high_resolution_clock::now();
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

            // //cout << "New connection: socket fd = ";
            // print128(new_socket);
            // //cout<<endl;
            client_sockets.push_back(new_socket);
            
            // //cout << "Client ID: ";
            // print128(client_map[new_socket].first);
            // //cout << endl;
        }

        // 7. IO operation on other sockets
        for (size_t i = 0; i < client_sockets.size(); ++i) {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds)) {
                char buffer[1024] = {0};
                int valread = read(sd, buffer, sizeof(buffer));

                if (valread == 0) {
                    // Client disconnected
                    //cout << "Client disconnected: socket fd = " << sd << endl;
                    close(sd);
                    client_sockets.erase(client_sockets.begin() + i);
                    --i;
                    if(client_map.find(sd) != client_map.end()){
                        client_map.erase(sd);
                    }
                } else {
                    // //cout << "Received from fd " << sd << ": " << buffer << endl;
                    auto t = process_received_pk(buffer);
                    if(t.first == 0){
                        if(client_map.find(sd) != client_map.end()){
                            client_map.erase(sd);
                            //cout<<"Invalid packet received, client removed"<<endl;
                        }
                        //cout << "Invalid packet received" << endl;
                        return_val = send(sd, INVALID_STRING.c_str(), INVALID_STRING.size(), 0);
                        close(sd);
                        continue;
                    }
                    client_map[sd] = {client_id, t.second};
                    client_id++;
                    // //cout << "Client ID: ";
                    // print128(client_map[sd].first);
                    // //cout << ", pk: ";
                    // print128(client_map[sd].second);
                    // //cout<< endl;
                    send(sd, OK_STRING.c_str(), OK_STRING.size(), 0); // Echo
                }
            }
        }

        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        time_data.network_time += duration.count();
    }


    close(server_fd);
    main_end = chrono::high_resolution_clock::now();
    auto main_duartion = chrono::duration_cast<chrono::milliseconds>(main_end - main_start);
    cout << "Server closed" << endl;
    cout<<"Network time: " << time_data.network_time << " ms" << endl;
    cout<<"Polynomial generation time: " << time_data.intt_ploynomial_time << " ms" << endl;
    cout<<"Masking process time: " << time_data.masking_process_time << " ms" << endl;
    cout<<"Total time: " << main_duartion.count() << " ms" << endl;
    return 0;

}