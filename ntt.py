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
         result = np.zeros(N, dtype=object)
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

   def find_prime_for_polynomial(self, n, start=999999999):
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
           p -= 1

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
        padded_x = np.zeros(M, dtype=object)
        padded_x[:N] = x
        return padded_x, M
    return x, N

def PRG(seed):
      """
      Pseudo-Random Generator (PRG) function.
      This is a simple linear congruential generator (LCG).
      """
      # For Now returning seed only
      return seed

class user:
   def __init__(self, gid, sk, pk, n_g, primitive_root, prime):
      self.gid = gid
      self.sk = sk
      self.pk = pk
      self.n_g = n_g
      self.primitive_root = primitive_root
      self.prime = prime
      self.mask = None

   def create_mask(self, received_pk, p_s,index_self_pk):
      if len(received_pk) != p_s:
          raise ValueError("The length of received_pk must be equal to p_s.")
      if index_self_pk >= len(received_pk):
            raise ValueError("index_self_pk must be less than the length of received_pk.")
      
      odd = index_self_pk % 2
      
      received_pk = [received_pk[i] for i in range(len(received_pk)) if i != index_self_pk]

      self.mask = 0
      for i in range(len(received_pk)):
          self.mask = (self.mask + (pow(-1,odd+i+1))*PRG(pow(received_pk[i], self.sk, self.prime))) % self.prime
      
    #   print("Mask for User ID:", self.gid, "is", self.mask)
      return 1
   
   def send_aggregator_evaluating_results(self, obj_aggregator):
      if self.mask is None:
          raise ValueError("Mask has not been created yet.")
      
      value_representation = np.zeros(self.n_g, dtype=object)
      temp = 1
      for i in range(self.n_g):
          value_representation[i] = (self.mask + pow(temp, self.gid, self.prime)) % self.prime
          temp = (temp * self.primitive_root) % self.prime
    
    #   print("Value Representation for User ID:", self.gid, "is", value_representation)
      obj_aggregator.receive_value_representation(value_representation, self)
      return 1
   

class aggregator:
   def __init__(self, n_g, primitive_root, prime):
      self.n_g = n_g
      self.primitive_root = primitive_root
      self.prime = prime
      self.received_pk = []
      self.sum_value_representation = np.zeros(self.n_g, dtype=object)

   def receive_pk(self, pk, user_obj):
      self.received_pk.append((pk, user_obj))

   def send_pk(self):
      if len(self.received_pk)%2 != 0:
         raise ValueError("The number of received public keys must be even. p_s must be even.")
      p_s = len(self.received_pk)
      # Create a list of public keys
      public_keys = [pk for pk, _ in self.received_pk]

      # We need tp parallelize this process of sending public_key list to all users

      for i in range(p_s):
          user_obj = self.received_pk[i][1]
          if(user_obj.create_mask(public_keys, p_s, i) != 1):
              raise ValueError("Mask creation failed.")
      
      return 1
   
   def receive_value_representation(self, value_representation, user_obj):
      if len(value_representation) != self.n_g:
          raise ValueError("The length of value_representation must be equal to n_g.")
      
      for i in range(self.n_g):
            self.sum_value_representation[i] = (self.sum_value_representation[i] + value_representation[i]) % self.prime
      return 1
   
   def get_polynomial(self, obj_ntt):
      if len(self.sum_value_representation) != self.n_g:
          raise ValueError("The length of sum_value_representation must be equal to n_g.")
      
    #   print("Sum of Value Representation :", self.sum_value_representation)
      self.sum_value_representation = pad_to_power_of_two(self.sum_value_representation)[0]

      # Perform Inverse NTT on the sum_value_representation
      output = obj_ntt.inverse_ntt(self.sum_value_representation, self.prime, self.primitive_root)
      # Remove padding
      output = output[:self.n_g]
      
    #   print("Final Plolynomial after Aggregation :", output)
      return (1, output)


def test_aggregation():
    n_g = 512
    p_s = 400

    # p_s << prime , Must be Condition to avoid Coefficient modulo avoidance

    obj_prime = prime_factor_nth_root()
    prime = obj_prime.find_prime_for_polynomial(n_g)  # Find a prime for the polynomial length
    primitive_root = obj_prime.find_primitive_nth_root(prime, n_g)  # The primitive root for prime 5
    if pow(primitive_root, n_g, prime) != 1:
        raise ValueError("The primitive root is not valid for the given polynomial length.")
    print("Prime Modulo :", prime)
    print("Primitive Root:", primitive_root)
    # Create 5 users
    users = []
    # gid_list = [1, 4, 4, 7]
    gid_list = []
    check_polynomial = [0]*n_g
    for i in range(p_s):
        sk = np.random.randint(1, prime)
        pk = pow(primitive_root, sk, prime)
        gid = np.random.randint(0,n_g)
        if(gid >= n_g):
            raise ValueError("gid must be less than n_g.")
        gid_list.append(gid)
        check_polynomial[gid] = check_polynomial[gid] + 1
        # gid = gid_list[i]
        users.append(user(gid, sk, pk, n_g, primitive_root, prime))
        # print("User ID:", gid, "Secret Key:", sk, "Public Key:", pk)
    # print("gid_list: ", gid_list)
    # Create aggregator
    obj_aggregator = aggregator(n_g, primitive_root, prime)
    # Send public keys to aggregator
    for user_obj in users:
        obj_aggregator.receive_pk(user_obj.pk, user_obj)
    # Send public keys to users
    if obj_aggregator.send_pk() != 1:
        raise ValueError("Public key sending failed.")
    

    # TO check if thge sum of masks is equal to 0
    mask = 0
    for i in range(len(users)): 
        mask = (mask + users[i].mask) % prime

    if mask != 0:
        raise ValueError("The sum of masks is not equal to 0.")
    # Send value representation to aggregator

    temp = 1
    ans = []
    for i in range(n_g):
        t = 0
        for j in range(len(gid_list)):
            t = (t + pow(temp, gid_list[j], prime)) % prime
        temp = (temp* primitive_root) % prime

        ans.append(t)

    # print("Sum of Value Representation :", ans)


    for user_obj in users:
        user_obj.send_aggregator_evaluating_results(obj_aggregator)

    if ans != obj_aggregator.sum_value_representation.tolist():
        raise ValueError("The sum of value representation is not equal to the expected result.")
    else:
        print("The sum of value representation is equal to the expected result.")

    # Create NTT object
    obj_ntt = NTT(prime, primitive_root)
    # Get polynomial from aggregator
    res = obj_aggregator.get_polynomial(obj_ntt)
    if res[0] != 1:
        raise ValueError("Polynomial retrieval failed.")
    
    ouput = res[1]

    if check_polynomial != ouput.tolist():
        raise ValueError("The polynomial is not equal to the expected result.")
    else:
        print("The polynomial is equal to the expected result.")

    sum = 0
    for i in range(len(ouput)):
        sum = (sum + ouput[i])
    if sum != p_s:
        raise ValueError("The sum of polynomial is not equal to the expected result.")
    else:
        print("The sum of polynomial is equal to the expected result.")

    print("Final Polynomial after Aggregation :", ouput)
    
    

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
#    main()
    test_aggregation()