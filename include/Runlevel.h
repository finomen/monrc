/*
 * Runlevel.h
 *
 *  Created on: Oct 8, 2013
 *      Author: Nikolay Filchenko
 */

#ifndef RUNLEVEL_H_
#define RUNLEVEL_H_

#include <exception>
#include <vector>

#include <Service.h>

class Runlevel {
public:
	std::string const & name() const;
	std::vector<Service> & services();
	Runlevel(Runlevel &&) = default;
	void reload();
private:
	Runlevel(std::string const & name);
	Runlevel(Runlevel const &) = delete;
	friend std::vector<Runlevel> & runlevels();
private:
	std::string name_;
	std::vector<Service> services_;
};

std::vector<Runlevel> & runlevels();

class RunlevelNotFoundException : public std::exception {
public:
	RunlevelNotFoundException(std::string const & name) throw() {
		message = "Runlevel `" + name + "` not found";
	}

	const char * what() const throw() {
		return message.c_str();
	}

private:
	std::string message;
};


Runlevel & runlevel(std::string const & name);

#endif /* RUNLEVEL_H_ */
