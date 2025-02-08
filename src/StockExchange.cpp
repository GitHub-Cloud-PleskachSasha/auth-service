#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <httplib.h>
// #include <openssl/opensslv.h>

int main(int argc, char* argv[])
{
	httplib::SSLServer svr("./cert.pem", "./key.pem");
	// httplib::Server svr;

	if (!svr.is_valid()) {
		std::cerr << "Invalid SSL server configuration!1" << std::endl;
		return 1;
	}

	svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
		res.set_content("1H44ello World!!!1", "text/plain");
	});

	!svr.listen("0.0.0.0", 8080) ? (std::cerr << "Server failed to start!" << std::endl) : (std::cout << "Server is running!" << std::endl);
	
	return 0;
}
