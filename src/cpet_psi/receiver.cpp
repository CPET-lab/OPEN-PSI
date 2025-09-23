/**
 * @file receiver.cpp
 * @author mingi kim
 * @brief source file implementing member functions of the Receiver class
 */


#include "receiver.h"

#include <iostream>

Receiver::Receiver()
    : data_(), // 빈 vector로 초기화
      data_size_(0),
      l_p_distance_(0) {}


/** * @brief Constructor with parameters
 * @param data set receiver's vector
 * @param context SEALContext
 * @param encoder CoeffEncoder
 */
Receiver::Receiver(const std::vector<int64_t>& data, int64_t modulus, seal::CoeffEncoder & encoder) : data_(data), modulus_(modulus) {
    data_ = data;
    data_size_ = data.size();
    l_p_distance_ = 0;

    for (const auto& val : data_) {
        l_p_distance_ += val * val;
    }

    // generate random vector, and encode it to random_plaintext_
    for (size_t i = 0; i < data_size_; i++) {
        int64_t random_value = random_Zp(modulus_);
        random_vector_.push_back(random_value);
    }
    this->encode(encoder);
    encoder.encode(random_vector_, random_plaintext_);
}

void Receiver::noise_flooding(const seal::Ciphertext& ciphertext, const seal::SEALContext& context, const seal::Evaluator& evaluator, seal::Ciphertext& destination)
{
   // Extract encryption parameters.
    auto context_data_ptr = context.get_context_data(ciphertext.parms_id());
    const auto& coeff_modulus = context_data_ptr->parms().coeff_modulus();
    size_t coeff_modulus_size = coeff_modulus.size();
   uint64_t plain_modulus = context_data_ptr->parms().plain_modulus().value();
    uint64_t noise_bound = 1ULL << 40;

    // Copy
    seal::Ciphertext encrypted_copy = ciphertext;

    if (encrypted_copy.is_ntt_form())
    {
        evaluator.transform_from_ntt_inplace(encrypted_copy);
    }

    seal::Ciphertext::ct_coeff_type* ptr = encrypted_copy.data();

   // Add big noise to c_0.
   // c_0 += t * E, where E is sampled from uniform distribution [0, 2^40].
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dist(0, noise_bound);
    uint64_t E = dist(gen);

    for (size_t j = 0; j < coeff_modulus_size; j++)
    {
        auto modulus = coeff_modulus[j];
        auto poly_modulus_degree = encrypted_copy.poly_modulus_degree();
        for (; poly_modulus_degree--; ptr++)
        {
            uint64_t noise = seal::util::multiply_uint_mod(plain_modulus, E, modulus);
         *ptr = seal::util::add_uint_mod(*ptr, noise, modulus);
        }
    }

    if (!encrypted_copy.is_ntt_form())
    {
        evaluator.transform_to_ntt_inplace(encrypted_copy);
    }

    destination = encrypted_copy;
}

Receiver::~Receiver() {
}
