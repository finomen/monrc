/*
 * Service.cpp
 *
 *  Created on: Oct 8, 2013
 *      Author: Nikolay Filchenko
 */

#include <cstdlib>
#include <rc.h>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <Service.h>

namespace {
int popenRWE(int *rwepipe, const char *exe, const char * const argv[]) {
	int in[2];
	int out[2];
	int err[2];
	int pid;
	int rc;

	rc = pipe(in);
	if (rc < 0)
		goto error_in;

	rc = pipe(out);
	if (rc < 0)
		goto error_out;

	rc = pipe(err);
	if (rc < 0)
		goto error_err;

	pid = fork();
	if (pid > 0) { // parent
		close(in[0]);
		close(out[1]);
		close(err[1]);
		rwepipe[0] = in[1];
		rwepipe[1] = out[0];
		rwepipe[2] = err[0];
		return pid;
	} else if (pid == 0) { // child
		close(in[1]);
		close(out[0]);
		close(err[0]);
		close(0);
		dup(in[0]);
		close(1);
		dup(out[1]);
		close(2);
		dup(err[1]);

		execvp(exe, (char**) argv);
		exit(1);
	} else
		goto error_fork;

	return pid;

	error_fork: close(err[0]);
	close(err[1]);
	error_err: close(out[0]);
	close(out[1]);
	error_out: close(in[0]);
	close(in[1]);
	error_in: return -1;
}

int pcloseRWE(int pid, int *rwepipe) {
	int rc, status;
	close(rwepipe[0]);
	close(rwepipe[1]);
	close(rwepipe[2]);
	rc = waitpid(pid, &status, 0);
	return status;
}
}

std::string const & Service::name() const {
	return name_;
}

Service::ServiceStatus Service::status() const {
	if (rc_service_daemons_crashed(name_.c_str()))
		return Service::CRASHED;
	return static_cast<Service::ServiceStatus>(rc_service_state(name_.c_str()));
}

bool Service::start() {
	if (status() == CRASHED) {
		zap();
	}

	if (status() == STOPPED) {
		return exec("start");
	}

	return false;
}

bool Service::stop() {
	if (status() != STOPPED) {
		return exec("stop");
	}
	return false;
}

bool Service::restart() {
	if (status() != STOPPED) {
		return exec("restart");
	}
	return false;
}

bool Service::reload() {
	if (status() == STARTED) {
		return exec("reload");
	}

	return false;
}

bool Service::zap() {
	if (status() == CRASHED) {
		return exec("zap");
	}

	return false;
}

bool Service::exec(std::string const & cmd) {
	std::string command = "/etc/init.d/" + name();
	int pipe[3];
	int pid;
	const char * const args[] = { command.c_str(), cmd.c_str(), NULL };
	pid = popenRWE(pipe, args[0], args);
	int status = 0;
	waitpid(pid,&status,0);

	std::ostringstream out;
	char buf[1024];
	ssize_t s = 0;
	while ((s = read(pipe[1], buf, 1024)) > 0) {
		out << std::string(buf, s);
	}

	lastOut = out.str();

	std::ostringstream err;
	while ((s = read(pipe[2], buf, 1024)) > 0) {
		err << std::string(buf, s);
	}

	lastErr = err.str();


	pcloseRWE(pid, pipe);
	return !status;
}

std::string const & Service::getLastStdout() const {
	return lastOut;
}

std::string const & Service::getLastStderr() const {
	return lastErr;
}

Service::Service(std::string const & name, bool loggingEnabled_) :
		name_(name), loggingEnabled_(loggingEnabled_) {
}

bool Service::loggingEnabled() const {
    return loggingEnabled_;
}
