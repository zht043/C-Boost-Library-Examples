#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
//#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/attributes.hpp>
// #include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
//#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

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

int main(int, char*[])
{
    /* boost logger is thread safe! */

    // create logging sink
    typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
    
    // set where to log to
    sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
    // set filters
    sink->set_filter(severity > sev2);
    // set format
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

    
    boost::log::core::get()->add_sink(sink);
    // boost::log::add_common_attributes();

    // construct the logger
    boost::log::sources::severity_logger<severity_level> slog;
    
    BOOST_LOG_SEV(slog, sev1) << "hello world 1";
    BOOST_LOG_SEV(slog, sev2) << "hello world 2";
    BOOST_LOG_SEV(slog, sev3) << "hello world 3";
    BOOST_LOG_SEV(slog, sev4) << "hello world 4";

    slog.add_attribute(LOG_TAG("Tag1"));
    BOOST_LOG_SEV(slog, sev5) << "hello world 5";

    return 0;
}