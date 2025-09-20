#include "fv_boot.h"

using namespace std;
using namespace seal;
using namespace seal::util;


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

    EncryptionParameters context_param(scheme_type::bfv);

    uint64_t poly_modulus_degree = pow(2, 15); // 2^15 = 32768
    context_param.set_poly_modulus_degree(poly_modulus_degree);
    context_param.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 39));

    context_param.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 40, 40 }));

    uint64_t chain_size = context_param.coeff_modulus().size();

    uint64_t temp = context_param.coeff_modulus()[chain_size - 2].value();
    cout << "P 를 제외한 마지막 coeff mod :" << temp << endl;

    uint64_t plain_modulus = context_param.plain_modulus().value();
    cout << "Plain modulus: " << plain_modulus << endl;

    SEALContext context(context_param, true, sec_level_type::tc128);
    cout << "total coeff_modulus_bit_count: " << context.first_context_data()->total_coeff_modulus_bit_count() << endl;
    
    //  // --- 이 부분이 수정된 부분입니다 ---

    // 1. 첫 번째 ContextData 가져오기
    shared_ptr<const SEALContext::ContextData> first_data = context.first_context_data();
    for (size_t i = 0; i < context_param.coeff_modulus().size() - 1; i++) {
        auto delta = first_data->coeff_div_plain_modulus()[i];
        cout << "delta[" << i << "]: " << delta.operand << endl;
        cout << "delta[" << i << "]: " << delta.quotient << endl;
    }
    
    // if (first_data)
    // {
    //     // 2. total_coeff_modulus의 포인터 가져오기
    //     const uint64_t* total_mod_ptr = first_data->total_coeff_modulus();

    //     cout << first_data->upper_half_increment()[0] << endl;
    //     // 3. total_coeff_modulus를 구성하는 uint64_t의 개수 가져오기
    //     size_t uint64_count = first_data->parms().coeff_modulus().size();
        
    //     // 4. uint_to_dec_string 함수를 사용하여 10진수 문자열로 변환
    //     //    MemoryManager::GetPool()을 통해 기본 메모리 풀을 전달
    //     string dec_string = uint_to_dec_string(total_mod_ptr, uint64_count, MemoryManager::GetPool());
    //     string upper_half_string = uint_to_dec_string(first_data->upper_half_increment(), 1, MemoryManager::GetPool());
    //     cout << "--- First Context Data ---" << endl;
    //     cout << "Full total_coeff_modulus (Decimal): " << dec_string << endl;
    //     cout << "Total coeff_modulus bit count: " << first_data->total_coeff_modulus_bit_count() << endl;
    //     cout << "Upper half increment (Decimal): " << upper_half_string << endl;
    //     cout << "--------------------------" << endl << endl;
    // }


    print_parameters(context);
    cout << context.first_context_data()->qualifiers().parameter_error_message() << '\n';
    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_key;
    keygen.create_relin_keys(relin_key);

    BatchEncoder encoder(context);
    Encryptor encryptor(context, public_key);
    Decryptor decryptor(context, secret_key);
    Evaluator evaluator(context);

    vector<int64_t> fv_test;
    for (int i = 0; i < 5; i++) {
        fv_test.push_back(i);
    }

    Plaintext pt;
    encoder.encode(fv_test, pt);


    Ciphertext ct;
    encryptor.encrypt(pt, ct);

    auto context_data = context.first_context_data();

    print_noise_budget(decryptor, ct, "fv_test");

    decryptor.decrypt(ct, pt);
    vector<int64_t> result;
    encoder.decode(pt, result);
    for (size_t i = 0; i < 5; i++) {
        cout << result[i] << " ";
    }
    cout << endl;
    return 0;
}