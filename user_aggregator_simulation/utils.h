#include<bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <random>
#include <chrono>
#include <openssl/evp.h>
using namespace std;
using namespace chrono;



// TODO : remove dependence on this N_G
#define N_G 16384 // Power of 2
// #define P_S 4 // Even number of users at a time
#define PK_LST_SIZE 32768
#define MAX_PRIME_VAL 999999999999 // Vary depending on the need (N_G)
using int128 = __int128;
// Modular exponentiation

bool is_masked_print = 1, is_pk_lst_print = 1;

struct time_ds{
    double prg_time;
    double mask_gen_time;
    double network_time;
    double total_time;
    double mask_gen_evaluation_time;
    double pk_lst_extraction_time;

    time_ds(){
        prg_time = 0;
        mask_gen_time = 0;
        network_time = 0;
        total_time = 0;
        mask_gen_evaluation_time = 0;
        pk_lst_extraction_time = 0;
    }
};

time_ds time_data_glob;

int128 modpow(__int128 base, __int128 exp, __int128 mod) {
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

__int128 str_to_int128(const string& s) {
    __int128 result = 0;
    for (char c : s) {
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        }
    }
    return result;
}

void int128_to_bytes(__int128 seed, unsigned char* out) {
    for (int i = 0; i < 16; ++i) {
        out[15 - i] = static_cast<unsigned char>(seed & 0xFF);
        seed >>= 8;
    }
}

vector<__int128> PRG(__int128 seed, __int128 prime) {
    const int bytes_needed = N_G * 16;
    vector<unsigned char> output(bytes_needed, 0);

    unsigned char key[16];
    unsigned char iv[16] = {0};

    int128_to_bytes(seed, key);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!ctx) {
        cerr << "Failed to create EVP context" << endl;
        exit(1);
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv) != 1) {
        cerr << "Failed to initialize AES-CTR" << endl;
        exit(1);
    }

    int outlen = 0;
    if (EVP_EncryptUpdate(ctx, output.data(), &outlen, output.data(), bytes_needed) != 1) {
        cerr << "Encryption failed" << endl;
        exit(1);
    }

    EVP_CIPHER_CTX_free(ctx);

    vector<__int128> final_res;

    for (int i = 0; i < N_G; ++i) {
        __int128 res = 0;
        for (int j = 0; j < 16; ++j) {
            res = (res << 8) | output[i * 16 + j];
        }

        // Ensure non-negative and mod prime
        if (res < 0) res = -res;
        final_res.push_back(res % prime);
    }

    return final_res;
}

void print128(__int128 x) {
    if (x == 0) {
        cout << "0";
        return;
    }
    if (x < 0) {
        cout << "-";
        x = -x;
    }
    string out;
    while (x > 0) {
        out += '0' + x % 10;
        x /= 10;
    }
    reverse(out.begin(), out.end());
    cout << out;
}

void exit_the_process(int sockfd) {
    close(sockfd);
    //cout << "Exiting the process..." << endl;
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

class User{
    private:
        int128 gid;
        int128 sk;
        int128 mask;
        vector<int128> masked_lst;
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
            //cout << "pk_list size does not match p_s" << endl;
            return -1;
        }
        if(index_self_pk >= pk_list.size()) {
            //cout << "index_self_pk out of bounds" << endl;
            return -1;
        }

        chrono::high_resolution_clock::time_point start, end;

        this->masked_lst.clear();
        this->masked_lst.resize(this->n_g, 0);
        int128 odd = index_self_pk%2;
        this->mask = 0;
        int128 cnt = 0;
        for(int128 i = 0;i< p_s;i++){
            int128 sign = (odd+cnt+1)%2==0?1:-1;
            if(i != index_self_pk){
                start = high_resolution_clock::now();
                auto prg_res =  PRG((modpow(pk_list[i], this->sk,this->prime)), this->prime);
                end = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(end - start);
                time_data_glob.prg_time += duration.count();
                for(int128 j = 0;j< this->n_g;j++){
                    this->masked_lst[j] = (this->masked_lst[j] + sign*prg_res[j])%this->prime;
                    this->masked_lst[j] = (this->masked_lst[j] + this->prime)%this->prime;
                }
                cnt++;
            }
            this->mask = (this->mask + this->prime)%this->prime;
        }

        return 0;
    }

    vector<int128> get_masked_value_representation(){
        vector<int128> masked_value_representation;
        if(this->mask == -1){
            //cout << "Mask not created" << endl;
            return masked_value_representation;
        }
        int128 value_point = 1;


        for(int128 i = 0;i< this->n_g; i++){
            masked_value_representation.push_back((this->masked_lst[i] + modpow(value_point,this->gid, this->prime))%this->prime);
            value_point = (value_point * this->primitive_root)%this->prime;
        }

        return masked_value_representation;

    }
    
};

// pair<int128, vector<int128>> extract_pk_list(char* buffer){
//     vector<string> tokens;
//     string s(buffer);
//     string temp = "";
//     for(auto i : s){
//         if(i == ' '){
//             tokens.push_back(temp);
//             temp = "";
//         } else {
//             temp += i;
//         }
//     }
//     if(!temp.empty()){
//         tokens.push_back(temp);
//     }
//     //cout<<"tokens size: "<<tokens.size()<<endl;
//     vector<int128> pk_list;
//     if(tokens.size() < 3 || tokens[0] != "pk_list"){
//         //cout << "Invalid pk list" << endl;
//         return {-1,pk_list};
//     }

//     int sz = tokens.size();
//     for(int i = 2; i < sz-1; i++){
//         pk_list.push_back(str_to_int128(tokens[i]));
//     }
//     int128 pk_sz = str_to_int128(tokens[1]);
//     if(sz-3 != pk_sz){
//         //cout << "pk list size does not match" << endl;
//         return {-1,{}};
//     }
//     int128 index_self_pk = str_to_int128(tokens[sz-1]);
//     return {index_self_pk, pk_list};
// }

int128 bytes_to_int128(const uint8_t* data) {
    int128 val = 0;
    for (int i = 0; i < 16; i++) {
        val <<= 8;
        val |= data[i];
    }
    return val;
}

int32_t bytes_to_int32(const uint8_t* data) {
    int32_t val = 0;
    for (int i = 0; i < 4; i++) {
        val <<= 8;
        val |= data[i];
    }
    return val;
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
