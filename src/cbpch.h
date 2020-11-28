#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <asio.hpp>
#include <aegis.hpp>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <chrono>
#include <fstream>

#include <cstdlib>
#include <ctime>

#include <execinfo.h>
#include <signal.h>

#include <restclient-cpp/restclient.h>
#include <restclient-cpp/connection.h>

#include <bot/utils/utils.h>

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>