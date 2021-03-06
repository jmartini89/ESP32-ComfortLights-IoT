#ifndef WMPARAMETERS_H
#define WMPARAMETERS_H

#include <WiFiManager.h>

struct wmParameters {
  WiFiManagerParameter address;
  WiFiManagerParameter id;
  WiFiManagerParameter user;
  WiFiManagerParameter pass;
  WiFiManagerParameter topic;
  wmParameters()
    :address("server", "mqtt server", "", 40)
    ,id("id", "mqtt id", "", 40)
    ,user("user", "mqtt user", "", 40)
    ,pass("pass", "mqtt pass", "", 40)
    ,topic("topic", "mqtt topic", "", 40)
  {};
};

#endif
