/**
 * @file sender.cpp
 * @author mingi kim
 * @brief source file implementing member functions of the Sender class
 */


#include "sender.h"
#include <iostream>

Sender::Sender()
    : data_(), // 빈 vector로 초기화
      data_size_(0),
      l_p_distance_(0) {}

/**
 * @brief Constructor with parameters
 * @param data set sender's vector
 * @param encoder CoeffEncoder
 * @param encryptor Encryptor
 */
Sender::Sender(const std::vector<int64_t>& data, int64_t modulus, seal::CoeffEncoder & encoder, seal::Encryptor & encryptor) : data_(data), modulus_(modulus) {
    data_ = data;
    data_size_ = data.size();
    l_p_distance_ = 0;

    for (const auto& val : data_) {
        l_p_distance_ += val * val;
    }

    this->encode(encoder);
    this->encrypt(encryptor);
}


Sender::~Sender() {
}
