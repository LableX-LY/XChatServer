#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <vector>

using boost::asio::ip::tcp;
using namespace std;

class User {
public:
    User(const std::string& name, const std::string& password)
            : userName(name), userPassword(password) {}

    std::string getUserName() const {
        return userName;
    }

    std::string getUserPassword() const {
        return userPassword;
    }

private:
    std::string userName;
    std::string userPassword;
};

// Sample users array
std::vector<User> users = {
        User("邢利宇", "202145225129"),
        User("解来文", "202145225124"),
        User("赵漪浪", "202145225134")
};

class ChatServer {
public:
    ChatServer(boost::asio::io_context& io_context, int port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
        cout << "服务器在9999端口启动成功" << endl;
    }

private:
    void start_accept() {
        std::shared_ptr<tcp::socket> socket(new tcp::socket(acceptor_.get_executor()));
        acceptor_.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
            if (!error) {
                std::thread(&ChatServer::handle_client, this, socket).detach();
            }
            start_accept();
            cout << "服务器持续监听9999端口" << endl;
        });
    }

    void handle_client(std::shared_ptr<tcp::socket> socket) {
        string userName;
        try {
            while (true) {
                boost::asio::streambuf buffer;
                boost::asio::read_until(*socket, buffer, "\n");
                std::istream is(&buffer);
                std::string request_type;
                is >> request_type;

                if (request_type == "LOGIN") {
                    std::string userPassword;
                    is >> userName >> userPassword;

                    User user(userName, userPassword);
                    if (authenticate_user(user)) {
                        std::lock_guard<std::mutex> lock(mutex_);
                        sessions_[userName] = socket;

                        // Send login success message
                        std::string response = "LOGIN_SUCCESS\n";
                        boost::asio::write(*socket, boost::asio::buffer(response));

                        cout << userName + "登录成功" << endl;

                        // Send the current online user list
                        //send_online_users(socket);
                        //notify_all_users();

                    } else {
                        std::string response = "LOGIN_FAILED\n";
                        boost::asio::write(*socket, boost::asio::buffer(response));
                        cout << userName + "登录失败" << endl;
                    }
                } else if (request_type == "CHAT") {
                    std::string from_user, to_user, message;
                    is >> from_user >> to_user;
                    std::getline(is, message);

                    std::lock_guard<std::mutex> lock(mutex_);
                    if (sessions_.find(to_user) != sessions_.end()) {
                        std::string full_message = "CHAT " + from_user + ": " + message + "\n";
                        boost::asio::write(*sessions_[to_user], boost::asio::buffer(full_message));
                        cout << from_user + "向" + to_user + "发送消息" + message << endl;
                    } else {
                        std::string response = "USER_NOT_FOUND\n";
                        boost::asio::write(*socket, boost::asio::buffer(response));
                    }
                } else if (request_type == "GET_ONLINE_USERS") {
                    send_online_users(socket);
                } else if (request_type == "QUIT") {
                    std::lock_guard<std::mutex> lock(mutex_);
                    sessions_.erase(userName);
                    socket->close();
                    notify_all_users();
                    break;
                }
            }
        } catch (std::exception& e) {
            std::cerr << "Exception in handle_client: " << e.what() << "\n";
            std::lock_guard<std::mutex> lock(mutex_);
            sessions_.erase(userName);
            notify_all_users();
        }
    }

    void send_online_users(std::shared_ptr<tcp::socket> socket) {
        std::string user_list = "ONLINE_USERS";
        for (const auto& session : sessions_) {
            user_list += " " + session.first;
        }
        user_list += "\n";
        boost::asio::write(*socket, boost::asio::buffer(user_list));
    }

    void notify_all_users() {
        std::string user_list = "ONLINE_USERS";
        for (const auto& session : sessions_) {
            user_list += " " + session.first;
        }
        user_list += "\n";
        for (const auto& session : sessions_) {
            boost::asio::write(*session.second, boost::asio::buffer(user_list));
        }
    }

    static bool authenticate_user(const User& user) {
        for (const auto& u : users) {
            if (u.getUserName() == user.getUserName() && u.getUserPassword() == user.getUserPassword()) {
                return true;
            }
        }
        return false;
    }

    tcp::acceptor acceptor_;
    std::unordered_map<std::string, std::shared_ptr<tcp::socket>> sessions_;
    std::mutex mutex_;
};

int main() {
    try {
        boost::asio::io_context io_context;
        ChatServer server(io_context, 9999);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    //hahahaha
    return 0;
}