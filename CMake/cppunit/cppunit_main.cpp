/************************************************************************
 *
 * Copyright (C) 2004-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <stdexcept>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#ifdef BUNDLE_TEST_PROFILE

#include <filesystem>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/io/ProfileReader.hpp>

class MiniLauncher
{
public:
    MiniLauncher( std::filesystem::path profilePath )
    {
        ::fwRuntime::Runtime* runtime = ::fwRuntime::Runtime::getDefault();
        runtime->addDefaultBundles();

        const std::filesystem::path cwd = runtime->getWorkingPath();

        if (!std::filesystem::exists( profilePath ))
        {
            profilePath = cwd / profilePath;
        }

        if (!std::filesystem::exists( profilePath ))
        {
            throw (std::invalid_argument("<" + profilePath.string() + "> not found." ));
        }

        m_profile = ::fwRuntime::io::ProfileReader::createProfile(profilePath);
        ::fwRuntime::profile::setCurrentProfile(m_profile);

        m_profile->setParams(0, NULL);
        m_profile->start();
        m_profile->setup();
    }

    ~MiniLauncher()
    {
        m_profile->cleanup();
        m_profile->stop();
        m_profile.reset();
        ::fwRuntime::profile::setCurrentProfile(m_profile);
    }

private:
    ::fwRuntime::profile::Profile::sptr m_profile;

};

#endif

struct Options
{
    bool verbose;
    bool xmlReport;
    bool listTests;
    std::string xmlReportFile;
    std::vector< std::string > testsToRun;

#ifdef BUNDLE_TEST_PROFILE
    std::string profile;
#endif

    Options() :
        verbose(false),
        xmlReport(false),
        listTests(false)
#ifdef BUNDLE_TEST_PROFILE
        ,
        profile( BUNDLE_TEST_PROFILE )
#endif
    {
    }

    //------------------------------------------------------------------------------

    bool parse(int argc, char* argv[])
    {
        if (argc < 1)
        {
            return true;
        }

        const std::string programName( *argv != 0 ? *argv : "test_runner" );

        char** args    = argv + 1;
        char** argsEnd = argv + argc;
        while (args < argsEnd)
        {
            std::string arg(*args);

            if(arg == "--help" || arg == "-h")
            {
                std::cout
                    << "usage : " << programName << " "
                    << "[--help|-h] [--verbose|-v] [--xml|-x] [-o FILE] [--list|-l] [test1 ... testN]"
                    << std::endl
                    << "    -h,--help         Shows this help" << std::endl
                    << "    -s,--verbose      Shows each run test name and it status" << std::endl
                    << "    -x,--xml          Output results to a xml file" << std::endl
                    << "    -o FILE           Specify xml file name" << std::endl
                    << "    -l,--list         Lists test names" << std::endl
#ifdef BUNDLE_TEST_PROFILE
                    << "    -p,--profile      Profile to launch for bundle tests" << std::endl
#endif
                    << "    test1 ... testN   Test names to run" << std::endl
                    << std::endl;
                return false;
            }
            else if( arg == "--verbose" || arg == "-v")
            {
                this->verbose = true;
            }
            else if( arg == "--xml" || arg == "-x")
            {
                this->xmlReport = true;
            }
            else if( arg == "-o")
            {
                args++;
                if(args >= argsEnd)
                {
                    std::cerr << "value for -o is missing" << std::endl;
                    return false;
                }
                this->xmlReportFile = std::string(*args);
            }
            else if( arg == "--list" || arg == "-l")
            {
                this->listTests = true;
            }
#ifdef BUNDLE_TEST_PROFILE
            else if( arg == "--profile" || arg == "-p")
            {
                args++;
                if(args >= argsEnd)
                {
                    std::cerr << "value for -p/--profile is missing" << std::endl;
                    return false;
                }
                this->profile = std::string(*args);
            }
#endif
            else
            {
                this->testsToRun.push_back(arg);
            }

            args++;
        }
        return true;
    }

};

//------------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    Options options;

    const std::string testExecutable = (argc >= 1) ? std::string(argv[0]) : "unknown";
    options.xmlReportFile = testExecutable + "-cppunit-report.xml";

    if (!options.parse(argc, argv))
    {
        return 1;
    }

#ifdef BUNDLE_TEST_PROFILE
    MiniLauncher miniLaucher( options.profile );
#endif

    CPPUNIT_NS::Test* testSuite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest( testSuite );

    if( options.listTests )
    {
        CPPUNIT_NS::TestResult result;
        runner.run( result );
        return 0;
    }

    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener( &result );

    // Listener that prints the name of each test before running it.
    CPPUNIT_NS::BriefTestProgressListener BriefProgress;

    // Listener that print dots as test run.
    CPPUNIT_NS::TextTestProgressListener textProgress;

    if(options.verbose)
    {
        controller.addListener( &BriefProgress );
    }
    else
    {
        controller.addListener( &textProgress );
    }

    if(options.testsToRun.empty())
    {
        options.testsToRun.push_back("");
    }

    for(const std::string& test : options.testsToRun)
    {
        try
        {
            runner.run( controller, test );
        }
        catch ( std::exception& e )
        {
            std::cerr << "[" << ((test.empty()) ? "All tests" : test) << "]" << "Error: " << e.what() << std::endl;
            return 1;
        }
        catch ( ... )
        {
            std::cerr << "[" << ((test.empty()) ? "All tests" : test) << "]" << "Unexpected error. " << std::endl;
            return 1;
        }
    }

    // Print test results in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
    outputter.write();

    if(options.xmlReport)
    {
        std::ofstream file( options.xmlReportFile.c_str() );
        CPPUNIT_NS::XmlOutputter xml( &result, file );
        xml.write();
        file.close();
    }

    if (result.testFailuresTotal())
    {
        return 1;
    }

    return 0;
}
