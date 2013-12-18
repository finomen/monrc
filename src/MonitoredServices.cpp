#include <MonitoredServices.h>

#include <set>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <Runlevel.h>

MonitoredServices::MonitoredServices(std::string const& configFileName)
    : configFileName_(configFileName) {
    reload();
}

void MonitoredServices::reload() {
    using boost::property_tree::ptree;
    
    ptree pt;
    read_xml(configFileName_, pt);

    std::set<std::string> monitoredServices;
    std::set<std::string> disableLoggingFor;

    BOOST_FOREACH(ptree::value_type& v, pt.get_child("monrc.monitored.services")) {
        monitoredServices.insert(v.second.data());
    }
    BOOST_FOREACH(ptree::value_type& v, pt.get_child("monrc.monitored.runlevels")) {
        Runlevel & r = runlevel(v.second.data());
        for (auto & s : r.services()) {
            monitoredServices.insert(s.name());
        }
    }
    BOOST_FOREACH(ptree::value_type& v, pt.get_child("monrc.excluded.services")) {
        monitoredServices.erase(v.second.data());
    }

    BOOST_FOREACH(ptree::value_type& v, pt.get_child("monrc.disableLoggingFor.services")) {
        disableLoggingFor.insert(v.second.data());
    }

    mServices_.clear();
    for (std::set<std::string>::const_iterator ms = monitoredServices.begin(); ms != monitoredServices.end(); ++ms) {
        mServices_.emplace_back(std::move(Service(*ms, disableLoggingFor.find(*ms) == disableLoggingFor.end())));
    }
}

std::vector<Service>& MonitoredServices::get() {
    return mServices_;
}
