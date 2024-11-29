#include <asio.hpp>

using asio::ip::tcp;

void asio_task();
void handle_client(tcp::socket& socket);
