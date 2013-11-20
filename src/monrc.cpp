/*
 * monrc.c
 *
 *  Created on: Oct 8, 2013
 *      Author: Nikolay Filchenko
 */

#include <iostream>
#include <Service.h>
#include <Runlevel.h>
#include <MonitoredServices.h>
#include <thread>
#include <System.h>

int main(int argc, const char * argv[]) {
	System::daemonize("/var/run/monrc.pid", "/var/log/monrc.log");

    MonitoredServices monitoredServices("/usr/local/etc/monrc_config.xml");
    std::cout << "Monitored services:" << std::endl;
	for (auto & s : monitoredServices.get()) {
		std::cout << " * " << s.name() << std::endl;
	}

	std::cout << "MonRC actions:" << std::endl;
	while (!System::term()) {
		for (Service & s : monitoredServices.get()) {
            if (System::term()) {
                break;
            }
			if (s.status() == Service::CRASHED || s.status() == Service::STOPPED) {
                std::cout << " - Restarting " << s.name() << std::endl;
                if (s.start()) {
                    std::cout << " - Done" << std::endl;
                } else {
                    std::cerr << " - Failed!" << std::endl;
                }
			}
		}

        if (!System::term()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
	}

    System::logTermination();
	return 0;
}
