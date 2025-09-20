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


Receiver::~Receiver() {
}
