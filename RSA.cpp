#include <iostream>
#include <getopt.h>
#include <gmpxx.h>
#include <bits/stdc++.h>

using namespace std;

#define ARGUMENT_ERROR 1

class RSA{
private:
    mpz_class p, q, n, phi_n, e, d;
    unsigned int seed = time(nullptr)/2;
    gmp_randclass rand = gmp_randclass(gmp_randinit_mt);

    mpz_class gen_prime(int bits);
    bool is_prime(mpz_class n_p, unsigned int k_p, int bits);
    bool miller_test(mpz_class d_p, mpz_class n_p);
    mpz_class gcd(mpz_class a, mpz_class b);
    mpz_class m_inverse(mpz_class a, mpz_class m);

public:
    RSA();
    ~RSA();
    void genereate_keys(unsigned int bits);
    void encrypt_decrypt(string e, string n, string m);
    void factorization();
    void printHelp();
};

RSA::RSA(){
}

RSA::~RSA(){
}

bool RSA::miller_test(mpz_class d_p, mpz_class n_p){
    mpz_class a, x;
    
    // [2, n-2]
    rand.seed(seed++);
    a = 2 + rand.get_z_range(n-4);
    mpz_powm(x.get_mpz_t(), a.get_mpz_t(), d_p.get_mpz_t(), n_p.get_mpz_t());
 
    if(x == 1 || x == n_p-1){
        return true;
    }
    
    while(d_p != n_p-1){
        x = (x * x) % n_p;
        d_p *= 2;
        if(x == 1){
            return false;
        }
        if(x == n_p-1){
            return true;
        }
    }
    return false;
}

bool RSA::is_prime(mpz_class n_p, unsigned int k_p, int bits){
    mpz_class d_p;

    if(bits <= 3){
        if(n_p <= 1 || n_p == 4){
            return false;
        }

        if(n_p <= 3){
            return true;
        }
    }

    d_p = n_p - 1;

    while(d_p % 2 == 0){
        d_p /= 2;
    }
 
    for (unsigned int i = 0; i < k_p; i++){
        if(!miller_test(d_p, n_p)){
            return false;
		}
    }
    return true;
}

mpz_class RSA::gen_prime(int bits){
    mpz_class result;
    // gmp_randclass rand(gmp_randinit_mt);

    do{
        rand.seed(seed++);
        result = rand.get_z_bits(bits);
        mpz_setbit(result.get_mpz_t(), bits-1);
    }while(!is_prime(result, 1, bits));
    return result;
}

mpz_class RSA::gcd(mpz_class a, mpz_class b){
    if(b == 0){
        return a;
    }
    a %= b;
    return gcd(b, a);
}

mpz_class RSA::m_inverse(mpz_class a, mpz_class m){
    mpz_class m0 = m;
    mpz_class y("0",10);
    mpz_class x("1",10);

    
    if (m == 1){
        return 0; 
    }
  
    while (a > 1) 
    { 
        mpz_class q_i = a / m; 
        mpz_class t = m; 
  
        m = a % m, a = t; 
        t = y; 
  
        y = x - q_i * y; 
        x = t; 
    } 
  
    if (x < 0) {
        x += m0; 
    }
    return x; 
}

void RSA::genereate_keys(unsigned int bits){
    int half_bits = bits/2;
    while(true){
        p = gen_prime(half_bits);
        q = gen_prime(half_bits);
        if(p != q){
            n = p*q;
            if(mpz_sizeinbase(n.get_mpz_t(), 2) == bits){
                break;
            }
        }
    }

    phi_n = (q-1)*(p-1);

    do{
        rand.seed(seed++);
        e = 2 + rand.get_z_range(phi_n-3);
    }while(gcd(e, phi_n) != 1);

    d = m_inverse(e, phi_n);

    // cout << "P: " << p << " (" << mpz_sizeinbase(p.get_mpz_t(), 2) << ") " << endl;;
    // cout << "Q: " << q << " (" << mpz_sizeinbase(q.get_mpz_t(), 2) << ") " << endl;;
    // cout << "N: " << n << " (" << mpz_sizeinbase(n.get_mpz_t(), 2) << ") " << endl;
    // cout << "Phi(n): " << phi_n << endl;
    // cout << "E: " << e << endl;
    // cout << "D: " << d << endl;
    

    cout << hex << "0x" << p << " 0x" << q << " 0x" << n << " 0x" << e << " 0x" << d << endl;
}

void RSA::encrypt_decrypt(string es, string ns, string ms){
    mpz_class ei(es);
    mpz_class ni(ns);
    mpz_class mi(ms);

    mpz_class res;
    mpz_powm(res.get_mpz_t(), mi.get_mpz_t(), ei.get_mpz_t(), ni.get_mpz_t());
    cout << hex << "0x" << res << endl;
}

void RSA::factorization(){
    cout << "Factorization was not implemented." << endl;
}

void RSA::printHelp(){
    cout << "kry [options]" << endl <<
    "Options:" << endl <<
    "-h                     Print this help message" << endl <<
    "-g B                   Generates keys (P Q N E D)" << endl <<
    "-e E N M               Encrypts message (C)" << endl <<
    "-d D N C               Decrypts message (M)" << endl <<
    "-b N                   Cracks RSA (P)" << endl << endl <<
    "Parameters:" << endl <<
    "B                      Modulus size in bits" << endl <<
    "E                      Public exponent" << endl <<
    "D                      Private exponent" << endl <<
    "N                      Public modulus" << endl <<
    "M                      Open message" << endl <<
    "C                      Encrypted message" << endl;    
}

int main(int argc, char** argv){
    if(argc == 1 || argv[1][0] != '-'){
        cerr << "Wrong program parameters. Use \"./kry -h\"" << endl;
        exit(ARGUMENT_ERROR);
    }
    string option = argv[1];

    RSA rsa = RSA();

    switch(option[1]){
        case 'g':
            if(argc < 3){
                cerr << "Wrong program parameters. Use \"./kry -h\"" << endl;
                exit(ARGUMENT_ERROR);
            }
            rsa.genereate_keys(stoi(argv[2]));
            break;
        case 'e':
            if(argc < 5){
                cerr << "Wrong program parameters. Use \"./kry -h\"" << endl;
                exit(ARGUMENT_ERROR);
            }
            rsa.encrypt_decrypt(argv[2], argv[3], argv[4]);
            break;
        case 'd':
            if(argc < 5){
                cerr << "Wrong program parameters. Use \"./kry -h\"" << endl;
                exit(ARGUMENT_ERROR);
            }
            rsa.encrypt_decrypt(argv[2], argv[3], argv[4]);
            break;
        case 'b':
            if(argc < 3){
                cerr << "Wrong program parameters. Use \"./kry -h\"";
                exit(ARGUMENT_ERROR);
            }
            rsa.factorization();
            break;
        case 'h':
            rsa.printHelp();
            break;
        default:
            cerr << "Wrong program parameters. Use \"./kry -h\"";
            exit(ARGUMENT_ERROR);
        }
    return 0;
}