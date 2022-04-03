#include <iostream>
#include <getopt.h>
#include <gmpxx.h>
#include <bits/stdc++.h>

using namespace std;

#define ARGUMENT_ERROR 1

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
            counter++;
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

bool is_prime(mpz_class n, unsigned int k, int bits){
    mpz_class d, d_mod;

    if(bits <= 3){
        if(n <= 1 || n == 4){
            return false;
        }

        if(n <= 3){
            return true;
        }
    }

    d = n - 1;

    while(d % 2 == 0){
        d /= 2;
    }
 
    for (unsigned int i = 0; i < k; i++){
        if(!miller_test(d, n)){
            return false;
		}
    }
    return true;
}

mpz_class gen_prime(int bits, unsigned int &counter){
    mpz_class result;
    gmp_randclass rand(gmp_randinit_mt);

    while(true){
        rand.seed(time(nullptr) + counter);
        result = rand.get_z_bits(bits);
        mpz_setbit(result.get_mpz_t(), bits-1);
		if(is_prime(result, 1, bits)){
            counter++;
			break;
		}
		counter++;
	}
    return result;
}

void gen_pq(mpz_class &p, mpz_class &q, unsigned int bits){
    int half_bits = bits/2;
    unsigned int counter = 0;
    while(true){
        p = gen_prime(half_bits, counter);
        q = gen_prime(half_bits, counter);
        if(p != q && bits_size(p*q) == bits){
            break;
        }
    }
}

mpz_class gcd(mpz_class a, mpz_class b){
    if(b == 0){
        return a;
    }
    a %= b;
    return gcd(b, a);
}

mpz_class m_inverse(mpz_class a, mpz_class m){
    mpz_class m0 = m;
    mpz_class y("0",10);
    mpz_class x("1",10);

    
    if (m == 1){
        return 0; 
    }
  
    while (a > 1) 
    { 
        mpz_class q = a / m; 
        mpz_class t = m; 
  
        m = a % m, a = t; 
        t = y; 
  
        y = x - q * y; 
        x = t; 
    } 
  
    if (x < 0) {
        x += m0; 
    }
    return x; 
}

void genereate_keys(int bits){
    mpz_class p, q, n, phi_n, e, d;

    gen_pq(p, q, bits);
    n = p * q;
    phi_n = (q-1)*(p-1);
    do{
        e = gen_in_range(1, phi_n);
    }while(gcd(e, phi_n) != 1);

    d = m_inverse(e, phi_n);

    cout << hex << "0x" << p << " 0x" << q << " 0x" << n << " 0x" << e << " 0x" << d << endl;
}

void encrypt_decrypt(string e, string n, string m){
    mpz_class ei(e);
    mpz_class ni(n);
    mpz_class mi(m);
    

    mpz_class res;
    mpz_powm(res.get_mpz_t(), mi.get_mpz_t(), ei.get_mpz_t(), ni.get_mpz_t());
    cout << hex << "0x" << res << endl;
}

void factorization(){
    cout << "Cracking the private key" << endl;
}

void printHelp(){
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

    switch(option[1]){
        case 'g':
            if(argc < 3){
                cerr << "Wrong program parameters. Use \"./kry -h\"" << endl;
                exit(ARGUMENT_ERROR);
            }
            genereate_keys(stoi(argv[2]));
            break;
        case 'e':
            if(argc < 5){
                cerr << "Wrong program parameters. Use \"./kry -h\"" << endl;
                exit(ARGUMENT_ERROR);
            }
            encrypt_decrypt(argv[2], argv[3], argv[4]);
            break;
        case 'd':
            if(argc < 5){
                cerr << "Wrong program parameters. Use \"./kry -h\"" << endl;
                exit(ARGUMENT_ERROR);
            }
            encrypt_decrypt(argv[2], argv[3], argv[4]);
            break;
        case 'b':
            if(argc < 3){
                cerr << "Wrong program parameters. Use \"./kry -h\"";
                exit(ARGUMENT_ERROR);
            }
            cout << "Factorization was not implemented."
            break;
        case 'h':
            printHelp();
            break;
        default:
            cerr << "Wrong program parameters. Use \"./kry -h\"";
            exit(ARGUMENT_ERROR);
        }
    return 0;
}