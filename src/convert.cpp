/*
 *
 *
 * converter: converts between different unit measurements
 * Copyright (C) 2009  Matthew A. Todd
 *
 * This file is part of Converter.
 *
 * Converter is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Converter is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Converter. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 * convert.cpp
 *
 *  Created on: Aug 18, 2009
 *      Author: Matthew A. Todd
 */

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "convert.h"

#define COMMENT '#'

/**
 * used by Convert::initVertexs
 *
 * hidden data (i.e: in source file)
 */
struct convData
{
        string end;
        float value;
        convData(const string & gEnd, const float gValue) :
            end(gEnd), value(gValue)
        {
        }
};

/**
 * @see initVertexs(ifstream)
 */
void Convert::initVertexs(const string & filename)
{
    ifstream file(filename.c_str(), ios::in);
    if (!file.good())
    {
        logger.Log(LOG_LEVELS::init_vertex, "couldn't open file " + filename);
        throw ERROR::BAD_INPUT_FILE;
    }
    else
        logger.Log(LOG_LEVELS::init_vertex, "reading in from " + filename);
    initVertexs(file);
    file.close();
}

/**
 * function reads data from file. Lines can be commented using '#' (which is
 * defined in the COMMENT macro. Function is a loop that enters/leaves the body or
 * "inside" of the conversion data upon viewing '{' and '}'. It stores all
 * the data found in a queue (and the start_name and type in 2 local variables)
 * until all the data has been read, upon which it is entered into listVertex.
 *
 * Parses fractions if value is a fraction (making code much more complex).
 *
 * Alternative would be to use pointers.
 *
 example file:
 # COMMENT
 unit_name type                      // ! inside
 {                                   // => inside
 end_unit conversion_value           // inside
 end_unit conversion_value           // inside
 end_unit conversion_value           // inside
 }                                   // => ! inside
 unit_name type
 {
 end_unit conversion_value
 end_unit conversion_value
 end_unit conversion_value
 }
 *
 * @param file file from which to read data
 *
 */
void Convert::initVertexs(ifstream & file)
{
    string temp;
    bool inside = false;
    queue<convData> convList;
    string name;

    while (getline(file, temp))
    {
        boost::algorithm::trim(temp);

        if (!temp.empty() && temp[0] != COMMENT)
        {
            if (temp == "{")
                inside = true;
            else if (temp == "}")
            {
                inside = false;
                Vertex vert(name);
                while (!convList.empty())
                {
                    convData temp = convList.front();
                    convList.pop();

                    vert.add(temp.end, temp.value);
                }
                listVertex.insert(vert);
            }
            else if (!inside)
            {
                /*
                 * don't need to use string stream, but it'll remove any
                 * extra parameters (which could be user comments)
                 */
                // unit_name
                stringstream newline(temp);

                newline >> name;

                logger.Log(LOG_LEVELS::init_vertex, "name = " + name);
            }

            else if (inside)
            {
                stringstream newline(temp);
                string end, number;

                newline >> end;
                newline >> number;

                /**
                 * parse fraction if number is one, else just read it strait
                 * in
                 */
                double value = 0;
                unsigned int loc;
                if ((loc = number.find("/")) != string::npos)
                {
                    string strNumer = number.substr(0, loc);
                    string strDenom = number.substr(loc + 1);
                    stringstream ss, ss2;
                    double numer, denom;

                    ss << strNumer;
                    ss >> numer;
                    ss2 << strDenom;
                    ss2 >> denom;

                    value = numer / denom;
                }
                else
                {
                    stringstream ss;
                    ss << number;
                    ss >> value;
                }

                logger.Log(LOG_LEVELS::init_vertex, "end = " + end
                        + "\tvalue = " + boost::lexical_cast<string>(value));

                convList.push(convData(end, value));
            }
        }
    }
}

/**
 * function searches known conversion values for start, using getConvertValue().
 * function then tries converting through all known types, to try to reach end
 * unit (directed graph problem).
 *
 * if start and end are same (converting to self), automatically returns 1.
 *
 *
 * recursive implementation. A queue implementation should be possible.
 * Recursive implementation risks stack overflow if an infinite loop arises,
 * as it did a couple of times during development. (probs where how values
 * initialized and the very inside of the for loop)
 *
 * @param start starting unit/vertex
 * @param end ending unit/vertex
 * @param value value to convert from start to end
 * @return true if conversion successful, false otherwise
 *
 * @author Matthew A. Todd
 * @date August 18, 2009
 */
const bool Convert::convert(const Vertex & start, const string& end,
        double & value) const
{

    //! unit converting to self
    if (start.name == end)
        return 1;

    // find conversion value
    float temp = start.getConvertValue(end);
    if (temp != 0)
    {
        value *= temp;
        return true;
    }

    // for each Vertex w adjacent to v that is not visited and same measurement type,
    //  try route
    map<string, float>::const_iterator iter;
    for (iter = start.convertValue.begin(); iter != start.convertValue.end(); iter++)
    {
        Vertex & w = getVertex(iter->first);
        if (!w.visited)
        {
            logger.Log(LOG_LEVELS::convert, "trying " + start.name + " -> "
                    + w.name);
            w.visited = true;
            if (convert(w, end, value))
            {
                value *= iter->second;
                return true;
            }
        }
    }

    return false;
}
