#include "handle_uuid.hpp"


std::string generate_uuid_to_string(){
    uuid id = random_generator()();

    // Konvertera UUID till en sträng
    std::stringstream ss;
    ss << id;
    std::string uuid_str = ss.str();
    return uuid_str;
    }
