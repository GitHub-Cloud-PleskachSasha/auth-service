#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <httplib.h>
#include <string>
#include <mysql_orm/Connection.hpp>


int main(int argc, char* argv[])
{
	httplib::SSLServer svr("./cert.pem", "./key.pem");
	// httplib::Server svr;

    try {
        auto connection = mysql_orm::Connection("localhost", 3306, "username", "password", "database");
        std::cerr << "Goooooooooood" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

	if (!svr.is_valid()) {
		std::cerr << "Invalid SSL server configuration!1" << std::endl;
		return 1;
	}

	svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
		res.set_content("Hello World!!!1", "text/plain");
	});

	svr.Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        std::string email = req.get_param_value("email");
        std::string password = req.get_param_value("password");

        if (email.empty() || password.empty()) {
            res.set_content("Email or password is missing!", "text/plain");
            return;
        }

        res.set_content("Login" , "text/plain");
    });

	svr.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        std::string email = req.get_param_value("email");
        std::string password = req.get_param_value("password");

        if (email.empty() || password.empty()) {
            res.set_content("Email or password is missing!", "text/plain");
            return;
        }

        res.set_content("Registration email: " + email + "====pass: " + password, "text/plain");
    });

	!svr.listen("0.0.0.0", 8080) ? (std::cerr << "Server failed to start!" << std::endl) : (std::cout << "Server is running!" << std::endl);
	
	return 0;
}
