/**
 * @file receiver.cpp
 * @author mingi kim
 * @brief source file implementing member functions of the FuzzyPSI class
 */

#include "fpsi.h"
#include <iostream>

FuzzyPSI::FuzzyPSI() {
}

FuzzyPSI::FuzzyPSI(pair<int64_t, int64_t> sender_range, int64_t receiver_value, int64_t threshold, int64_t modulus) 
    : sender_range_(sender_range), receiver_value_(receiver_value), threshold_(threshold), modulus_(modulus) {
}

int64_t FuzzyPSI::fuzzy_fast_one_to_one_matching() const {
    int64_t sender_start = sender_range_.first;
    int64_t sender_end = sender_range_.second;
    int64_t receiver_value = receiver_value_;

    // case 1 : check start > end (because of modulus!)
    if (sender_start > sender_end) {
        cout << "Not implemented yet!" << endl;
        return -1;
    }
    else {
        uint64_t unsigned_sender_start = static_cast<uint64_t>(sender_start + (modulus_ / 2));
        uint64_t unsigned_sender_end = static_cast<uint64_t>(sender_end + (modulus_ / 2));
        uint64_t unsigned_receiver_value = static_cast<uint64_t>(receiver_value + (modulus_ / 2));

        cout << "unsigned sender range: (" << unsigned_sender_start << ", " << unsigned_sender_end << ")" << endl;
        cout << "unsigned receiver value: " << unsigned_receiver_value << endl;

        std::vector<std::string> sa = genRepStringA(unsigned_sender_start, unsigned_sender_end);
        std::vector<std::string> sb = genRepStringB(unsigned_receiver_value, threshold_ * threshold_);
        cout << "Sender representative strings: " << endl;
        print(sa);
        cout << "Receiver representative strings: " << endl;
        print(sb);

        for (const auto &s : sa) {
            for (const auto &t : sb) {
                if (s == t) {
                    cout << "Match found: " << s << endl;
                    return 1;
                }
            }
        }
    }
    return -1;
}

FuzzyPSI::~FuzzyPSI() {
}

