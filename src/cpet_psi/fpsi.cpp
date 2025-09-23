/**
 * @file fpsi.cpp
 * @author mingi kim
 * @brief source file implementing member functions of the FuzzyPSI class
 */

#include "fpsi.h"
#include <iostream>
#include <limits>

FuzzyPSI::FuzzyPSI() {
}

FuzzyPSI::FuzzyPSI(Sender & sender, Receiver & receiver, int64_t threshold, int64_t modulus, uint64_t slot_count) 
    : sender_(sender), receiver_(receiver), threshold_(threshold), modulus_(modulus), data_size_(slot_count) {
    sender_range_ = pair<int64_t, int64_t>(0,0);
    receiver_value_ = 0;
    MAX_VEC_ = vector<int64_t>(data_size_, std::numeric_limits<int64_t>::max());
}

void FuzzyPSI::distribute(seal::CoeffEncoder & encoder, seal::Decryptor & decryptor, seal::SEALContext & context, seal::Evaluator & evaluator, seal::Ciphertext & sender_ciphertext) {
    // steps:
    // 1. Sender -> Receiver ciphertext transmission
    // 2. Sender and Receiver ciphertext-plaintext multiplication
    // 3. Receiver computes integer psi value (-b^2 + R) mod t
    // 4. Sender computes psi range (a^2 - 2ab + R - d^2, a^2 -2ab + R + 1)

    seal::Ciphertext multiplied_ciphertext;
    receiver_.multiply_plain_with_ciphertext_and_add_random_vector(context, evaluator, sender_ciphertext, multiplied_ciphertext);

    // for DEBUG
    // print_noise_budget(decryptor, multiplied_ciphertext, "multiplied_ciphertext");
    receiver_value_ = receiver_.get_psi_value();

    // (a^2 - 2ab + R - d^2, a^2 -2ab + R + 1)
    sender_range_ = sender_.get_psi_range(decryptor, encoder, multiplied_ciphertext, threshold_);
}
vector<int64_t> FuzzyPSI::fuzzy_fast_one_to_one_matching(seal::CoeffEncoder & encoder, seal::Decryptor & decryptor, seal::SEALContext & context, seal::Evaluator & evaluator, seal::Ciphertext & sender_ciphertext) {

    distribute(encoder, decryptor, context, evaluator, sender_ciphertext);

    int64_t sender_start = sender_range_.first;
    int64_t sender_end = sender_range_.second;
    int64_t receiver_value = receiver_value_;

    // case 1 : check start > end (because of modulus!)
    if (sender_start > sender_end) {
        cout << "Not implemented yet!" << endl;
        return MAX_VEC_; // return max int64_t if no match
    }
    // case 2 : normal case
    // if start, end, receiver_value are negative, convert to positive by adding modulus / 2
    else {
        uint64_t unsigned_sender_start = static_cast<uint64_t>(sender_start + (modulus_ / 2));
        uint64_t unsigned_sender_end = static_cast<uint64_t>(sender_end + (modulus_ / 2));
        uint64_t unsigned_receiver_value = static_cast<uint64_t>(receiver_value + (modulus_ / 2));

        // for DEBUG
        // cout << "unsigned sender range: (" << unsigned_sender_start << ", " << unsigned_sender_end << ")" << endl;
        // cout << "unsigned receiver value: " << unsigned_receiver_value << endl;

        std::vector<std::string> sa = genRepStringA(unsigned_sender_start, unsigned_sender_end);
        std::vector<std::string> sb = genRepStringB(unsigned_receiver_value, threshold_ * threshold_);

        // for DEBUG
        // cout << "Sender representative strings: " << endl;
        // print(sa);
        // cout << "Receiver representative strings: " << endl;
        // print(sb);
        
        // TODO : 이 부분 PSI 를 사용하도록 변경 해야함.
        for (const auto &s : sa) {
            for (const auto &t : sb) {
                if (s == t) {
                    cout << "Match found: " << s << endl;
                    return receiver_.get_data();
                }
            }
        }
    }
    return MAX_VEC_; // return max int64_t if no match
}

FuzzyPSI::~FuzzyPSI() {
}

