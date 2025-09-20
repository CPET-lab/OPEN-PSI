/**
 * @file receiver.cpp
 * @author <--- 여기에 당신의 이름을 작성하세요 --->
 * @brief Receiver 클래스의 멤버 함수를 구현하는 소스 파일입니다.
 */

// 1. 클래스 선언이 담긴 헤더 파일을 포함합니다.
#include "receiver.h"

// <--- 필요한 다른 라이브러리가 있다면 여기에 포함합니다. --->
// 예를 들어, 화면 출력을 위해 iostream을 포함할 수 있습니다.
#include <iostream>

// 2. 클래스 메서드 구현

/**
 * @brief receiver 클래스의 기본 생성자입니다.
 * * Receiver my_receiver; 와 같이 객체를 생성할 때 호출됩니다.
 * 멤버 변수들을 안전한 기본값으로 초기화하는 역할을 합니다.
 */
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

/**
 * @brief Receiver 클래스의 소멸자입니다.
 * * 객체가 메모리에서 해제될 때 자동으로 호출됩니다.
 * std::vector와 같은 표준 라이브러리 컨테이너를 사용하면
 * 메모리 관리가 자동으로 이루어지므로, 대부분의 경우 비워두어도 안전합니다.
 * 이 구현을 추가하면 "undefined reference to `Receiver::~Receiver()`" 링커 오류가 해결됩니다.
 */
Receiver::~Receiver() {
    // 객체가 소멸될 때 특별히 수행할 작업이 있다면 여기에 작성합니다.
    // (예: 파일 닫기, 네트워크 연결 해제 등)
}
