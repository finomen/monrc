/*
 * System.cpp
 *
 *  Created on: Oct 17, 2013
 *      Author: Nikolay Filchenko
 */

#include <System.h>
#include <iostream>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

namespace System {

void daemonize(std::string const & pidFile, std::string const & logFile) {
	if (pid_t pid = fork()) {
		if (pid > 0) {
			exit(0);
		} else {
			syslog(LOG_ERR | LOG_USER,
					"Unable to start daemon (first fork wailed: %m)");
			exit(1);
		}
	}

	setsid();
	chdir("/");
	umask(0);

	if (pid_t pid = fork()) {
		if (pid > 0) {
			exit(0);
		} else {
			syslog(LOG_ERR | LOG_USER,
					"Unable to start daemon (second fork wailed: %m)");
			exit(1);
		}
	}

	std::ofstream pidf(pidFile.c_str());
	pidf << getpid();
	pidf << std::flush;
	pidf.close();

	close(0);
	close(1);
	close(2);

	if (open("/dev/null", O_RDONLY) < 0) {
		syslog(LOG_ERR | LOG_USER, "Unable to open /dev/null: %m");
		exit(1);
	}

	const char* output = logFile.c_str();
	const int flags = O_WRONLY | O_CREAT | O_APPEND;
	const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if (open(output, flags, mode) < 0) {
		syslog(LOG_ERR | LOG_USER, "Unable to open output file %s: %m", output);
		exit(1);
	}

	if (dup(1) < 0) {
		syslog(LOG_ERR | LOG_USER, "Unable to dup output descriptor: %m");
		exit(1);
	}

	syslog(LOG_INFO | LOG_USER, "MonRC started");
}

}

