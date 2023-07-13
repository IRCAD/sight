/************************************************************************
 *
 * Copyright (C) 2004-2023 IRCAD France
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

// cspell:ignore NOLINT SEM_NOGPFAULTERRORBOX

#ifdef _WIN32
#include <cstdlib>
#include <windows.h>
#endif

#include <core/log/SpyLogger.hpp>
#include <core/runtime/runtime.hpp>

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
    bool verbose {false};
    bool xmlReport {false};
    bool listTests {false};
    std::string xmlReportFile;
    std::vector<std::string> testsToRun;

    Options() = default;

    //------------------------------------------------------------------------------

    bool parse(const std::vector<char*>& argv)
    {
        if(argv.empty())
        {
            return true;
        }

        const std::string programName(argv[0] != nullptr ? argv[0] : "test_runner");

        auto args    = argv.begin() + 1;
        auto argsEnd = argv.end();
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
                << "    -v,--verbose      Shows each run test name and it status" << std::endl
                << "    -x,--xml          Output results to a xml file" << std::endl
                << "    -o FILE           Specify xml file name" << std::endl
                << "    -l,--list         Lists test names" << std::endl
                << "    test1 ... testN   Test names to run" << std::endl
                << std::endl;
                return false;
            }

            if(arg == "--verbose" || arg == "-v")
            {
                this->verbose = true;
            }
            else if(arg == "--xml" || arg == "-x")
            {
                this->xmlReport = true;
            }
            else if(arg == "-o")
            {
                ++args;
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
            else if(arg == "-B")
            {
                ++args;
                if(args >= argsEnd)
                {
                    std::cerr << "value for -B is missing" << std::endl;
                    return false;
                }

                const std::filesystem::path externalBundel {std::string(*args)};
                if(!std::filesystem::exists(externalBundel) || !std::filesystem::is_directory(externalBundel))
                {
                    std::cerr << "The external bundle provided in argument is not a consistent directory : "
                    << externalBundel.string() << std::endl;
                    return false;
                }

                sight::core::runtime::addModules(externalBundel);
            }
            else
            {
                this->testsToRun.push_back(arg);
            }

            ++args;
        }

        return true;
    }
};

//------------------------------------------------------------------------------

void init_log_output()
{
    std::string logFile = "fwTest.log";

    FILE* pFile = fopen(logFile.c_str(), "w");
    if(pFile == nullptr)
    {
        std::error_code err;
        std::filesystem::path sysTmp = std::filesystem::temp_directory_path(err);
        if(err.value() != 0)
        {
            // replace log file appender by stream appender: current dir and temp dir unreachable
            sight::core::log::SpyLogger::add_console_log();
        }
        else
        {
            // creates fwTest.log in temp directory: current dir unreachable
            sysTmp  = sysTmp / logFile;
            logFile = sysTmp.string();
            sight::core::log::SpyLogger::add_file_log(logFile);
        }
    }
    else
    {
        // creates fwTest.log in the current directory
        if(fclose(pFile) != 0)
        {
            perror("fclose");
        }

        sight::core::log::SpyLogger::add_file_log(logFile);
    }
}

//------------------------------------------------------------------------------

void init_runtime()
{
    // This variable is set when configuring this file in the fw_test() CMake macro
    static const std::string moduleName = "@TESTED_MODULE@"; // NOLINT(readability-redundant-string-init)
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
    static const std::string moduleName = "@TESTED_MODULE@"; // NOLINT(readability-redundant-string-init)
    if(!moduleName.empty())
    {
        sight::core::runtime::shutdown();
    }
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
#ifdef _WIN32
    // This allows to disable debug dialogs on Windows Debug during unit tests.
    // It is especially useful in the CI, where these debug dialogs block the process and causes a
    // timeout, and makes it impossible to find out the problem if physical access isn't possible.
    // The messages are logged in the console instead.
    if(const char* disableAbortDialog = std::getenv("DISABLE_ABORT_DIALOG"); disableAbortDialog != nullptr)
    {
        _set_error_mode(_OUT_TO_STDERR);
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    }
#endif

    init_log_output();
    init_runtime();

    Options options;

    const std::string testExecutable = (argc >= 1) ? std::string(argv[0]) : "unknown";
    options.xmlReportFile = testExecutable + "-cppunit-report.xml";

    if(!options.parse(std::vector(argv, argv + argc)))
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
        options.testsToRun.emplace_back();
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

    if(result.testFailuresTotal() != 0)
    {
        return 1;
    }

    return 0;
}
