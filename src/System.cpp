/*
 * System.cpp
 *
 *  Created on: Oct 17, 2013
 *      Author: Nikolay Filchenko
 */

#include <System.h>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

namespace System {

namespace {
    bool _term;
}

bool term() {
    return _term;
}

void sigHandler(int signo) {
    if (signo == SIGTERM && !_term) {
        std::cout << "Got SIGTERM, terminating" << std::endl;
        _term = true;
        kill(0, SIGTERM);  // send SIGTERM to the whole process group
    }
}

void logTermination() {
    std::cout << "MonRC stopped" << std::endl;
	syslog(LOG_INFO | LOG_USER, "MonRC stopped");
}

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

    _term = false;

    struct sigaction termAction;
    memset(&termAction, 0, sizeof(termAction));
    termAction.sa_handler = sigHandler;
    sigset_t blockSigSet;
    sigemptyset(&blockSigSet);
    sigaddset(&blockSigSet, SIGTERM);
    sigaddset(&blockSigSet, SIGINT);
    termAction.sa_mask = blockSigSet;
    if (sigaction(SIGTERM, &termAction, 0) == -1) {
        std::cout << "SIGTERM can not be handled" << std::endl;
    }

	syslog(LOG_INFO | LOG_USER, "MonRC started");
    std::cout << "MonRC started" << std::endl;
}

}

