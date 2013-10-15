/*
 * Service.h
 *
 *  Created on: Oct 8, 2013
 *      Author: Nikolay Filchenko
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include <string>

class Runlevel;

class Service {
public:
	enum ServiceStatus {
		STOPPED     = 0x0001,
		STARTED     = 0x0002,
		STOPPING    = 0x0004,
		STARTING    = 0x0008,
		INACTIVE    = 0x0010,
		HOTPLUGGED  = 0x0100,
		FAILED      = 0x0200,
		SCHEDULED   = 0x0400,
		WASINACTIVE = 0x0800,
		CRASHED     = 0xFFFF
	};
public:
	std::string const & name() const;
	ServiceStatus status() const;
	bool start();
	bool stop();
	bool restart();
	bool reload();
	bool zap();
	Service(Service &&) = default;
	std::string const & getLastStdout() const;
	std::string const & getLastStderr() const;
private:
	bool exec(std::string const & command);
	Service(std::string const & name);
	Service(const Service &) = delete;
	friend class Runlevel;
private:
	std::string name_;
	std::string lastOut;
	std::string lastErr;
};

#endif // SERVICE_H_
