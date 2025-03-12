#include <iostream>
#include <string>
#include <memory>
#include <httplib.h>
#include <jwt-cpp/jwt.h>
#include "../repositories/PersonRepository.hpp"

class AuthService {
public:
    AuthService(std::shared_ptr<PersonRepository> repo)
        : repo(repo){

    }

    httplib::Response registerUser(const httplib::Request& req) {
        httplib::Response res;

        std::string email = req.get_param_value("email");
        std::string password = req.get_param_value("password");

        if (email.empty() || password.empty()) {
            res.status = 400;
            res.set_content("Email or password is missing!", "text/plain");
            return res;
        }

        if (repo->emailExists(email)) {
            res.status = 409;
            res.set_content("Email is already in use!", "text/plain");
            return res;
        }
        
        try {
            repo->createPerson(email, password);
            res.status = 201;
            res.set_content("Registration successful for email: " + email, "text/plain");
        }
        catch (const std::exception& e) {
            res.status = 500;
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        }

        return res;
    }

    httplib::Response loginUser(const httplib::Request& req) {
        httplib::Response res;

        std::string email = req.get_param_value("email");
        std::string password = req.get_param_value("password");

        if (email.empty() || password.empty()) {
            res.status = 400;
            res.set_content("Email or password is missing!", "text/plain");
            return res; 
        }

        try {
            bool isCreated = repo->personExist(email, password);
           
            if (isCreated) {
                res.status = 401;
                res.set_content("Login failed: no matching user", "text/plain");
            } else {
                auto token = repo->loginPerson(email, password);
            
                nlohmann::json responseJson;
                responseJson["token"] = token;

                res.status = 200;
                res.set_content(responseJson.dump(), "application/json");
            }
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Login failed: " + std::string(e.what()), "text/plain");
        }

        return res;
    }

    httplib::Response validateToken(const httplib::Request& req) {
        httplib::Response res;

        std::string token;
        
        if (req.has_param("token")) {
            token = req.get_param_value("token");
        }
        else {
            res.status = 400;
            res.set_content("Token is missing", "text/plain");
            return res;
        }
    
        try {
            auto decoded_token = jwt::decode(token);
    
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{"secret_key"})
                .with_issuer("authservice");
    
            verifier.verify(decoded_token);
    
            auto user_id_claim = decoded_token.get_payload_claim("user_id").as_string();
            auto email_claim   = decoded_token.get_payload_claim("email").as_string();
    
            std::string response_body = "{\"valid\": true, \"user_id\": \"" + user_id_claim +
                                        "\", \"email\": \"" + email_claim + "\"}";
            res.status = 200;
            res.set_content(response_body, "application/json");
        }
        catch (const std::exception& e) {

            res.status = 401;
            res.set_content(std::string("Invalid token: ") + e.what(), "text/plain");
        }

        return res;
    }

private:
    std::shared_ptr<PersonRepository> repo;
};
