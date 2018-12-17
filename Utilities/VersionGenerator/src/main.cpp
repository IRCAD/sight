/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <fwAtomsPatch/VersionsManager.hpp>

/** \file VersionGenerator/src/main.cpp
 *
 *********************
 * Software : VersionGenerator
 *********************
 * Allows to generate a file with .versions extesion
 * This file contains the listof the data and theirs version.
 * HELP  : VersionGenerator.exe --help
 * USE :   VersionGenerator.exe \<options\>
 * Allowed options:
 *   -h [ --help ]           produce help message
 *   -o [ --output ] arg     set the output file
 *   -f [ --context ] arg     set context name. It will be register in the generated file.
 *   -v [ --version ] arg    set version name. It will be register in the generated file.
 */

int main(int argc, char** argv)
{
    // Declare the supported options.
    ::boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("output,o", ::boost::program_options::value< std::string >(), "set output file")
        ("context,f", ::boost::program_options::value< std::string >(), "set context name")
        ("version,v", ::boost::program_options::value< std::string >(), "set version name")
    ;

    // Manage the options
    ::boost::program_options::variables_map vm;
    ::boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    ::boost::program_options::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }
    else if(!vm.count("context"))
    {
        std::cout << "You must specify a context name." << "\n";
        return EXIT_FAILURE;
    }
    else if(!vm.count("version"))
    {
        std::cout << "You must specify a version name." << "\n";
        return EXIT_FAILURE;
    }
    else if(!vm.count("output"))
    {
        std::cout << "You must specify a output file." << "\n";
        return EXIT_FAILURE;
    }

    // Generate the result
    ::boost::filesystem::path versionFile(vm["output"].as< std::string >());
    ::fwAtomsPatch::VersionsManager::generateNewFile(versionFile, vm["context"].as< std::string >(),
                                                     vm["version"].as< std::string >());

    return EXIT_SUCCESS;
}

