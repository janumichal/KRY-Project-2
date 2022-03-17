#include <iostream>
#include <getopt.h>
#include <gmpxx.h>

using namespace std;

#define ARGUMENT_ERROR 1
#define PUBLIC_EXPONENT 3

unsigned int bits_size(mpz_t num){
    return mpz_sizeinbase(num, 2);
}

void gen_in_range(mpz_t res, int from, mpz_t to){
    unsigned long int counter = 0;
    while(true){
        unsigned long seed;
        gmp_randstate_t state;
        gmp_randinit_mt(state);
        seed = time(nullptr)+counter;
        gmp_randseed_ui(state, seed);
        mpz_urandomm(res, state, to);
        gmp_randclear(state);
        if(mpz_cmp_ui(res, from) > 0){
            break;
        }
        counter++;
    }
    return;
}

bool miller_test(mpz_t d, mpz_t n){
    mpz_t a, x, n_1, n_2;
    mpz_init(a);
    mpz_init(x);
    mpz_init(n_1);
	mpz_init(n_2);
    
    // Pick a random number in [2..n-2]
    // Corner cases make sure that n > 4
    // int a = 2 + rand() % (n - 4);
	mpz_sub_ui(n_2, n, 2);
    gen_in_range(a, 1, n_2);
	mpz_clear(n_2);
 
    // Compute a^d % n
    // int x = power(a, d, n);
    mpz_powm(x, a, d, n);
 
    // if (x == 1  || x == n-1)
    mpz_sub_ui(n_1, n, 1);
    if(mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, n_1) == 0){
        mpz_clear(n_1);
        mpz_clear(a);
        mpz_clear(x);
        return true;
    }
    
    // Keep squaring x while one of the following doesn't
    // happen
    // (i)   d does not reach n-1
    // (ii)  (x^2) % n is not 1
    // (iii) (x^2) % n is not n-1
    // while (d != n-1)
    while(mpz_cmp(d, n_1) != 0){

    //     x = (x * x) % n;
        mpz_mul(x, x, x);
        mpz_mod(x, x, n);
    //     d *= 2;
        mpz_mul_ui(d, d, 2);
    //     if (x == 1)      return false;
        if(mpz_cmp_ui(x, 1) == 0){
            mpz_clear(n_1);
            mpz_clear(a);
            mpz_clear(x);
            return false;
        }
    //     if (x == n-1)    return true;
        if(mpz_cmp(x, n_1) == 0){
            mpz_clear(n_1);
            mpz_clear(a);
            mpz_clear(x);
            return true;
        }
    }

    mpz_clear(n_1);
    mpz_clear(a);
    mpz_clear(x);
 
    // Return composite
    return false;
}

bool is_prime(mpz_t n, unsigned int k){
    mpz_t d, d_mod;
    mpz_init(d);
    mpz_init(d_mod);

    // Corner cases
    // if (n <= 1 || n == 4)  return false;
    if(mpz_cmp_ui(n, 1) <= 0 || mpz_cmp_ui(n, 4) == 0){
        mpz_clear(d);
        mpz_clear(d_mod);
        return false;
    }

    // if (n <= 3) return true;
    if(mpz_cmp_ui(n, 3) <= 0){
        mpz_clear(d);
        mpz_clear(d_mod);
        return true;
    }
 
    // Find r such that n = 2^d * r + 1 for some r >= 1
    // int d = n - 1;
    mpz_sub_ui(d, n, 1);

    // while (d % 2 == 0)
    mpz_mod_ui(d_mod, d, 2);
    while(mpz_cmp_ui(d_mod, 0) == 0){
    //     d /= 2;
        mpz_div_ui(d, d, 2);
		mpz_mod_ui(d_mod, d, 2);
    }

 
    // Iterate given nber of 'k' times
    for (unsigned int i = 0; i < k; i++){
        //  if (!miillerTest(d, n))
        if(!miller_test(d, n)){
            mpz_clear(d);
            mpz_clear(d_mod);   
            return false;
		}
    }
    
    mpz_clear(d);
    mpz_clear(d_mod);
    return true;
}

void gen_prime(mpz_t result, int bits){
    unsigned long int counter = 0;
    unsigned long seed;
    gmp_randstate_t state;
    gmp_randinit_mt(state);
	while(true){
		seed = time(nullptr)+counter;
		gmp_randseed_ui(state, seed);
		mpz_urandomb(result, state, bits);
		mpz_setbit(result, bits-1);
		if(is_prime(result, 1)){
			break;
		}
		counter++;
	}
    gmp_randclear(state);
    return;
}

void gen_pq(mpz_t p, mpz_t q, int bits){
    bool fst = true;
    int p_bits = bits/2;
    int q_bits;
    while(true){
        if(fst){
            gen_prime(p, p_bits);
            fst = false;
            q_bits = bits - bits_size(p);
        }else{
            gen_prime(q, q_bits);
            if(mpz_cmp(q, p) != 0){
                break;
            }
        }
    }
}

void gcd(mpz_t res, mpz_t a, mpz_t b){
    // Everything divides 0
    if (mpz_cmp_ui(a, 0) == 0){
        mpz_set(res, b);
        return;
    }
    if (mpz_cmp_ui(b, 0) == 0){
        mpz_set(res, a);
        return;
    }
  
    if (mpz_cmp(a, b) == 0){
        mpz_set(res, a);
        return;
    }

    if (mpz_cmp(a, b) > 0){
        mpz_t a_b;
        mpz_init(a_b);
        mpz_sub(a_b, a, b);
        gcd(res, a_b, b);
        mpz_clear(a_b);
        return;
    }
    mpz_t b_a;
    mpz_init(b_a);
    mpz_sub(b_a, a, b);
    gcd(res, b_a, b);
    mpz_clear(b_a);
    return;
}

void genereate_keys(int bits){
    cout << "Bits: " << bits << endl;
    mpz_t p, q, n, phi_n, q_1, p_1, e;
    mpz_init(n);
    mpz_init(p);
    mpz_init(q);
    mpz_init(phi_n);
    mpz_init(p_1);
    mpz_init(q_1);
    mpz_init(e);


    // Generate p and q
    gen_pq(p, q, bits);
    // n = p * q
    mpz_mul(n, p, q);
    // (p-1)
    mpz_sub_ui(p_1, p, 1);
    // (q-1)
    mpz_sub_ui(q_1, q, 1);
    // phi(n) = (q-1)*(p-1)
    mpz_mul(phi_n, p_1, q_1);
    // choose e that 1 < e < phi(n)
    gen_in_range(e, 1, phi_n);


    gmp_printf("P: %Zd (%d) ", p, bits_size(p));
    // mpz_out_str(stdout, 2, p); cout << endl;
    gmp_printf("Q: %Zd (%d) ", q, bits_size(q));
    // mpz_out_str(stdout, 2, q); cout << endl;
    gmp_printf("N: %Zd (%d) \n", n, bits_size(n));
    gmp_printf("(p-1): %Zd\n", p_1);
    gmp_printf("(q-1): %Zd\n", q_1);
    gmp_printf("phi(n): %Zd\n", phi_n);
    gmp_printf("E: \t%Zd\n", e);

    // TODO add is_prime function
    // TODO add gcd


    mpz_clear(e);
    mpz_clear(phi_n);
    mpz_clear(p_1);
    mpz_clear(q_1);
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);


}

void encrypt(){
    cout << "Encrypting msg" << endl;
}

void decrypt(){
    cout << "Decrypting msg" << endl;
}

void factorization(){
    cout << "Cracking the private key" << endl;
}

void printHelp(){
    cout << "kry [options]" << endl <<
    "Options:" << endl <<
    "-h                     Print this help message" << endl <<
    "-g B                   Generates keys" << endl <<
    "-e E N M               Encrypts message" << endl <<
    "-d D N C               Decrypts message" << endl <<
    "-b N                   Cracks RSA" << endl << endl <<
    "Parameters:" << endl <<
    "B                      Modulus size in bits" << endl <<
    "E                      Public exponent" << endl <<
    "D                      Private exponent" << endl <<
    "N                      Public modulus" << endl <<
    "M                      Open message" << endl <<
    "C                      Encrypted message" << endl;    
}


int main(int argc, char** argv){
    static struct option opts[] = {
        {"g", required_argument, nullptr, 'g'},
        {"e", required_argument, nullptr, 'e'},
        {"d", required_argument, nullptr, 'd'},
        {"b", required_argument, nullptr, 'b'},
        {"h", no_argument, nullptr, 'h'},
        {0, 0, nullptr, 0}
    };

    int opt, idx = 0;
    while((opt = getopt_long_only(argc, argv, "", opts, &idx)) != EOF){
        switch(opt){
            case 'g':
                genereate_keys(stoi(optarg));
                break;
            case 'e':
                cout << "E " << optarg << endl;
                break;
            case 'd':
                cout << "D " << optarg << endl;
                break;
            case 'b':
                cout << "B " << optarg << endl;
                break;
            case 'h':
                printHelp();
                break;
            default:
                cout << "Unsupported argument!" << endl;
                exit(ARGUMENT_ERROR);

        }
    }
    return 0;
}