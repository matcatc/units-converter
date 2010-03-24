/*
 *
 converter: converts between different unit measurements
 Copyright (C) 2009  Matthew A. Todd

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * main.cpp
 *
 *  Created on: Aug 18, 2009
 *      Author: Matthew A. Todd
 */

#include <iostream>
#include <sstream>

// for boost's program option handling
#include <boost/program_options.hpp>
#include <iterator>
namespace po = boost::program_options;

#include "convert.h"

LogFile logger;

int main(int ac, char* av[])
{

    try
    {
        string start, end, file;
        double initialValue;

        po::options_description desc("Allowed options");

        desc.add_options()("help,h", "produce help message")("version,V",
                "produce version/license information")("start,s", po::value<
                string>(), "starting unit")("end,e", po::value<string>(),
                "ending unit")("value,v", po::value<double>(),
                "value/amount of starting unit")("file,f",
                po::value<string>(), "file containing conversion values")(
                "list,l", "list possible units");

        po::positional_options_description pd;
        pd.add("value", 1).add("start", 1).add("end", 1);

        po::variables_map vm;
        po::store(
                po::command_line_parser(ac, av).options(desc).positional(pd).run(),
                vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            cout << "Simple usage:\n"
                "converter value start_unit end_unit" << endl;
            cout << "\n\nFor more information, please visit:\n"
                "http://one.xthost.info/matcat/programs/math/converter.html\n"
                "or email me at matcatprg@yahoo.com" << endl;
            return 1;
        }

        if (vm.count("version"))
        {
            cout
                    << "converter: converts between different unit measurements\n"
                        "Copyright (C) 2009  Matthew A. Todd\n"
                        "\n"
                        "This program is free software: you can redistribute it and/or modify\n"
                        "it under the terms of the GNU General Public License as published by\n"
                        "the Free Software Foundation, either version 3 of the License, or\n"
                        "(at your option) any later version.\n"
                        "\n"
                        "This program is distributed in the hope that it will be useful,\n"
                        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                        "GNU General Public License for more details.\n"
                        "\n"
                        "You should have received a copy of the GNU General Public License\n"
                        "along with this program.  If not, see <http://www.gnu.org/licenses/>."
                    << endl;
            return 1;
        }

        if (vm.count("file"))
        {
            file = vm["file"].as<string> ();
        }
        else
        {
            file = DEFAULT_CONVERT_FILE;
        }

        if (vm.count("list"))
        {
            Convert temp(file);
            temp.printList(cout);
            return 1;
        }

        if (vm.count("start"))
        {
            start = vm["start"].as<string> ();
        }
        if (vm.count("end"))
        {
            end = vm["end"].as<string> ();
        }
        if (vm.count("value"))
        {
            initialValue = vm["value"].as<double> ();
        }
        // END boost::program_options

        logger.Log(LOG_LEVELS::command_line_input, "start unit = " + start
                + ", end unit = " + end + ", amount = "
                + boost::lexical_cast<string>(initialValue));

        double value = initialValue;
        Convert conv(start, end, value, file);

        cout << initialValue << " " << start << " = " << value << " " << end
                << "(s)" << endl;
    }
    catch (exception & e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
