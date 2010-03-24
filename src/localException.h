/*
 * localException.h
 *
 *  Created on: Aug 19, 2009
 *      Author: matcat
 */
#ifndef LOCALEXCEPTION_H_
#define LOCALEXCEPTION_H_

#include "LogFile.h"  // logging func's
#include <exception>
#include <cstring>

using namespace std;

extern LogFile logger;

namespace LOG_LEVELS
{
//! log levels for our logger
/**
 *
 * @author Matthew A. Todd
 *
 * 0: null, n/a, don't know, temp
 *
 * 1-9: user input
 *
 * 10-19:
 *
 * 90-99: testing
 */
enum log_levels
{
    null = 0, //!< null, n/a, don't know, temp
    general_user_input = 1, //!< user input that can come from multiple sources
    command_line_input, //!< command line options
    init_vertex = 10,//!< for initializing vertexs (including file)
    convert = 20,
//!< conversion related functions
};

}

namespace ERROR
{

/**
 * couldn't find Vertex
 *
 * @author Matthew A. Todd
 * @date August 19, 2009
 */
class could_not_find_vertex: public exception
{
        string ret;
    public:
        could_not_find_vertex(string name)
        {
            ret = "could not find unit type (vertex) " + name;
        }
        virtual const char* what() const throw ()
        {
            return ret.c_str();
        }
        virtual ~could_not_find_vertex() throw ()
        {
        }
};
#define COULD_NOT_FIND_VERTEX(p) could_not_find_vertex(p)

/**
 * Problem with opening output file.
 *
 * @author Matthew A. Todd
 * @date March 6, 2009
 */
class bad_output_file: public exception
{
        virtual const char* what() const throw ()
        {
            return "bad output file";
        }
};
#define BAD_OUTPUT_FILE bad_output_file()

/**
 * Problem with opening input file
 *
 * @author Matthew A. Todd
 * @date March 6, 2009
 */
class bad_input_file: public exception
{
        virtual const char* what() const throw ()
        {
            return "bad input file";
        }
};
#define BAD_INPUT_FILE bad_input_file()

} /* end of ERROR namespace*/
#endif /* LOCALEXCEPTION_H_ */

