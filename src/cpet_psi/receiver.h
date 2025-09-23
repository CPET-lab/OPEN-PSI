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

    explicit Receiver(const std::vector<int64_t>& data, int64_t modulus, seal::CoeffEncoder & encoder);

    /**
     * @brief Get integer psi value 
     * @return (-b^2 + R) mod t
     */
    inline int64_t get_psi_value() const {
        int64_t result = -l_p_distance_ + random_vector_[data_size_ - 1];
        return centered_modulus(result, modulus_);
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
     * @param context SEALContext
     * @param evaluator Evaluator
     * @param sender_ciphertext Ciphertext from sender
     * @param destination Result ciphertext
     */
    inline void multiply_plain_with_ciphertext_and_add_random_vector(seal::SEALContext &context, seal::Evaluator &evaluator, const seal::Ciphertext &sender_ciphertext, seal::Ciphertext &destination) {
        evaluator.multiply_plain(sender_ciphertext, plaintext_, destination);
        evaluator.add_plain_inplace(destination, random_plaintext_);

        // [TODO] : noise flooding 추가 해야함
        noise_flooding(destination, context, evaluator, destination);
    }
    
    /**
     * @brief Add noise flooding to the ciphertext
     * @param ciphertext Ciphertext to add noise flooding
     * @param context SEALContext
     * @param evaluator Evaluator
     * @param destination Result ciphertext with noise flooding
     */
    void noise_flooding(const seal::Ciphertext& ciphertext, const seal::SEALContext& context, const seal::Evaluator& evaluator, seal::Ciphertext& destination);
    
    // destructor
    ~Receiver();


private:
    vector<int64_t> data_;
    uint64_t data_size_;
    int64_t l_p_distance_;
    int64_t modulus_;
    seal::Plaintext plaintext_;
    seal::Ciphertext ciphertext_; 
    vector<int64_t> random_vector_;
    seal::Plaintext random_plaintext_;
}; 

#endif 