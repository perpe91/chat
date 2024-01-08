#include "json.hpp"
#include "message.hpp"
#include "time_utils.hpp"

#include <algorithm>
#include <iostream>

#include <pistache/net.h>
#include <pistache/endpoint.h>
#include <pistache/router.h>

using namespace Pistache;
using namespace Pistache::Http;

std::mutex message_queue_lock;

struct MessageStore {
    void getMessage(const Rest::Request& request, Http::ResponseWriter response) {
        try {
            nlohmann::json new_messages;
            auto last_time_str = request.query().get("last_update_time");
            if (last_time_str) {
                const auto last_time = time_from_string(*last_time_str);

                std::scoped_lock<std::mutex> lock{message_queue_lock};
                for (const auto& m : message_queue) {
                    if (m.timestamp > last_time) {
                        new_messages.push_back(m.to_json());
                    }
                }
            } else {
                std::scoped_lock<std::mutex> lock{message_queue_lock};
                for (const auto& m : message_queue) {
                    new_messages.push_back(m.to_json());
                }
            }
            response.send(Code::Ok, to_string(new_messages), 
                Mime::MediaType(Mime::Type::Application, Mime::Subtype::Json));

        } catch (std::exception& e) {
            std::cout << "Error receiving new message: " << e.what() << std::endl;
            response.send(Code::Internal_Server_Error);
        } catch (...) {
            std::cout << "Error receiving new message" << std::endl;
            response.send(Code::Internal_Server_Error);
        }
    }

    void postMessage(const Rest::Request& request, Http::ResponseWriter response) {
        using namespace Pistache::Http;
        try {
            chat::Message m = chat::Message::from_json_string(request.body());
            std::cout << "New message from " << m.username << ": " << request.body() << std::endl;
            {
                std::scoped_lock<std::mutex> lock{message_queue_lock};
                message_queue.push_back(m);
                if (message_queue.size() > 1000) {
                    std::sort(message_queue.begin(), message_queue.end());
                    message_queue.erase(message_queue.begin(), message_queue.begin() + 200);
                }
            }
            response.send(Code::Ok);
        } catch (std::exception& e) {
            std::cout << "Error receiving new message: " << e.what() << std::endl;
            response.send(Code::Internal_Server_Error);
        } catch (...) {
            std::cout << "Error receiving new message" << std::endl;
            response.send(Code::Internal_Server_Error);
        }
    }

private:
    std::vector<chat::Message> message_queue;
};

int main(int argc, const char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <port>\n\n";
        return 1;
    }
    int port = std::stoi(argv[1]);

    using namespace Pistache::Rest;
    Router router;

    MessageStore messageStore;
    Routes::Get(router, "/chat", Routes::bind(&MessageStore::getMessage, &messageStore));
    Routes::Post(router, "/chat", Routes::bind(&MessageStore::postMessage, &messageStore));

    std::cout << "Server listening on port " << port << "\n";
    Address addr("0.0.0.0", Port(port));

    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    server.serve();
}
