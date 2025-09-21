// sender.h

#ifndef SENDER_H_
#define SENDER_H_ 

# include "util.h"

class Sender {
public:
    Sender();
    
    /**
     * @brief Constructor with parameters
     * @param data set sender's vector
     * @param encoder CoeffEncoder
     * @param encryptor Encryptor
     */
    explicit Sender(const std::vector<int64_t>& data, int64_t modulus, seal::CoeffEncoder & encoder, seal::Encryptor & encryptor);

    /**
     * @brief Get psi range (start, end)
     * @param decryptor Decryptor
     * @param encoder CoeffEncoder
     * @param multiplied_ciphertext Ciphertext after multiplication
     * @param threshold threshold value
     * @return pair of (start, end)
     */
    inline pair<int64_t, int64_t> get_psi_range(seal::Decryptor & decryptor, seal::CoeffEncoder & encoder, seal::Ciphertext & multiplied_ciphertext, int64_t threshold) const {
        // decrypt
        seal::Plaintext decrypted_plaintext;
        decryptor.decrypt(multiplied_ciphertext, decrypted_plaintext);

        // decode
        vector<int64_t> decoded_result;
        encoder.decode(decrypted_plaintext, decoded_result);

        int64_t start = l_p_distance_ + decoded_result[data_size_ - 1] - threshold*threshold;
        start = centered_modulus(start, modulus_);
        int64_t end = l_p_distance_ + decoded_result[data_size_ - 1] + 1;
        end = centered_modulus(end, modulus_);
        return {start, end};
    }

    /**
     * @brief Get data
     * @return vector<int64_t>
     */
    inline vector<int64_t> get_data() const {
        return data_;
    }
    /**
     * @brief Print data (for DEBUG)
     */
    inline void print_data() const {
        for (size_t i = 0; i < 10; i++) {
            cout << data_[i] << " ";
        }
        cout << "... ";
        cout << endl;
    }

    /**
     * @brief Encode data to plaintext
     * @param encoder CoeffEncoder
     */
    inline void encode(seal::CoeffEncoder &encoder) {
        encoder.encode(data_, plaintext_);
    }

    /**
     * @brief Encrypt plaintext to ciphertext
     * @param encryptor Encryptor
     */
    inline void encrypt(seal::Encryptor &encryptor) {
        encryptor.encrypt(plaintext_, ciphertext_);
    }

    /**
     * @brief Get ciphertext
     * @return ciphertext
     */
    inline seal::Ciphertext get_ciphertext() const {
        return ciphertext_;
    }

    // destructor
    ~Sender();


private:
    vector<int64_t> data_;
    uint64_t data_size_;
    int64_t l_p_distance_;
    seal::Plaintext plaintext_;
    seal::Ciphertext ciphertext_; 
    int64_t modulus_;
};

#endif