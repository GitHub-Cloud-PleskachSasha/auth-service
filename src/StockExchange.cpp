// #define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <thread>
#include <httplib.h>
// #include <openssl/opensslv.h>

int main(int argc, char* argv[])
{
	// httplib::SSLServer svr("./cert.pem", "./key.pem");
	httplib::Server svr;

	// if (!svr.is_valid()) {
	// 	std::cerr << "Invalid SSL server configuration!" << std::endl;
	// 	return 1;
	// }

	svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
		res.set_content("Hello World1!2", "text/plain");
	});

	std::thread th([&svr](){
		auto res = svr.listen("0.0.0.0", 8080);
	});

	while(true) {
		std::cout << "I am alive" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}

	th.join();

	return 0;
}