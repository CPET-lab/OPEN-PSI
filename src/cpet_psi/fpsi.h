#ifndef FPSI_H_
#define FPSI_H_

# include "util.h"
# include "DASetAug.h"
# include "sender.h"
# include "receiver.h"

class FuzzyPSI {
public:
    FuzzyPSI();

    /**
     * @brief Constructor with parameters
     * @param sender_range sender's psi range (start, end)
     * @param receiver_value receiver's integer psi value
     * @param threshold threshold value
     * @param modulus modulus value
     */

    explicit FuzzyPSI(Sender & sender, Receiver & receiver, int64_t threshold, int64_t modulus, uint64_t slot_count);

    /**
     * @brief Print info (for DEBUG)
     */
    inline void print_info() const {
        cout << "Sender range: (" << sender_range_.first << ", " << sender_range_.second << ")" << endl;
        cout << "Receiver value: " << receiver_value_ << endl;
        cout << "Threshold: " << threshold_ << endl;
        cout << "Modulus: " << modulus_ << endl;
    }

    /**
     * @brief Distribute process: sender <-> receiver
     * @brief Steps:
     * 1. Sender -> Receiver ciphertext transmission
     * 2. Sender and Receiver ciphertext-plaintext multiplication
     * 3. Receiver computes integer psi value (-b^2 + R) mod t
     * 4. Sender computes psi range (a^2 - 2ab + R - d^2, a^2 -2ab + R + 1)
     * @param encoder CoeffEncoder
     * @param decryptor Decryptor
     * @param evaluator Evaluator
     * @param sender_ciphertext Ciphertext from sender
     * @return void
     */
    void distribute(seal::CoeffEncoder & encoder, seal::Decryptor & decryptor, seal::Evaluator & evaluator, seal::Ciphertext & sender_ciphertext);

    /** 
    * @brief Perform fuzzy one-to-one matching
    * @brief Steps:
    * 1. distribute process
    * 2. check if receiver_value in sender_range
    * 2-1 case 1 : check start > end (because of modulus!) (not implemented yet)
    * 2-2 case 2 : normal case
    * @brief If match, return receiver_value; else return vector, which elements are max int64_t

    * @param encoder CoeffEncoder
    * @param decryptor Decryptor
    * @param evaluator Evaluator
    * @param sender_ciphertext Ciphertext from sender
    * @return if match, return receiver_value; else return vector, which elements are max int64_t
    */
    vector<int64_t> fuzzy_fast_one_to_one_matching(seal::CoeffEncoder & encoder, seal::Decryptor & decryptor, seal::Evaluator & evaluator, seal::Ciphertext & sender_ciphertext);
    
    // destructor
    ~FuzzyPSI();


private:
    Sender sender_;
    Receiver receiver_;
    pair<int64_t, int64_t> sender_range_;
    int64_t receiver_value_;
    int64_t threshold_;
    int64_t modulus_;
    uint64_t data_size_;
    vector<int64_t> MAX_VEC_;
}; 


#endif