#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <httplib.h>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>

#include "AuthService.hpp"

#include <odb/mysql/database.hxx>


int main(int argc, char* argv[])
{
	httplib::SSLServer svr("./cert.pem", "./key.pem");

	if (!svr.is_valid()) {
		std::cerr << "Invalid SSL server configuration!1" << std::endl;
		return 1;
	}

    std::shared_ptr<odb::database> db(
        new odb::mysql::database("root", "root", "authservice", "db", 3306)
    );
    
    auto repo = std::make_shared<PersonRepository>(db);


    auto authService = std::make_shared<AuthService>(repo);

	svr.Post("/register", [&db, authService](const httplib::Request& req, httplib::Response& res) {
        try {
            res = authService->registerUser(req);
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        }
    });

	svr.Get("/login", [&db, authService](const httplib::Request& req, httplib::Response& res) {
        try {
            res = authService->loginUser(req);
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        }
    });

    svr.Get("/validate", [&db, authService](const httplib::Request& req, httplib::Response& res) {
        try {
            res = authService->validateToken(req);
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        }
    });

    svr.Post("/change-password", [&db, authService](const httplib::Request& req, httplib::Response& res) {
        try {
            res = authService->changePassword(req);
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        }
    });
    
	!svr.listen("0.0.0.0", 8080) ? (std::cerr << "Server failed to start!" << std::endl) : (std::cout << "Server is running!" << std::endl);
	
	return 0;
}
