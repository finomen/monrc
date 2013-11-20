/*
 * Runlevel.cpp
 *
 *  Created on: Oct 8, 2013
 *      Author: Nikolay Filchenko
 */

#include <exception>
#include <rc.h>
#include <utility>

#include <Runlevel.h>

std::string const & Runlevel::name() const {
	return name_;
}

std::vector<Service> & Runlevel::services() {
	return services_;
}

void Runlevel::reload() {
	RC_STRINGLIST * services = rc_services_in_runlevel(name_.c_str());
	RC_STRING * service = services->tqh_first;

    services_.clear();
	while (service) {
		services_.emplace_back(std::move(Service(service->value)));
		service = service->entries.tqe_next;
	}
}

Runlevel::Runlevel(std::string const & name) : name_(name){
	reload();
}

std::vector<Runlevel> & runlevels() {
	static std::vector<Runlevel> list;
	if (list.empty()) {
		RC_STRINGLIST * runlevels = rc_runlevel_list();
		RC_STRING * runlevel = runlevels->tqh_first;

		while (runlevel) {
			list.emplace_back(std::move(Runlevel(runlevel->value)));
			runlevel = runlevel->entries.tqe_next;
		}
	}

	return list;
}

Runlevel & runlevel(std::string const & name) {
	std::vector<Runlevel> & levels = runlevels();
	for (int i = 0; i < levels.size(); ++i) {
		if (levels[i].name() == name) {
			return levels[i];
		}
	}

	throw RunlevelNotFoundException(name);
}

