/************************************************************************
 *
 * Copyright (C) 2004-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/log/SpyLogger.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/profile/Profile.hpp>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/XmlOutputter.h>

#include <filesystem>

struct Options
{
    bool verbose;
    bool xmlReport;
    bool listTests;
    std::string xmlReportFile;
    std::vector<std::string> testsToRun;

    Options() :
        verbose(false),
        xmlReport(false),
        listTests(false)
    {
    }

    //------------------------------------------------------------------------------

    bool parse(int argc, char* argv[])
    {
        if(argc < 1)
        {
            return true;
        }

        const std::string programName(*argv != 0 ? *argv : "test_runner");

        char** args    = argv + 1;
        char** argsEnd = argv + argc;
        while(args < argsEnd)
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
                << "    test1 ... testN   Test names to run" << std::endl
                << std::endl;
                return false;
            }
            else if(arg == "--verbose" || arg == "-v")
            {
                this->verbose = true;
            }
            else if(arg == "--xml" || arg == "-x")
            {
                this->xmlReport = true;
            }
            else if(arg == "-o")
            {
                args++;
                if(args >= argsEnd)
                {
                    std::cerr << "value for -o is missing" << std::endl;
                    return false;
                }

                this->xmlReportFile = std::string(*args);
            }
            else if(arg == "--list" || arg == "-l")
            {
                this->listTests = true;
            }
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

void init_log_output()
{
    auto& logger = sight::core::log::SpyLogger::get();

    std::string logFile = "fwTest.log";

    FILE* pFile = fopen(logFile.c_str(), "w");
    if(pFile == NULL)
    {
        std::error_code err;
        std::filesystem::path sysTmp = std::filesystem::temp_directory_path(err);
        if(err.value() != 0)
        {
            // replace log file appender by stream appender: current dir and temp dir unreachable
            logger.add_console_log();
        }
        else
        {
            // creates fwTest.log in temp directory: current dir unreachable
            sysTmp  = sysTmp / logFile;
            logFile = sysTmp.string();
            logger.add_file_log(logFile);
        }
    }
    else
    {
        // creates fwTest.log in the current directory
        fclose(pFile);
        logger.add_file_log(logFile);
    }
}

//------------------------------------------------------------------------------

void init_runtime()
{
    // This variable is set when configuring this file in the fw_test() CMake macro
    static const std::string moduleName = "@TESTED_MODULE@";
    if(!moduleName.empty())
    {
        SIGHT_INFO("Automatic loading of module '" + moduleName + "'");
        sight::core::runtime::init();
        sight::core::runtime::loadModule(moduleName);
    }
}

//------------------------------------------------------------------------------

void shutdown_runtime()
{
    // This variable is set when configuring this file in the fw_test() CMake macro
    static const std::string moduleName = "@TESTED_MODULE@";
    if(!moduleName.empty())
    {
        sight::core::runtime::shutdown();
    }
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    init_log_output();
    init_runtime();

    Options options;

    const std::string testExecutable = (argc >= 1) ? std::string(argv[0]) : "unknown";
    options.xmlReportFile = testExecutable + "-cppunit-report.xml";

    if(!options.parse(argc, argv))
    {
        return 1;
    }

    CPPUNIT_NS::Test* testSuite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

    if(options.listTests)
    {
        for(int i = 0 ; i < testSuite->getChildTestCount() ; ++i)
        {
            std::cout << testSuite->getChildTestAt(i)->getName() << std::endl;
        }

        return 0;
    }

    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest(testSuite);

    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that collects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);

    // Listener that prints the name of each test before running it.
    CPPUNIT_NS::BriefTestProgressListener BriefProgress;

    // Listener that print dots as test run.
    CPPUNIT_NS::TextTestProgressListener textProgress;

    if(options.verbose)
    {
        controller.addListener(&BriefProgress);
    }
    else
    {
        controller.addListener(&textProgress);
    }

    if(options.testsToRun.empty())
    {
        options.testsToRun.push_back("");
    }

    for(const std::string& test : options.testsToRun)
    {
        try
        {
            runner.run(controller, test);
        }
        catch(std::exception& e)
        {
            std::cerr << "[" << ((test.empty()) ? "All tests" : test) << "]" << "Error: " << e.what() << std::endl;
            return 1;
        }
        catch(...)
        {
            std::cerr << "[" << ((test.empty()) ? "All tests" : test) << "]" << "Unexpected error. " << std::endl;
            return 1;
        }
    }

    shutdown_runtime();

    // Print test results in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter(&result, std::cerr);
    outputter.write();

    if(options.xmlReport)
    {
        std::ofstream file(options.xmlReportFile.c_str());
        CPPUNIT_NS::XmlOutputter xml(&result, file);
        xml.write();
        file.close();
    }

    if(result.testFailuresTotal())
    {
        return 1;
    }

    return 0;
}
