// Copyright (c) 2009-2014 Vladimir Batov.
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. See http://www.boost.org/LICENSE_1_0.txt.

#include "./example.hpp"
#include <boost/convert.hpp>
#include <boost/convert/stream.hpp>
#include <boost/lexical_cast.hpp>

using std::string;
using boost::convert;
using boost::lexical_cast;

static void process_failure() {}
static void log(...) {}
static int fallback_function() { return -1; }

static
void
example1()
{
    boost::cnv::cstream  cnv;
    std::string const    str = "123";
    std::string const   str1 = "123";
    std::string const   str2 = "123";
    std::string const   str3 = "123";
    int const fallback_value = -1;

    {
        //[getting_serious_example1
        int i2 = convert<int>("not an int", cnv).value_or(-1); // after the call i2==-1

        if (i2 == -1) process_failure();
        //]
    }
    {
        //[getting_serious_example2
        try
        {
            int i1 = lexical_cast<int>(str); // Throws when fails
            int i2 = convert<int>(str, cnv).value(); // Throws when fails
        }
        catch (...)
        {
            process_failure();
        }
        //]
    }
    {
        //[getting_serious_example3
        boost::optional<int> r1 = convert<int>(str1, cnv); // Does not throw
        boost::optional<int> r2 = convert<int>(str2, cnv); // Does not throw
        // ...
        try // Delayed processing of potential exceptions
        {
            int i1 = r1.value(); // Will throw if conversion failed
            int i2 = r2.value(); // Will throw if conversion failed
        }
        catch (boost::bad_optional_access const&)
        {
            // Handle failed conversion
        }

        // Exceptions are avoided altogether
        int i1 = r1 ? r1.value() : fallback_value;
        int i2 = r2.value_or(fallback_value);
        int i3 = convert<int>(str3, cnv).value_or(fallback_value);
        int i4 = convert<int>(str3, cnv).value_or_eval(fallback_function);
        //]
    }
}

//[getting_serious_example5
struct fallback_func
{
    int operator()() const { log("Failed to convert"); return 22; }
};
//]

static
void
example4()
{
    boost::cnv::cstream  cnv;
    std::string const    str = "123";
    int const fallback_value = -1;
    //[getting_serious_example4
    boost::optional<int> res = boost::convert<int>(str, cnv);

    if (!res) log("str conversion failed");

    int i1 = res.value_or(fallback_value);

    // ...proceed
    //]
    //[getting_serious_example6
    // Fallback function is called when failed
    int i2 = convert<int>(str, cnv).value_or_eval(fallback_func());
    int i3 = convert<int>(str, cnv, fallback_func());
    //]
}
static
void
example5()
{
    boost::cnv::cstream  cnv;
    std::string const    str = "123";
    int const fallback_value = -1;
    //[getting_serious_example7
    // Error-processing behavior are specified clearly and uniformly.
    // a) i1: Returning the provided fallback value;
    // b) i2: Calling the provided failure-processing function;
    // c) i3: Throwing an exception.

    int i1 = convert<int>(str, cnv, fallback_value);
    int i2 = convert<int>(str, cnv, fallback_func());

    try
    {
        // Throwing behavior specified explicitly rather than implied
        int i3 = convert<int>(str, cnv, boost::throw_on_failure);
    }
    catch (boost::bad_optional_access const&)
    {
        // Handle failed conversion
    }
    //]
}

void
example::getting_serious()
{
    example1();
    example4();
    example5();
}
