#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <httplib.h>
#include <string>
#include <memory>

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include <odb/mysql/database.hxx>

#include "person-odb.hxx"


int main(int argc, char* argv[])
{
    try
    {
        // std::cout << "temp message";
        // std::unique_ptr<odb::database> db(
        //     new odb::mysql::database("root", "root", "stockexchange", "db", 3306)
        // );

        // person p("test@example.com", "password");

        // {
        //     odb::transaction t(db->begin());
        //     db->persist(p);
        //     t.commit();
        // }

        // {
        //     odb::transaction t(db->begin());
        //     std::shared_ptr<person> p2(db->load<person>(p.id()));
        //     std::cout << "User email: " << p2->email() << std::endl;
        //     t.commit();
        // }
    }
    catch(const std::exception& e)
    {
        // std::cerr << "ODB Exception: " << e.what() << std::endl;
        // return 1;
    }
    
    return 0;

	httplib::SSLServer svr("./cert.pem", "./key.pem");
	// httplib::Server svr;

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
