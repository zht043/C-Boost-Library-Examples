#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
//#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/attributes.hpp>
// #include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/log/attributes/scoped_attribute.hpp>

#include <iostream>
#include <boost/chrono.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/thread.hpp> 



unsigned int millis(void) {
    auto t = boost::chrono::high_resolution_clock::now();
    return (unsigned int)(double(t.time_since_epoch().count()) / 1000000.00f);
}

unsigned int micros(void) {
    auto t = boost::chrono::high_resolution_clock::now();
    return (unsigned int)(double(t.time_since_epoch().count()) / 1000.00f);
}


enum severity_level
{
    sev1,
    sev2,
    sev3,
    sev4,
    sev5
};
std::ostream& operator<< (std::ostream& strm, severity_level level)
{
    static const char* strings[] =
    {
        "sev1",
        "sev2",
        "sev3",
        "sev4",
        "sev5"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);

    return strm;
}

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);

BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string);
#define LOG_TAG(str) "Tag", boost::log::attributes::constant< std::string >(str)


BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timeline, "Timeline", boost::log::attributes::timer::value_type)

int main(int, char*[])
{
    /* boost logger is thread safe! */

    // create logging sink
    typedef boost::log::sinks::asynchronous_sink< boost::log::sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
    
    // set where to log to
    sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
    // set filters
    sink->set_filter(severity > sev2);
    // set format
    sink->set_formatter
    (
        boost::log::expressions::stream
            << std::hex << std::setw(8) << std::setfill('0') << line_id << std::dec << std::setfill(' ')
            << ": <" << severity << ">\t"
            << boost::log::expressions::if_(boost::log::expressions::has_attr(timeline))
               [
                    boost::log::expressions::stream << "[" << timeline << "] "
               ]
            << boost::log::expressions::if_(boost::log::expressions::has_attr(tag_attr))
               [
                    boost::log::expressions::stream << "[" << tag_attr << "] "
               ]
            << boost::log::expressions::smessage
    );

    
    BOOST_LOG_SCOPED_THREAD_ATTR("Timeline", boost::log::attributes::timer());
    boost::log::add_common_attributes();
    boost::log::core::get()->add_sink(sink);

    // construct the logger
    boost::log::sources::severity_logger<severity_level> slog;

/*
    sink->set_formatter
    (
        boost::log::expressions::stream
            << "<" << severity << ">\t"
            << boost::log::expressions::if_(boost::log::expressions::has_attr(tag_attr))
               [
                    boost::log::expressions::stream << "[" << tag_attr << "] "
               ]
            << boost::log::expressions::smessage
    );
*/
    
    BOOST_LOG_SEV(slog, sev1) << "hello world 1";
    BOOST_LOG_SEV(slog, sev2) << "hello world 2";
    BOOST_LOG_SEV(slog, sev3) << "hello world 3";
    BOOST_LOG_SEV(slog, sev4) << "hello world 4";

    auto t0 = micros();
    slog.add_attribute(LOG_TAG("Tag1"));
    BOOST_LOG_SEV(slog, sev5) << "hello world 5";
    BOOST_LOG_SEV(slog, sev5) << "hello world 6";
    auto dt = micros() - t0;
    std::cout << dt << std::endl;


    t0 = millis();
    while(millis() - t0 < 1000);

    return 0;
}