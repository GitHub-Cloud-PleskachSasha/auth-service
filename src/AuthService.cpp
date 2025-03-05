#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <httplib.h>
#include <string>
#include <memory>

#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/query.hxx>

#include <odb/mysql/database.hxx>

#include "person-odb.hxx"

int main(int argc, char* argv[])
{
	httplib::SSLServer svr("./cert.pem", "./key.pem");

	if (!svr.is_valid()) {
		std::cerr << "Invalid SSL server configuration!1" << std::endl;
		return 1;
	}

    std::unique_ptr<odb::database> db(
        new odb::mysql::database("root", "root", "authservice", "db", 3306)
    );
    
    {
        odb::transaction t (db->begin ());    
        db->execute("CREATE TABLE IF NOT EXISTS person (id INT AUTO_INCREMENT PRIMARY KEY, email VARCHAR(255) UNIQUE, password VARCHAR(255))");
        t.commit();
    }

	svr.Post("/register", [&db](const httplib::Request& req, httplib::Response& res) {
        std::string email = req.get_param_value("email");
        std::string password = req.get_param_value("password");

        if (email.empty() || password.empty()) {
            res.status = 400;
            res.set_content("Email or password is missing!", "text/plain");
            return;
        }
        try {
            odb::transaction t(db->begin());

            odb::result<person> r(db->query<person>(odb::query<person>::email == email));
            if (!r.empty()) {
                res.status = 409;
                res.set_content("Email is already in use!", "text/plain");
                return;
            }

            person newPerson(email, password);
            db->persist(newPerson);
            t.commit();

            res.status = 201;
            res.set_content("Registration successful for email: " + email, "text/plain");
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        }
    });

	svr.Post("/login", [&db](const httplib::Request& req, httplib::Response& res) {
        std::string email = req.get_param_value("email");
        std::string password = req.get_param_value("password");

        if (email.empty() || password.empty()) {
            res.status = 400;
            res.set_content("Email or password is missing!", "text/plain");
            return;
        }

        try {
            odb::transaction t(db->begin());
        
            odb::result<person> r(db->query<person>(
                odb::query<person>::email == email &&
                odb::query<person>::password == password
            ));
        
            if (r.empty()) {
                res.status = 401;
                res.set_content("Login failed: no matching user", "text/plain");
            } else {
                person user = *r.begin();

                auto token = jwt::create()
                    .set_issuer("authservice")        
                    .set_type("JWS")                      
                    .set_issued_at(std::chrono::system_clock::now()) 
                    .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24)) 
                    .set_payload_claim("user_id", jwt::claim(std::to_string(user.getId())))   
                    .set_payload_claim("email", jwt::claim(user.email()))                      
                    .sign(jwt::algorithm::hs256{"secret_key"}); 

                res.status = 200;
                nlohmann::json responseJson;
                responseJson["token"] = token;
                res.set_content(responseJson.dump(), "application/json");
            }
        
            t.commit();
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Login failed: " + std::string(e.what()), "text/plain");
        }
    });

    svr.Post("/validate", [](const httplib::Request& req, httplib::Response& res) {
        std::string token;
        
        if (req.has_param("token")) {
            token = req.get_param_value("token");
        }
        else {
            res.status = 400;
            res.set_content("Token is missing", "text/plain");
            return;
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
    });
    

	!svr.listen("0.0.0.0", 8080) ? (std::cerr << "Server failed to start!" << std::endl) : (std::cout << "Server is running!" << std::endl);
	
	return 0;
}
