/*
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
 *
 *
 * convert.h
 *
 *  Created on: Aug 18, 2009
 *      Author: Matthew A. Todd
 */

#ifndef CONVERT_H_
#define CONVERT_H_

#include <string>
#include <map>
#include <queue>
#include <set>

#include "localException.h"

using namespace std;

#define DEFAULT_CONVERT_FILE "convert_values"

#define METER_TO_FOOT 3.2808399

struct Vertex
{
        const string name; //!< name of this vertex
        bool visited; //!< whether this vertex visited in search for end unit (think Dijkstra's Algorithm and directed graphs)
        map<string, float> convertValue; //!< conversion values to adjacent vertexes

        // removed type b/c it wasn't doing anything,
        // mostly just a waste of space
        //        const string type; //!< measurement type

        //! normal ctor
        Vertex(const string & gName) :
            name(gName), visited(false)
        {
        }

        //! copy ctor
        Vertex(const Vertex & b) :
            name(b.name), visited(b.visited), convertValue(b.convertValue)
        {
        }

        /**
         * get conversion value/factor from this to end
         *
         * @param end end conversion unit
         */
        const float getConvertValue(const string & end) const
        {
            map<string, float>::const_iterator iter = convertValue.find(end);
            if (iter != convertValue.end())
                return iter->second;

            return 0;
        }

        /**
         * whether next is adjacent to this vertex
         *
         * @param next vertex to check adjacency on
         */
        inline const bool isAdjacent(const string & next) const
        {
            return getConvertValue(next) != 0;
        }

        /**
         * add a conversion value to a Vertex
         *
         * @param end end conversion unit
         * @param value conversion factor/value
         */
        inline void add(const string & end, const float & value)
        {
            convertValue.insert(pair<string, float> (end, value));
        }
};

/**
 * class to convert units from one to another. Can be done through 3 funcitons,
 * operator (), ctor(start, end, value), or convert(...)
 */
class Convert
{
    private:
        /**
         * comarison functor/functiod for Vertexs. Used in set listVertex.
         */
        struct VertexComp
        {
                bool operator()(const Vertex& lhs, const Vertex& rhs) const
                {
                    return lhs.name < rhs.name;
                }
        };

        void initVertexs(ifstream &);
        void initVertexs(const string &);

        //! list of vertexs/units
        set<Vertex, VertexComp> listVertex;

    public:
        Convert(const string & file = DEFAULT_CONVERT_FILE)
        {
            initVertexs(file);
        }

        /**
         * @see convert()
         */
        Convert(const string & start, const string & end, double & value,
                const string & file = DEFAULT_CONVERT_FILE)
        {
            initVertexs(file);
            convert(start, end, value);
        }

        /**
         *  functoid/functor
         *  @see convert()
         */
        bool operator()(const string & start, const string & end,
                double & value)
        {
            return convert(start, end, value);
        }

        // @see convert()
        const bool convert(const string & start, const string & end,
                double & value) const
        {
            return convert(getVertex(start), end, value);
        }
        const bool convert(const Vertex&, const string&, double &) const;

        /**
         * returns a vertex for a given name
         *
         * @throws ERROR::could_not_find_vertex
         *
         * @param name name of vertex to find/return
         * @return Vertex
         */
        Vertex & getVertex(const string & name) const
        {
            set<Vertex>::const_iterator iter = listVertex.find(Vertex(name));
            if (iter != listVertex.end())
                return (Vertex &) *iter;
            // else
            throw ERROR::COULD_NOT_FIND_VERTEX(name);
        }

        /**
         * print out all the known units
         *
         * @param stream stream to which to print
         * @return stream
         *
         * @author Matthew A. Todd
         * @date August 21, 2009
         */
        ostream & printList(ostream & stream) const
        {
            set<Vertex>::const_iterator iter;
            for (iter = listVertex.begin(); iter != listVertex.end(); iter++)
            {
                stream << iter->name << endl;
            }

            return stream;
        }
};

#endif /* CONVERT_H_ */
