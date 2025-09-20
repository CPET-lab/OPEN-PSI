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

Receiver::Receiver(const std::vector<int64_t>& data, seal::SEALContext & context, seal::CoeffEncoder & encoder) : data_(data) {
    data_ = data;
    data_size_ = data.size();
    l_p_distance_ = 0;

    for (const auto& val : data_) {
        l_p_distance_ += val * val;
    }

    for (size_t i = 0; i < data_size_; i++) {
        int64_t random_value = random_Zp(context.first_context_data()->parms().plain_modulus().value());
        random_vector_.push_back(random_value);
    }
    this->encode(encoder);
    encoder.encode(random_vector_, random_plaintext_);
}


Receiver::~Receiver() {
}
