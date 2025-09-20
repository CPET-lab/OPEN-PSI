#ifndef FPSI_H_
#define FPSI_H_

# include "util.h"
# include "DASetAug.h"

class FuzzyPSI {
public:
    FuzzyPSI();

    /**
     * @brief Constructor with parameters
     * @param sender_range sender's psi range (start, end)
     * @param receiver_value receiver's integer psi value
     * @param threshold threshold value
     * @param modulus modulus value
     */

    explicit FuzzyPSI(pair<int64_t, int64_t> sender_range, int64_t receiver_value, int64_t threshold, int64_t modulus);

    inline void print_info() const {
        cout << "Sender range: (" << sender_range_.first << ", " << sender_range_.second << ")" << endl;
        cout << "Receiver value: " << receiver_value_ << endl;
        cout << "Threshold: " << threshold_ << endl;
        cout << "Modulus: " << modulus_ << endl;
    }

    int64_t fuzzy_fast_one_to_one_matching() const;
    
    // destructor
    ~FuzzyPSI();


private:
    pair<int64_t, int64_t> sender_range_;
    int64_t receiver_value_;
    int64_t threshold_;
    int64_t modulus_;
}; 


#endif