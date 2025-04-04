#Import numpy module
import numpy as np

class NTT:
      def __init__(self, p, g):
         self.p = p  # prime modulus
         self.g = g  # primitive root
   

      def modinv(self, a, p):
         """Return the modular inverse of a modulo p."""
         return pow(a, -1, p)

      def ntt_cooley_tukey(self, x, p, g):
         N = len(x)
         if N == 1:
            return x
         # Compute odd and even part 
         evenPart = self.ntt_cooley_tukey(x[::2], p, (g * g) % p)
         oddPart = self.ntt_cooley_tukey(x[1::2], p, (g * g) % p)

         g_squared_mod_p = (g * g) % p
         factor = 1
         result = np.zeros(N, dtype=int)
         # for the N/2 range 
         for i in range(N // 2):
            term = (factor * oddPart[i]) % p
            #Compute the first part of the result
            result[i] = (evenPart[i] + term) % p
            # Compute the second part of the result
            result[i + N // 2] = (evenPart[i] - term) % p
            # Update the factor by multiplying with g_squared_mod_p.
            factor = (factor * g) % p

         return result


      def inverse_ntt(self, x, p, g):
          """
          Compute the inverse Number Theoretic Transform (NTT) of x modulo p.

          Parameters:
            x : numpy array
                The transformed array.
            p : int
                The prime modulus.
            g : int
                The primitive root used in the forward NTT.

          Returns:
            numpy array containing the original sequence.
          """
          N = len(x)
          # Compute the inverse of the primitive root g modulo p.
          g_inv = self.modinv(g, p)
          # Compute the inverse transform using the same recursive structure.
          y = self.ntt_cooley_tukey(x, p, g_inv)
          # Normalize the result by multiplying with the modular inverse of N.
          inv_N = self.modinv(N, p)
          return (y * inv_N) % p


class prime_factor_nth_root:

   def is_prime(self, num):
    """Return True if num is a prime number, else False."""
    if num < 2:
        return False
    if num == 2:
        return True
    if num % 2 == 0:
        return False
    # Check for factors up to sqrt(num)
    i = 3
    while i * i <= num:
        if num % i == 0:
            return False
        i += 2
    return True

   def find_prime_for_polynomial(self, n, start=2):
       """
       Find the smallest prime number p such that p ≡ 1 (mod n).

       Parameters:
         n : int
             The length of the polynomial (or the transform size). 
             A suitable prime must satisfy n | (p-1).
         start : int, optional
             The starting value for checking primes (default is 2).

       Returns:
         int
             A prime p such that (p-1) is divisible by n.
       """
       p = start
       # Loop until we find a prime p such that (p-1) % n == 0
       while True:
           if self.is_prime(p) and ((p - 1) % n == 0):
               return p
           p += 1

   def prime_factors(self, n):
    """Return the set of prime factors of n."""
    factors = set()
    d = 2
    while d * d <= n:
        while n % d == 0:
            factors.add(d)
            n //= d
        d += 1
    if n > 1:
        factors.add(n)
    return factors

   def is_primitive_root(self, a, p):
       """Check if a is a primitive root modulo p."""
       factors = self.prime_factors(p - 1)
       for q in factors:
           # If a^((p-1)/q) ≡ 1 (mod p) for any prime factor q, a is not primitive.
           if pow(a, (p - 1) // q, p) == 1:
               return False
       return True

   def find_primitive_nth_root(self, p, n):
       """
       Find a primitive n-th root of unity modulo p.
       
       Parameters:
         n : int
             The order of the desired root of unity (polynomial length).
         p : int
             The prime modulus (should satisfy n | (p-1)).
       
       Returns:
         omega : int
             A primitive n-th root of unity modulo p.
       """
       if (p - 1) % n != 0:
           raise ValueError("n does not divide p-1, so a primitive n-th root of unity does not exist.")
       
       exponent = (p - 1) // n
       # Find a primitive root of the whole multiplicative group modulo p.
       for a in range(2, p):
           if self.is_primitive_root(a, p):
               omega = pow(a, exponent, p)
               # Check that omega has order exactly n:
               if omega != 1 and pow(omega, n, p) == 1:
                   is_primitive = True
                   for k in range(1, n):
                       if pow(omega, k, p) == 1:
                           is_primitive = False
                           break
                   if is_primitive:
                       return omega
       raise ValueError("No primitive n-th root of unity found.")


def pad_to_power_of_two(x):
    N = len(x)
    # Find the next power of two
    M = 1
    while M < N:
        M *= 2
    # Pad with zeros if necessary
    if M != N:
        padded_x = np.zeros(M, dtype=int)
        padded_x[:N] = x
        return padded_x, M
    return x, N


def main():
   input_array = np.array([1, 2, 3, 4, 3, 4,3,2,19])
   input_array,_ = pad_to_power_of_two(input_array)

   obj_prime = prime_factor_nth_root()

   prime = obj_prime.find_prime_for_polynomial(len(input_array), start=2)  # Find a prime for the polynomial length
   print("Prime Modulo :", prime)
   primitive_root = obj_prime.find_primitive_nth_root(prime, len(input_array))  # The primitive root for prime 5
   # check if the primitive root is valid
   if pow(primitive_root, len(input_array), prime) != 1:
       raise ValueError("The primitive root is not valid for the given polynomial length.")
   print("Primitive Root:", primitive_root)

   obj_ntt = NTT(prime, primitive_root)
   output = obj_ntt.ntt_cooley_tukey(input_array, prime, primitive_root)
   print("NTT :", output)
   output2 = obj_ntt.inverse_ntt(output, prime, primitive_root)
   print("Inverse NTT :", output2)


if __name__ == "__main__":
   main()