#include <iostream>
#include <string>
#include <memory>
#include <httplib.h>
#include <jwt-cpp/jwt.h>
#include "IAuthService.hpp"
#include "../repositories/IPersonRepository.hpp"


class AuthService : public IAuthService {
public:
    AuthService(std::shared_ptr<IPersonRepository> repo)
        : repo(repo){
    }

    RegistrationResult registerUser(const std::string& email, const std::string& password) override {
        RegistrationResult result;

        if (email.empty() || password.empty()) {
            result.success = false;
            result.errorCode = 400;
            result.errorMessage = "Email or password is missing!";
            return result;
        }

        try {
            if (repo->emailExists(email)) {
                result.success = false;
                result.errorCode = 409;
                result.errorMessage = "Email is already in use!";
                return result;
            }
            repo->createPerson(email, password);
            result.success = true;
            result.errorCode = 0;
            result.errorMessage = "";
        } 
        catch (const std::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Unexpected error: " + std::string(e.what());
        }

        return result;
    }
    
    LoginResult loginUser(const std::string& email, const std::string& password) override {
        LoginResult result;

        if (email.empty() || password.empty()) {
            result.success = false;
            result.errorCode = 400;
            result.errorMessage = "Email or password is missing!";
            return result;
        }

        try {
            bool exists = repo->personExist(email, password);
           
            if (!exists) {
                result.success = false;
                result.errorCode = 409;
                result.errorMessage = "Login failed: no matching user";
            } else {
                result.token = repo->loginPerson(email, password);
                result.success = true;
                result.errorCode = 0;
            }
        }
        catch (const std::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Unexpected error: " + std::string(e.what());
        }

        return result;
    }

    TokenValidationResult validateToken(const std::string& token) override {
        TokenValidationResult result;

        if (token.empty()) {
            result.success = false;
            result.errorCode = 400;
            result.errorMessage = "Token is missing";
            return result;
        }
    
        try {
            auto decoded_token = jwt::decode(token);
    
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{"secret_key"})
                .with_issuer("authservice");
    
            verifier.verify(decoded_token);
    
            result.userId = decoded_token.get_payload_claim("user_id").as_string();
            result.email = decoded_token.get_payload_claim("email").as_string();
            result.valid = true;
            result.success = true;
            result.errorCode = 0;
        }
        catch (const std::exception& e) {
            result.success = false;
            result.errorCode = 401;
            result.errorMessage = std::string("Invalid token: ") + e.what();
            result.valid = false;
        }

        return result;
    }

    PasswordChangeResult changePassword(const std::string& email, const std::string& oldPassword, const std::string& newPassword) override {
        PasswordChangeResult result;
        
        if (email.empty() || oldPassword.empty() || newPassword.empty()) {
            result.success = false;
            result.errorCode = 400;
            result.errorMessage = "Email, old or new password missing!";
            return result;
        }
        
        try {
            bool exists = repo->personExist(email, oldPassword);
            
            if (!exists) {
                result.success = false;
                result.errorCode = 401;
                result.errorMessage = "Password change failed: incorrect email or old password.";
                return result;
            }
            
            bool isUpdated = repo->changePassword(email, oldPassword, newPassword);
            if (!isUpdated) {
                result.success = false;
                result.errorCode = 400;
                result.errorMessage = "Failed to update password";
            } else {
                result.success = true;
                result.errorCode = 0;
                result.errorMessage = "Password successfully updated";
            }
        }
        catch (const std::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Unexpected error: " + std::string(e.what());
        }
        
        return result;
    }

private:
    std::shared_ptr<IPersonRepository> repo;
};
