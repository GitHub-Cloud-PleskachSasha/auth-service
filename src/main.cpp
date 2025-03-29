#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <httplib.h>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>

#include "AuthService.hpp"
#include "./repositories/PersonRepository.hpp"

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
            std::string email = req.get_param_value("email");
            std::string password = req.get_param_value("password");

            auto result = authService->registerUser(email, password);

            if (result.success) {
                res.status = 201;
                res.set_content("Registration successful for email: " + email, "text/plain");
            } else {
                res.status = result.errorCode;
                res.set_content(result.errorMessage, "text/plain");
            }
        } catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content("Unexpected error: " + std::string(e.what()), "text/plain");
        }
    });

	svr.Get("/login", [&db, authService](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string email = req.get_param_value("email");
            std::string password = req.get_param_value("password");
        
            auto result = authService->loginUser(email, password);
            
            if (result.success) {
                nlohmann::json responseJson;
                responseJson["token"] = result.token;
                
                res.status = 200;
                res.set_content(responseJson.dump(), "application/json");
            } else {
                res.status = result.errorCode;
                res.set_content(result.errorMessage, "text/plain");
            }
        } catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content("Unexpected error: " + std::string(e.what()), "text/plain");
        }
    });

    svr.Get("/validate", [&db, authService](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string token;
        
            if (req.has_param("token")) {
                token = req.get_param_value("token");
            } else {
                res.status = 400;
                res.set_content("Token is missing", "text/plain");
                return;
            }

            auto result = authService->validateToken(token);

            if (result.success) {
                nlohmann::json responseJson;
                responseJson["valid"] = result.valid;
                responseJson["user_id"] = result.userId;
                responseJson["email"] = result.email;
                
                res.status = 200;
                res.set_content(responseJson.dump(), "application/json");
            } else {
                res.status = result.errorCode;
                res.set_content(result.errorMessage, "text/plain");
            }
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Validation failed: " + std::string(e.what()), "text/plain");
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content("Unexpected error: " + std::string(e.what()), "text/plain");
        }
    });

    svr.Post("/change-password", [&db, authService](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string email = req.get_param_value("email");
            std::string oldPassword = req.get_param_value("old_password");
            std::string newPassword = req.get_param_value("new_password");
            
            auto result = authService->changePassword(email, oldPassword, newPassword);
            
            if (result.success) {
                nlohmann::json responseJson;
                responseJson["message"] = result.errorMessage;
                
                res.status = 200;
                res.set_content(responseJson.dump(), "application/json");
            } else {
                res.status = result.errorCode;
                res.set_content(result.errorMessage, "text/plain");
            }
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Password change failed: " + std::string(e.what()), "text/plain");
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content("Unexpected error: " + std::string(e.what()), "text/plain");
        }
    });
    
	!svr.listen("0.0.0.0", 8080) ? (std::cerr << "Server failed to start!" << std::endl) : (std::cout << "Server is running!" << std::endl);
	
	return 0;
}
