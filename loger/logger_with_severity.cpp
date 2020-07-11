#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
//#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
// #include <boost/log/sources/record_ostream.hpp>

#include <iostream>
#include <boost/chrono.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/thread.hpp> 


enum severity_level
{
    sev1,
    sev2,
    sev3,
    sev4,
    sev5
};
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);


int main(int, char*[])
{

    /* boost logger is thread safe! */

    // set filters
    boost::log::core::get()->set_filter(severity > sev2);

    boost::log::sources::severity_logger<severity_level> slog;

    BOOST_LOG_SEV(slog, sev1) << "hello world 1";
    BOOST_LOG_SEV(slog, sev2) << "hello world 2";
    BOOST_LOG_SEV(slog, sev3) << "hello world 3";
    BOOST_LOG_SEV(slog, sev4) << "hello world 4";
    BOOST_LOG_SEV(slog, sev5) << "hello world 5";

    return 0;
}