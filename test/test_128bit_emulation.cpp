// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/charconv/detail/emulated128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <limits>
#include <cstdint>

void native_128_test()
{
    auto r1 = boost::charconv::detail::full_multiplication(1, 1);
    BOOST_TEST_EQ(r1.high, 0);
    BOOST_TEST_EQ(r1.low, 1);

    auto r2 = boost::charconv::detail::full_multiplication(10, std::numeric_limits<std::uint64_t>::max());
    BOOST_TEST_EQ(r2.high, 9);
    BOOST_TEST_EQ(r2.low, UINT64_C(18446744073709551606));
}

int main(void)
{
    native_128_test();
    return boost::report_errors();
}
