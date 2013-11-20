#ifndef MONITORED_SERVICES_H_
#define MONITORED_SERVICES_H_

#include <vector>

#include <Service.h>

class MonitoredServices {
public:
    MonitoredServices(std::string const& configFileName);
    MonitoredServices(MonitoredServices &&) = default;
    void reload();
    std::vector<Service>& get();
private:
    MonitoredServices(MonitoredServices const &) = delete;
private:
    std::string configFileName_;
    std::vector<Service> mServices_;
};

#endif  // MONITORED_SERVICES_H_
