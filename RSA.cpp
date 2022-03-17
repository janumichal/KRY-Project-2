#include <iostream>
#include <getopt.h>
#include <gmpxx.h>
#include <bitset>

using namespace std;

#define ARGUMENT_ERROR 1
#define PUBLIC_EXPONENT 3

unsigned int bits_size(mpz_class num){
    return mpz_sizeinbase(num.get_mpz_t(), 2);
}

mpz_class gen_in_range(int from, mpz_class to){
    unsigned long int counter = 0;
    mpz_class result;
    gmp_randclass rand(gmp_randinit_mt);

    while(true){
        rand.seed(time(nullptr)+counter);
        result = rand.get_z_range(to);
        if(result >= from){
            break;
        }
        counter++;
    }
    return result;
}

bool miller_test(mpz_class d, mpz_class n){
    mpz_class a, x;
    
    a = gen_in_range(1, n-1);
    mpz_powm(x.get_mpz_t(), a.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
 
    if(x == 1 || x == n-1){
        return true;
    }
    
    while(d != n-1){
        x = (x * x) % n;
        d *= 2;
        if(x == 1){
            return false;
        }
        if(x == n-1){
            return true;
        }
    }
    return false;
}

bool is_prime(mpz_class n, unsigned int k){
    mpz_class d, d_mod;

    // Corner cases
    // if (n <= 1 || n == 4)  return false;
    if(n <= 1 || n == 4){
        return false;
    }

    // if (n <= 3) return true;
    if(n <= 3){
        return true;
    }
 
    // Find r such that n = 2^d * r + 1 for some r >= 1
    // int d = n - 1;
    d = n - 1;

    // while (d % 2 == 0)
    while(d % 2 == 0){
    //     d /= 2;
        d /= 2;
    }

 
    // Iterate given nber of 'k' times
    for (unsigned int i = 0; i < k; i++){
        //  if (!miillerTest(d, n))
        if(!miller_test(d, n)){
            return false;
		}
    }
    return true;
}

mpz_class gen_prime(int bits){
    unsigned int counter = 0;
    mpz_class result;
    gmp_randclass rand(gmp_randinit_mt);

    while(true){
        rand.seed(time(nullptr) + counter);
        result = rand.get_z_bits(bits);
		if(is_prime(result, 1)){
			break;
		}
		counter++;
	}
    return result;
}

void gen_pq(mpz_class &p, mpz_class &q, unsigned int bits){
    bool fst = true;
    int p_bits = bits/2;
    int q_bits;
    while(true){
        if(fst){
            p = gen_prime(p_bits);
            fst = false;
            q_bits = bits - bits_size(p);
        }else{
            q = gen_prime(q_bits);
            if(p != q){
                break;
            }
        }
    }
}

mpz_class gcd(mpz_class a, mpz_class b){
    // Everything divides 0
    if (a == 0){
        return b;
    }
    if (b == 0){
        return a;
    }
  
    if (a == b){
        return a;
    }

    if (a > b){
        return gcd(a-b, b);
    }
    return gcd(a, b-a);
}

mpz_class m_inverse(){
    
}

void genereate_keys(int bits){
    mpz_class p, q, n, phi_n, e;

    // Generate p and q
    gen_pq(p, q, bits);
    // n = p * q
    n = p * q;
    // phi(n) = (q-1)*(p-1)
    phi_n = (q-1)*(p-1);
    // choose e that 1 < e < phi(n)
    do{
        e = gen_in_range(1, phi_n);
    }while(gcd(e, phi_n) != 1);


    

    cout << "P: " << p << " (" << bits_size(p) << ") " << endl;;
    // mpz_out_str(stdout, 2, p.get_mpz_t()); cout << endl;
    cout << "Q: " << q << " (" << bits_size(q) << ") " << endl;;
    // mpz_out_str(stdout, 2, q.get_mpz_t()); cout << endl;
    cout << "N: " << n << " (" << bits_size(n) << ") " << endl;
    cout << "Phi(n): " << phi_n << endl;
    cout << "E: " << e << endl;

    // TODO add is_prime function
    // TODO add gcd
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