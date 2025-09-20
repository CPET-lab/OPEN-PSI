#include "sender.h"
#include "receiver.h"

using namespace std;
using namespace seal;

int main(){
    
    // +----------------------------------------------------+
    // | poly_modulus_degree | max coeff_modulus bit-length |
    // +---------------------+------------------------------+
    // | 1024                | 27                           |
    // | 2048                | 54                           |
    // | 4096                | 109                          |
    // | 8192                | 218                          |
    // | 16384               | 438                          |
    // | 32768               | 881                          |
    // +---------------------+------------------------------+

    EncryptionParameters context_param(scheme_type::bgv);

    uint64_t poly_modulus_degree = pow(2, 15); // 2^15 = 32768
    context_param.set_poly_modulus_degree(poly_modulus_degree);
    context_param.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 40));

    context_param.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 41, 41, 60 }));

    SEALContext context(context_param, true, sec_level_type::tc128);

    print_parameters(context);
    cout << context.first_context_data()->qualifiers().parameter_error_message() << '\n';
    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_key;
    keygen.create_relin_keys(relin_key);

    CoeffEncoder encoder(context);
    Encryptor encryptor(context, public_key);
    Decryptor decryptor(context, secret_key);
    Evaluator evaluator(context);

    int64_t threshold = 200;
    cout << "Threshold: " << threshold << endl;
    
    // #1. Sender vector 생성
    // 1. vector<int64_t> sender_vector : a1 + a2·x + a3·x^2 + a4·x^3.. + ai·x^i (i 는 벡터 크기 - 1, 즉 마지막 항) 로 인코딩할 벡터


    vector<int64_t> sender_vector;
    uint64_t set_vector_size = poly_modulus_degree;

    for (size_t i = 0; i < set_vector_size; i++) {
        int64_t random_value = 0;
        random_value = 1;
        sender_vector.push_back(random_value);
    }

    Sender sender(sender_vector, encoder, encryptor);

    cout << "Sender vector: " << endl;
    
    for (size_t i = 0; i < 10; i++) {
        cout << sender.get_data(i) << " ";
    }
    cout << endl;

    // #2. Sender vector encoding
    // ex) a1 + a2·x + a3·x^2 + a4·x^3 + a5·x^4 ...
    // sender vector = { a11, a12, a13, a14, a15.. };

    
    Ciphertext sender_ciphertext = sender.get_ciphertext();
    print_noise_budget(decryptor, sender_ciphertext, "sender_ciphertext");

    //#4. Receiver vector 생성 후, 순서 반전
    // 마찬가지로 receiver vector 생성
    vector<int64_t> receiver_vector;
    for (size_t i = 0; i < set_vector_size; i++) {
        int64_t random_value = 0;
        random_value = 2;
        receiver_vector.push_back(random_value);
    }

    cout << "Receiver vector: " << endl;

    Receiver receiver(receiver_vector, context, encoder);
    for (size_t i = 0; i < 10; i++) {
        cout << receiver.get_data(i) << " ";
    }
    cout << endl;

    // 여기서부터 시간 측정
    // 시간 측정 변수 선언
    chrono::high_resolution_clock::time_point start_time, end_time;
    chrono::duration<double> elapsed_time;
    start_time = chrono::high_resolution_clock::now();

    // #6. Sender -> Receiver 암호문 전송
    // #7. sender 와 receiver 의 암호문 - 평문 곱셈
    Ciphertext multiplied_ciphertext = receiver.multiply_plain_with_ciphertext_and_add_random_vector(evaluator, sender_ciphertext);
    print_noise_budget(decryptor, multiplied_ciphertext, "multiplied_ciphertext(before relin)");
    


    int64_t receiver_distance = receiver.get_integer_psi_value();
    receiver_distance = centered_modulus(receiver_distance, context_param.plain_modulus().value());

    pair<int64_t, int64_t> sender_range = sender.get_psi_range(decryptor, encoder, multiplied_ciphertext, threshold);
    int64_t start = centered_modulus(sender_range.first, context_param.plain_modulus().value());
    int64_t end = centered_modulus(sender_range.second, context_param.plain_modulus().value());

    // 이후 (a^2 - 2ab + R - d^2, a^2 -2ab + R + 1) 범위를 계산
    // cout << "a²-2ab + R : " << result << endl;
    cout << "(a²-2ab + R - d², a²-2ab + R + 1) : " << start << ", " << end << endl;
    // #11. Receiver 는 -b^2 + R 을 가지고 있음.
    cout << "-b² + R : " << receiver_distance << endl;

    // 시간 측정 종료 및 결과 출력
    end_time = chrono::high_resolution_clock::now();
    elapsed_time = end_time - start_time;
    cout << "실행 시간: " << elapsed_time.count() << " 초" << endl;

    return 0;
}