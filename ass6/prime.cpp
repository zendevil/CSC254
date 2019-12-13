// Source: https://www.geeksforgeeks.org/smallest-special-prime-which-is-greater-than-or-equal-to-a-given-number/
// CPP program to find the Smallest Special Prime 
// which is greater than or equal to a given number 
#include <string.h>
  
// Function to check whether the number 
// is a special prime or not 
bool check_special_prime(bool* sieve, int num) 
{ 
    // While number is not equal to zero 
    while (num) { 
        // If the number is not prime 
        // return false. 
        if (!sieve[num]) { 
            return false; 
        } 
  
        // Else remove the last digit 
        // by dividing the number by 10. 
        num /= 10; 
    } 
  
    // If the number has become zero 
    // then the number is special prime, 
    // hence return true 
    return true; 
} 
  
// Function to find the Smallest Special Prime 
// which is greater than or equal to a given number 
int find_special_prime(int N) 
{ 
    bool sieve[N*10]; 
  
    // Initially all numbers are considered Primes. 
    memset(sieve, true, sizeof(sieve)); 
    sieve[0] = sieve[1] = false; 
    for (long long i = 2; i <= N*10; i++) { 
        if (sieve[i]) { 
  
            for (long long j = i * i; j <= N*10; j += i) { 
                sieve[j] = false; 
            } 
        } 
    } 
  
    // There is always an answer possible 
    while (true) { 
        // Checking if the number is a 
        // special prime or not 
        if (check_special_prime(sieve, N)) { 
            return N;
        } 
        // Else increment the number. 
        else
            N++; 
    } 
}

