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
                res.status = 200;
                res.set_content("Login is sucsses" "text/plain");
            }
        
            t.commit();
        }
        catch (const odb::exception& e) {
            res.status = 500;
            res.set_content("Login failed: " + std::string(e.what()), "text/plain");
        }
    });

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
                res.set_content("Email вже використовується!", "text/plain");
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

	!svr.listen("0.0.0.0", 8080) ? (std::cerr << "Server failed to start!" << std::endl) : (std::cout << "Server is running!" << std::endl);
	
	return 0;
}
