/*
 * System.h
 *
 *  Created on: Oct 17, 2013
 *      Author: Nikolay Filchenko
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>

namespace System {

bool term();
void logTermination();
void daemonize(std::string const & pidFile, std::string const & logFile = "/dev/null");

}

#endif /* SYSTEM_H_ */
