// Boost.Convert test and usage example
// Copyright (c) 2009-2014 Vladimir Batov.
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. See http://www.boost.org/LICENSE_1_0.txt.

#include <boost/convert/detail/forward.hpp>

#ifdef BOOST_CONVERT_BROKEN_COMPILER
int main(int, char const* []) { return 0; }
#else

#include <boost/convert.hpp>
#include <boost/convert/lexical_cast.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "./test.hpp"

using std::string;
using boost::convert;
using boost::lexical_cast;

//[callable_example1
void plain_old_func(string const& value_in, boost::optional<int>& value_out)
//]
{
    try
    {
        value_out = lexical_cast<int>(value_in);
    }
    catch (...)
    {
    }
}

template<typename TypeIn, typename TypeOut>
void
convert_all(TypeIn const&, boost::optional<TypeOut>&)
{
}

template<typename Type>
void
assign(boost::optional<Type>& value_out, Type const& value_in)
{
    value_out = value_in;
}

struct converter1
{
    template<typename TypeIn, typename TypeOut>
    void
    operator()(TypeIn const&, boost::optional<TypeOut>&) const
    {
    }
};

//[callable_example4
struct take_double { void operator()(double const&, boost::optional<string>&) const {}};
struct    take_int { void operator()(int const&, boost::optional<string>&) const {}};
//]

//[callable_example6
struct double_only
{
    // Declared for all types.
    template<typename TypeIn> void operator()(TypeIn const&, boost::optional<string>&) const;
};

// Defined only for certain types.
template<> void double_only::operator()<double>(double const&, boost::optional<string>&) const {}
//]

int
main(int argc, char const* argv[])
{
    typedef boost::function<void (string const& value_in, boost::optional<int>&)> boost_func;

    char const* const str = "-12";

    // Testing old-function-based converter.
    //[callable_example2
    int v01 = convert<int>(str, plain_old_func).value_or(-1);
    //]
    // Testing boost::function-based converter.
    int v02 = convert<int>(str, boost_func(plain_old_func)).value_or(-1);
    // Testing crazy boost::bind-based converter.
    //[callable_example3
    int v03 = convert<int>(str,
                  boost::bind(assign<int>, _2,
                      boost::bind(lexical_cast<int, string>, _1))).value_or(-1);
    //]
    BOOST_TEST(v01 == -12);
    BOOST_TEST(v02 == -12);
    BOOST_TEST(v03 == -12);

    convert<int>(str, convert_all<string, int>);
    convert<string>(11, convert_all<int, string>);
    convert<   int>(str,   converter1());
    convert<string>(11,    converter1());
    convert<string>(11.23, take_double());
    convert<string>(11,    take_int());
    //[callable_example5
    convert<string>(11, take_double()); // Compiler applies 'int' to 'double' promotion to call the converter.
    convert<string>(11.23, take_int()); // Compiler applies 'double' to 'int' implicit truncation.
    //]
    //[callable_example7
    convert<string>(11.23, double_only()); // Fine.
//  convert<string>(11,    double_only()); // Fails: undefined reference to `void double_only::operator()<int>
    //]

    return boost::report_errors();
}

#endif
