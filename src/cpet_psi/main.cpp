#include "sender.h"
#include "receiver.h"
#include "fpsi.h"

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

    int64_t threshold = 182;
    cout << "Threshold: " << threshold << endl;

    vector<int64_t> sender_vector;
    uint64_t set_vector_size = poly_modulus_degree;

    for (size_t i = 0; i < set_vector_size; i++) {
        int64_t random_value = 0;
        random_value = 1;
        sender_vector.push_back(random_value);
    }

    Sender sender(sender_vector, context_param.plain_modulus().value(), encoder, encryptor);

     // for DEBUG
    cout << "Sender vector: " << endl;
    sender.print_data();
    
    // #2. Sender vector encoding
    // ex) a1 + a2·x + a3·x^2 + a4·x^3 + a5·x^4 ...

    // for DEBUG
    Ciphertext sender_ciphertext = sender.get_ciphertext();
    print_noise_budget(decryptor, sender_ciphertext, "sender_ciphertext");

    // create receiver vector
    vector<int64_t> receiver_vector;
    for (size_t i = 0; i < set_vector_size; i++) {
        int64_t random_value = 0;
        random_value = 2;
        receiver_vector.push_back(random_value);
    }

    // for DEBUG
    cout << "Receiver vector: " << endl;
    Receiver receiver(receiver_vector, context_param.plain_modulus().value(), encoder);
    receiver.print_data();
    cout << endl;

    // psi 객체 생성
    FuzzyPSI fpsi(sender, receiver, threshold, context_param.plain_modulus().value(), context_param.poly_modulus_degree());

    // 여기서부터 시간 측정
    // 시간 측정 변수 선언
    chrono::high_resolution_clock::time_point start_time, end_time;
    chrono::duration<double> elapsed_time;
    start_time = chrono::high_resolution_clock::now();

    // fpsi.print_info();
    vector<int64_t> result = fpsi.fuzzy_fast_one_to_one_matching(encoder, decryptor, evaluator, sender_ciphertext);

    cout << "| IF match, return receiver value; else return max int64_t vector |" << endl;
    for (size_t i = 0; i < 10; i++) {
        cout << result[i] << " ";
    }
    cout << "..." << endl;

    // 시간 측정 종료 및 결과 출력
    end_time = chrono::high_resolution_clock::now();
    elapsed_time = end_time - start_time;
    cout << "실행 시간: " << elapsed_time.count() << " 초" << endl;

    return 0;
}