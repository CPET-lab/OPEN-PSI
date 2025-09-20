// sender.h

#ifndef SENDER_H_ // 이 파일이 아직 포함되지 않았다면
#define SENDER_H_ // 이제 포함되었다고 표시합니다.

# include "util.h"

class Sender {
public:
    // 기본 생성자: Sender sender_object; 와 같이 호출됩니다.
    Sender();
    
    // 4. 공개 멤버 함수 (메서드)
    /**
     * @brief 원격 서버에 연결합니다.
     * @param ip 연결할 서버의 IP 주소입니다.
     * @param port 연결할 서버의 포트 번호입니다.
     * @return 연결 성공 시 true, 실패 시 false를 반환합니다.
     */
    
    explicit Sender(const std::vector<int64_t>& data, seal::CoeffEncoder & encoder, seal::Encryptor & encryptor);

    inline pair<int64_t, int64_t> get_psi_range(seal::Decryptor & decryptor, seal::CoeffEncoder & encoder, seal::Ciphertext & multiplied_ciphertext, int64_t threshold) const {
        // decrypt
        seal::Plaintext decrypted_plaintext;
        decryptor.decrypt(multiplied_ciphertext, decrypted_plaintext);

        // decode
        vector<int64_t> decoded_result;
        encoder.decode(decrypted_plaintext, decoded_result);

        int64_t start = l_p_distance_ + decoded_result[data_size_ - 1] - threshold*threshold;
        int64_t end = l_p_distance_ + decoded_result[data_size_ - 1] + 1;
        return {start, end};
    }

    inline int64_t get_data(uint64_t &index) const {
        return data_[index];
    }

    inline void encode(seal::CoeffEncoder &encoder) {
        encoder.encode(data_, plaintext_);
    }

    inline void encrypt(seal::Encryptor &encryptor) {
        encryptor.encrypt(plaintext_, ciphertext_);
    }

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

}; // 클래스 선언 끝에는 반드시 세미콜론(;)을 붙여야 합니다.

#endif /* SENDER_H_ */ // 헤더 가드의 끝