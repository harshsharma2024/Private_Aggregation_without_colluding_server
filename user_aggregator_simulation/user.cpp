#include<bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;


using int128 = __int128;
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

__int128 str_to_int128(const std::string& s) {
    __int128 result = 0;
    for (char c : s) {
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        }
    }
    return result;
}

int128 PRG(int128 x) {
    // TODO : Implement a proper Pseudo Random Generator
    return x;
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

void exit_the_process(int sockfd) {
    close(sockfd);
    cout << "Exiting the process..." << endl;
    exit(0);
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

#define N_G 8 // Power of 2
#define P_S 2 // Even number of users at a time

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

class User{
    private:
        int128 gid;
        int128 sk;
        int128 mask;
    public:
        int128 pk;
        int128 n_g;
        int128 primitive_root;
        int128 prime;

    User(int128 gid, int128 sk, int128 pk, int128 n_g, int128 primitive_root, int128 prime): 
        gid(gid), sk(sk), pk(pk), n_g(n_g), primitive_root(primitive_root), prime(prime) {
            this->mask = -1;
        }

    int create_mask(vector<int128> &pk_list, int128 p_s, int128 index_self_pk){
        if(pk_list.size() != p_s) {
            cout << "pk_list size does not match p_s" << endl;
            return -1;
        }
        if(index_self_pk >= pk_list.size()) {
            cout << "index_self_pk out of bounds" << endl;
            return -1;
        }
        int128 odd = index_self_pk%2;
        this->mask = 0;
        int128 cnt = 0;
        for(int128 i = 0;i< p_s;i++){
            int128 sign = (odd+cnt+1)%2==0?1:-1;
            if(i != index_self_pk){
                this->mask = (this->mask + sign*PRG(modpow(pk_list[i], this->sk,this->prime)))% this->prime;
                cnt++;
            }
            this->mask = (this->mask + this->prime)%this->prime;
        }
        cout<< "Mask created: ";
        // print128(odd);
        // cout<<" ";
        print128(this->mask);
        cout<< endl;
        return 0;
    }

    vector<int128> get_masked_value_representation(){
        vector<int128> masked_value_representation;
        if(this->mask == -1){
            cout << "Mask not created" << endl;
            return masked_value_representation;
        }
        int128 value_point = 1;

        for(int128 i = 0;i< this->n_g; i++){
            masked_value_representation.push_back((this->mask + modpow(value_point,this->gid, this->prime))%this->prime);
            value_point = (value_point * this->primitive_root)%this->prime;
        }

        return masked_value_representation;

    }
    
};

pair<int128, vector<int128>> extract_pk_list(char* buffer){
    vector<string> tokens;
    string s(buffer);
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
    vector<int128> pk_list;
    if(tokens.size() < 3 || tokens[0] != "pk_list"){
        cout << "Invalid pk list" << endl;
        return {-1,pk_list};
    }

    int sz = tokens.size();
    for(int i = 2; i < sz-1; i++){
        pk_list.push_back(str_to_int128(tokens[i]));
    }
    int128 pk_sz = str_to_int128(tokens[1]);
    if(sz-3 != pk_sz){
        cout << "pk list size does not match" << endl;
        return {-1,{}};
    }
    int128 index_self_pk = str_to_int128(tokens[sz-1]);
    return {index_self_pk, pk_list};
}

int main() {
    int sockfd;
    sockaddr_in server_addr{};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8111);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // localhost

    if (connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    PrimeHelper prime_helper;
    int128 prime = prime_helper.find_prime_for_polynomial(N_G, 9999999);
    // int128 prime = 17;
    int128 primitive_root = prime_helper.find_primitive_nth_root(prime, N_G);

    cout << "Prime: ";
    print128(prime);
    cout << ", Primitive root: ";
    print128(primitive_root);
    cout << endl;

    int128 gid = 5;
    int128 sk = 28761337619;
    int128 pk = modpow(primitive_root, sk, prime);
    User user(gid, sk, pk, N_G, primitive_root, prime);

    string message = "Hello ";
    message += to_string(pk);
    send(sockfd, message.c_str(), message.size(), 0);
    
    char buffer[1024] = {0};
    int bytes_read = read(sockfd, buffer, sizeof(buffer));
    std::cout << "Server reply: " << std::string(buffer, bytes_read) << "\n";

    if(strcmp(buffer, "OK") == 0){
        cout << "OK received" << endl;
    } else {
        cout << "Invalid response" << endl;
        exit_the_process(sockfd);
        return 1;
    }

    char recv_pk_list[1024] = {0};
    int bytes_read_pk_list = read(sockfd, recv_pk_list, sizeof(recv_pk_list));
    std::cout << "Received pk list: " << std::string(recv_pk_list, bytes_read_pk_list) << "\n";

    auto res_pk_list = extract_pk_list(recv_pk_list);

    if(res_pk_list.second.size() == 0 || res_pk_list.first == -1){
        cout << "Invalid pk list" << endl;
        exit_the_process(sockfd);
        return 1;
    }
    auto pk_list = res_pk_list.second;
    int128 p_s = pk_list.size();
    int128 index_self_pk = res_pk_list.first;

    cout<<"Self pk index: ";
    print128(index_self_pk);
    cout<<endl;

    int stats = user.create_mask(pk_list, p_s, index_self_pk);
    if(stats != 0){
        cout << "Error creating mask" << endl;
        exit_the_process(sockfd);
        return 1;
    }
    vector<int128> masked_value_representation = user.get_masked_value_representation();
    if(masked_value_representation.size() == 0){
        cout << "Error getting masked value representation" << endl;
        exit_the_process(sockfd);
        return 1;
    }

    for(int128 i = 0; i < masked_value_representation.size(); i++){
        cout << "Masked value representation: ";
        print128(masked_value_representation[i]);
        cout<< endl;
    }

    string value_representation_str = "masked_points ";
    value_representation_str += to_string(masked_value_representation.size());
    
    for(int128 i = 0; i < masked_value_representation.size(); i++){
        value_representation_str += " " + to_string(masked_value_representation[i]);
    }

    send(sockfd, value_representation_str.c_str(), value_representation_str.size(), 0);
    cout << "Sent masked value representation to server" << endl;

    close(sockfd);
    return 0 ;
}
