// Here in this file we explicitly calculate time at the user end for various calculations
// 1. Mask generation, polynomial evaluation per user

#include "utils.h"

const int grp_sz = 16384; // # of grps
const int part_size = 100; // # partition size , number of clients parallely sending request to server

void client_mask_creation(int self_ind, int num_times){

    double coml_time = 0;

    PrimeHelper prime_helper;
    int128 prime = prime_helper.find_prime_for_polynomial(grp_sz, MAX_PRIME_VAL);
    int128 primitive_root = prime_helper.find_primitive_nth_root(prime, grp_sz);
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int128> dis(0, N_G - 1);
    uniform_int_distribution<int128> dis2(0, prime - 1);

    double pk_list_sz = 0;

    for(int i = 0;i<num_times;i++){
        

        __int128 gid1 = dis(gen), sk1 = dis2(gen);

        int128 pk1 = modpow(primitive_root, sk1, prime);

        User u1(gid1, sk1, pk1, grp_sz,primitive_root, prime);\

        vector<int128>recd_pk_lst(part_size, pk1);

        for(int i = 0;i<part_size;i++){
            if(i!=self_ind){
                int128 ski = dis2(gen);
                recd_pk_lst[i] = modpow(primitive_root, ski, prime);
            }
        }

        chrono::high_resolution_clock::time_point start, end;

        start = high_resolution_clock::now();

        u1.create_mask(recd_pk_lst, part_size, self_ind);

        u1.get_masked_value_representation();

        end = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(end - start).count();
        cout<<duration<<" ";
        coml_time += duration;

        pk_list_sz += recd_pk_lst.size();

    }

    cout<<endl;

    coml_time/=num_times;

    cout<<"Time taken(ms) for mask generation for client with partition size "<<part_size<<" and group size "<<grp_sz<<" : "<<coml_time<<endl;
    cout<<"Size of the pk list that has to be sent: "<<(16*(pk_list_sz/num_times)/(1024*1024))<<" MB"<<endl;

}


void server_aggregation_mask(User &user, int128 curr_ind, vector<int128>&pk_list, vector<vector<int128>>&mask_vector){
    user.create_mask(pk_list, part_size, curr_ind);
    mask_vector.push_back(user.get_masked_value_representation());

    return;
}

void serve_total_time(int iterations){

    PrimeHelper prime_helper;
    int128 prime = prime_helper.find_prime_for_polynomial(grp_sz, MAX_PRIME_VAL);
    int128 primitive_root = prime_helper.find_primitive_nth_root(prime, grp_sz);
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int128> dis(0, N_G - 1);
    uniform_int_distribution<int128> dis2(0, prime - 1);

    // Creating the masks, we assume that we will receive these masks from the clients and we will take the latency to receive into account during the network latenncy part\

    double compl_time = 0;

    double mask_lst_sz = 0;

    for(int it = 0;it<iterations;it++){
        // creating part_size users
        vector<int128>pk_lst;
        vector<User>user_lst;
        for(int i = 0;i<part_size;i++){
            int128 sk = dis2(gen);
            int128 pk = modpow(primitive_root, sk, prime);
            int128 gid = dis(gen);

            User user(gid, sk, pk, grp_sz, primitive_root, prime);
            user_lst.push_back(user);
            pk_lst.push_back(pk);
        }

        vector<vector<int128>>mask_vector;

        for(int128 i = 0;i<part_size;i++){
            server_aggregation_mask((user_lst[i]),i, (pk_lst),(mask_vector));
        }

        mask_lst_sz += mask_vector[0].size();

        // cout<<"Size of the mask vector : "<<(mask_vector.size())<<endl;

        if(mask_vector.size() != part_size){
            cout<<"Size do not match, check !!"<<endl;
        }
        for(auto i : mask_vector){
            if((i.size())!= grp_sz){
                cout<<"Issue at id "<<endl;
            }
        }

        high_resolution_clock::time_point start, end;

        start = high_resolution_clock::now();

        vector<int128>net_mask(grp_sz, 0);

        for(int i = 0;i<part_size;i++){
            for(int j = 0;j<grp_sz;j++){
                (net_mask[j] += mask_vector[i][j])%=prime;
            }
        }

        NTT ntt(primitive_root, prime);

        auto res = ntt.inverse_ntt(net_mask, prime, primitive_root);

        end = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(end - start).count();

        cout<<duration<<" ";

        int128 sum = 0;
        for(auto val : res){
            sum+=val;
        }

        if(sum != part_size){
            cout<<"Some issue in INTT !!!"<<endl;
        }
        // else{
        //     cout<<"Some issue in INTT"<<endl;
        // }

        compl_time += duration;
    }

    cout<<endl;
    
    compl_time /= iterations;

    mask_lst_sz /= iterations;

    cout<<"time taken by server for mask aggregation and polynomial generation for partition size "<<part_size<<" group size "<<grp_sz<<" : "<<compl_time<<endl;
    cout<<"size of the mask list to be sent from clien to server : "<<(mask_lst_sz*16/(1024*1024))<<" MB"<<endl;

}


int main(){
    client_mask_creation(1, 10);

    serve_total_time(10);
}

// then server time evaluation of summing up all the polynomial solution, and doing INTT


