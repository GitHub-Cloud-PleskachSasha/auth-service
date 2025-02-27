#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <httplib.h>
#include <string>
#include <memory>

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
        new odb::mysql::database("root", "root", "stockexchange", "db", 3306)
    );
    
    {
        odb::transaction t (db->begin ());    
        db->execute("CREATE TABLE IF NOT EXISTS person (id INT AUTO_INCREMENT PRIMARY KEY, email VARCHAR(255) UNIQUE, password VARCHAR(255))");
        t.commit();
    }

	svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
		res.set_content("Hello World!!!", "text/plain");
	});

	svr.Post("/login", [&db](const httplib::Request& req, httplib::Response& res) {
        std::string email = req.get_param_value("email");
        std::string password = req.get_param_value("password");


        if (email.empty() || password.empty()) {
            res.set_content("Email or password is missing!", "text/plain");
            return;
        }

        res.set_content("Login email: " + email + "====pass: " + password, "text/plain");
    });

	svr.Post("/register", [&db](const httplib::Request& req, httplib::Response& res) {
        std::string email = req.get_param_value("email");
        std::string password = req.get_param_value("password");

        if (email.empty() || password.empty()) {
            res.set_content("Email or password is missing!", "text/plain");
            return;
        }
        try {
            person newPerson(email, password);
            
            odb::transaction t(db->begin());
            
            db->persist(newPerson);
            
            t.commit();
            
            res.set_content("Registration successful for email: " + email, "text/plain");
        }
        catch (const odb::exception& e) {
            res.set_content("Registration failed: " + std::string(e.what()), "text/plain");
        }

        res.set_content("Registration email1: " + email + "====pass1: " + password, "text/plain");
    });

	!svr.listen("0.0.0.0", 8080) ? (std::cerr << "Server failed to start!" << std::endl) : (std::cout << "Server is running!" << std::endl);
	
	return 0;
}
