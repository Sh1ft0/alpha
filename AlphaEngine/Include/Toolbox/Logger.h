#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <sstream>

#include <memory>
#include <mutex>

#include <chrono>
#include <ctime>

namespace alpha
{
#ifdef ALPHA_DEBUG

    enum Severity
    {
        SDEBUG,
        SWARNING,
        SERROR,
    };

    class Logger
    {
    public:
        explicit Logger(const std::string & path)
            : m_path(path)
            , m_lineNumber(0)
        {
            // remove the file if it already exists, so we can start a fresh log
            remove(path.c_str());
        }
        virtual ~Logger() { }

        template<Severity severity, typename...Args>
        void print( Args...args )
        {
            // take write lock, and open file for appending only
            m_writeMutex.lock();
            m_outStream.open(m_path.c_str(), std::ios_base::app | std::ios_base::out);

            //if (m_outStream.open)
            {
                m_outStream << this->GetTimestamp();

                switch (severity)
                {
                    case Severity::SDEBUG:
                        m_outStream << "[  DEBUG  ] : ";
                        break;
                    case Severity::SWARNING:
                        m_outStream << "[*WARNING*] : ";
                        break;
                    case Severity::SERROR:
                        m_outStream << "[!!ERROR!!] : ";
                        break;
                }

                print_impl(args...);
            }
            m_outStream.close();

            m_writeMutex.unlock();
        }

    private:
        void print_impl()
        {
            // finish with a newline, so it logs cleanly
            m_outStream << std::endl;
        }
         
        template<typename First, typename...Rest>
        void print_impl(First param1, Rest...rest)
        {
            m_outStream << param1;
            print_impl(rest...);    
        }

        std::string GetTimestamp()
        {
            auto now = std::chrono::system_clock::now();
            auto now_time = std::chrono::system_clock::to_time_t(now);

            struct tm * timeinfo;
            timeinfo = std::localtime(&now_time);

            char buf[10];
            std::strftime(buf, 10, "%H:%M:%S", timeinfo);

            return std::string(buf);
        }

        const std::string m_path;

        unsigned int m_lineNumber;
        std::mutex m_writeMutex;
        
        std::ofstream m_outStream;
    };

    static Logger s_logger("alpha.log");

    #define LOG s_logger.print<Severity::SDEBUG>
    #define LOG_ERR s_logger.print<Severity::SERROR>
    #define LOG_WARN s_logger.print<Severity::SWARNING>

#else

    #define LOG(...)
    #define LOG_ERR(...)
    #define LOG_WARN(...)

#endif

}

#endif // LOGGER_H
