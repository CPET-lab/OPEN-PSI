// receiver.h

#ifndef RECEIVER_H_ 
#define RECEIVER_H_

# include "util.h"

class Receiver {
public:
    Receiver();
    
    // 4. 공개 멤버 함수 (메서드)
    /**
     * @brief 원격 서버에 연결합니다.
     * @param ip 연결할 서버의 IP 주소입니다.
     * @param port 연결할 서버의 포트 번호입니다.
     * @return 연결 성공 시 true, 실패 시 false를 반환합니다.
     */

    explicit Receiver(const std::vector<int64_t>& data, seal::SEALContext & context, seal::CoeffEncoder & encoder);

    inline int64_t get_integer_psi_value() const {
        return -l_p_distance_ + random_vector_[data_size_ - 1];
    }
   
    inline int64_t get_data(uint64_t &index) const {
        return data_[index];
    }
   
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

    inline seal::Ciphertext multiply_plain_with_ciphertext_and_add_random_vector(seal::Evaluator &evaluator, const seal::Ciphertext &sender_ciphertext) {
        seal::Ciphertext result;
        evaluator.multiply_plain(sender_ciphertext, plaintext_, result);
        evaluator.add_plain_inplace(result, random_plaintext_);
        return result;
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
}; // 클래스 선언 끝에는 반드시 세미콜론(;)을 붙여야 합니다.

#endif /* RECEIVER_H_ */ // 헤더 가드의 끝