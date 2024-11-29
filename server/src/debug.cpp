#include "debug.hpp"
#include "global_params.hpp"

using namespace std;

void add_debug_message(const string& message) {
    lock_guard<std::mutex> lock(debug_mutex);
    debug_messages.push_back(message);

    // Keep only the last 10 messages
    if (debug_messages.size() > 10) {
        debug_messages.erase(debug_messages.begin());
    }
}