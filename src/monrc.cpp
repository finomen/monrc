/*
 * monrc.c
 *
 *  Created on: Oct 8, 2013
 *      Author: Nikolay Filchenko
 */

#include <iostream>
#include <Service.h>
#include <Runlevel.h>
#include <thread>
#include <System.h>

int main(int argc, const char * argv[]) {
	System::daemonize("/var/run/monrc.pid", "/var/log/monrc.log");

	Runlevel & r = runlevel("default");
	for (auto & s : r.services()) {
		std::cout << " * " << s.name() << std::endl;
	}

	for (;;) {
		for (Service & s : r.services()) {
			if (s.status() == Service::CRASHED || s.status() == Service::STOPPED) {
				std::cout << " - Restarting " << s.name() << std::endl;
				if (!s.start())	{
					std::cerr << "Failed!" << std::endl;
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}

