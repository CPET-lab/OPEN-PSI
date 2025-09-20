// receiver.h

#ifndef RECEIVER_H_ 
#define RECEIVER_H_

# include "util.h"

class Receiver {
public:
    Receiver();
    
    /**
     * @brief Constructor with parameters
     * @param data set receiver's vector
     * @param context SEALContext
     * @param encoder CoeffEncoder
     */

    explicit Receiver(const std::vector<int64_t>& data, seal::SEALContext & context, seal::CoeffEncoder & encoder);

    /**
     * @brief Get integer psi value 
     * @return (-b^2 + R)
     */
    inline int64_t get_integer_psi_value() const {
        return -l_p_distance_ + random_vector_[data_size_ - 1];
    }
   
    /**
     * @brief Get data at index
     * @param index index of data
     * @return data at index
     */
    inline int64_t get_data(uint64_t &index) const {
        return data_[index];
    }
   
    /**
     * @brief Encode data to plaintext
     * @param encoder CoeffEncoder
     */
    inline void encode(seal::CoeffEncoder &encoder) {
        
        vector<int64_t> temp_data = data_;
        for (size_t i = 0; i < data_size_ / 2; i++) {
            swap(temp_data[i], temp_data[data_size_ - 1 - i]);
        }

        for (size_t i = 0; i < data_size_; i++) {
            temp_data[i] *= -2;
        }

        encoder.encode(temp_data, plaintext_);
    }

    /**
     * @brief Multiply plaintext with ciphertext and add random vector
     * @param evaluator Evaluator
     * @param sender_ciphertext Ciphertext from sender
     * @param destination Result ciphertext
     */
    inline void multiply_plain_with_ciphertext_and_add_random_vector(seal::Evaluator &evaluator, const seal::Ciphertext &sender_ciphertext, seal::Ciphertext &destination) {
        evaluator.multiply_plain(sender_ciphertext, plaintext_, destination);
        evaluator.add_plain_inplace(destination, random_plaintext_);
    }

    // destructor
    ~Receiver();


private:
    vector<int64_t> data_;
    uint64_t data_size_;
    int64_t l_p_distance_;
    seal::Plaintext plaintext_;
    seal::Ciphertext ciphertext_; 
    vector<int64_t> random_vector_;
    seal::Plaintext random_plaintext_;
}; 

#endif 