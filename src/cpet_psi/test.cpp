#include <seal/seal.h>
#include <random>

using namespace seal;
using namespace std;

int main()
{
    // Parameters.
    size_t poly_modulus_degree = 4096;
    int q_0_bit_size = 60;
    int P_bit_size = q_0_bit_size;
    int scale_bit_size = 51;
    int delta_bit_size = 60;
    double_t scale = pow(2.0, scale_bit_size);
    double_t delta = pow(2.0, delta_bit_size);
    size_t encrypted_level = 0;
    size_t d_0 = 7;
    size_t r = 6;


    // CKKS context.
    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_bootstrapping_depth(CKKSBootstrapper::get_bootstrap_depth(delta_bit_size, encrypted_level, d_0, r)); 
    parms.set_coeff_modulus(CKKSBootstrapper::create_coeff_modulus(   // create coefficient modulus for bootstrapping.
		poly_modulus_degree, 
        { q_0_bit_size, scale_bit_size, scale_bit_size, P_bit_size }, 
        delta_bit_size, encrypted_level, d_0, r));
    SEALContext context(parms, true, sec_level_type::none);
    // print_parameters(context);
    cout << endl;


    // Keys.
    KeyGenerator keygen(context, true);   // use sparse secret key.
    // KeyGenerator keygen(context);         // use non-sparse secret key.
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    GaloisKeys galois_keys;
	keygen.create_galois_keys(   // create Galois keys for bootstrapping.
        CKKSBootstrapper::create_galois_steps(poly_modulus_degree), galois_keys);


    // Algorithms.
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    CKKSEncoder encoder(context);
    CKKSBootstrapper bootstrapper(context);   // create bootstrapper instance.


    // Random data.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    vector<complex<double_t>> vector_1, res_vector;
    vector_1.reserve(encoder.slot_count());
    for (size_t i = 0; i < encoder.slot_count(); i++)
    {
        vector_1.push_back({ static_cast<double_t>(dist(gen)) });
        //vector_1.push_back({ static_cast<double_t>(dist(gen)) * 10 });
    }
    cout << "Input vector: " << endl;
    // print_vector(vector_1, 3, 7);

    Plaintext plain_1, plain_res;
    encoder.encode(vector_1, scale, plain_1);
    //encoder.encode(complex<double>(1, 1), scale, plain_1);

    Ciphertext cipher_1, cipher_res;
    encryptor.encrypt(plain_1, cipher_1);
    // print_ciphertext<double_t>(context, cipher_1, encoder, decryptor, 20, 7);


    // Evaluate (ct^4).
    evaluator.square_inplace(cipher_1);
    evaluator.relinearize_inplace(cipher_1, relin_keys);
    evaluator.rescale_to_next_inplace(cipher_1);
    evaluator.square_inplace(cipher_1);
    evaluator.relinearize_inplace(cipher_1, relin_keys);
    evaluator.rescale_to_next_inplace(cipher_1);
    // print_ciphertext<double_t>(context, cipher_1, encoder, decryptor, encoder.slot_count(), 15);


    // Bootstrap.
    bootstrapper.bootstrapping(cipher_1, encoder, evaluator, relin_keys, galois_keys, delta_bit_size, encrypted_level, d_0, r, cipher_res, true);
    // print_ciphertext<double_t>(context, cipher_res, encoder, decryptor, encoder.slot_count(), 15);


    return 0;
}