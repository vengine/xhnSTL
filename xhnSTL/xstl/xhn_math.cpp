
#include "xhn_math.hpp"

namespace xhn {

euint64 factorial_table[21] =
{
    1,
    1,
    2,
    6,
    24,
    120,
    720,
    5040,
    40320,
    362880,
    3628800,
    39916800,
    479001600,
    6227020800,
    87178291200,
    1307674368000,
    20922789888000,
    355687428096000,
    6402373705728000,
    121645100408832000,
    2432902008176640000,
};
    
#if MAX_INT_SIZE > 64
euint128 factorial_table128[35] =
{
    1,
    1,
    2,
    6,
    24,
    120,
    720,
    5040,
    40320,
    362880,
    3628800,
    39916800,
    479001600,
    6227020800,
    87178291200,
    1307674368000,
    20922789888000,
    355687428096000,
    6402373705728000,
    121645100408832000,
    2432902008176640000,
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26) *
        static_cast<euint128>(27),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26) *
        static_cast<euint128>(27) *
        static_cast<euint128>(28),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26) *
        static_cast<euint128>(27) *
        static_cast<euint128>(28) *
        static_cast<euint128>(29),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26) *
        static_cast<euint128>(27) *
        static_cast<euint128>(28) *
        static_cast<euint128>(29) *
        static_cast<euint128>(30),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26) *
        static_cast<euint128>(27) *
        static_cast<euint128>(28) *
        static_cast<euint128>(29) *
        static_cast<euint128>(30) *
        static_cast<euint128>(31),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26) *
        static_cast<euint128>(27) *
        static_cast<euint128>(28) *
        static_cast<euint128>(29) *
        static_cast<euint128>(30) *
        static_cast<euint128>(31) *
        static_cast<euint128>(32),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26) *
        static_cast<euint128>(27) *
        static_cast<euint128>(28) *
        static_cast<euint128>(29) *
        static_cast<euint128>(30) *
        static_cast<euint128>(31) *
        static_cast<euint128>(32) *
        static_cast<euint128>(33),
    static_cast<euint128>(2432902008176640000) *
        static_cast<euint128>(21) *
        static_cast<euint128>(22) *
        static_cast<euint128>(23) *
        static_cast<euint128>(24) *
        static_cast<euint128>(25) *
        static_cast<euint128>(26) *
        static_cast<euint128>(27) *
        static_cast<euint128>(28) *
        static_cast<euint128>(29) *
        static_cast<euint128>(30) *
        static_cast<euint128>(31) *
        static_cast<euint128>(32) *
        static_cast<euint128>(33) *
        static_cast<euint128>(34),
};
#endif

}
