#include <iostream>
#include <string>
#include <memory>
#include <httplib.h>
#include <jwt-cpp/jwt.h>
#include "../repositories/PersonRepository.hpp"

struct RegistrationResult {
    bool success;
    int errorCode;
    std::string errorMessage;
};

struct LoginResult {
    bool success;
    int errorCode;
    std::string errorMessage;
    std::string token;
};

struct TokenValidationResult {
    bool success;
    int errorCode;
    std::string errorMessage;
    bool valid;
    std::string userId;
    std::string email;
};

struct PasswordChangeResult {
    bool success;
    int errorCode;
    std::string errorMessage;
};

class AuthService {
public:
    AuthService(std::shared_ptr<PersonRepository> repo)
        : repo(repo){

    }

    RegistrationResult registerUser(const std::string& email, const std::string& password) {
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
        } catch (const odb::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Database error: " + std::string(e.what());
        }
        catch (const std::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Unexpected error: " + std::string(e.what());
        }

        return result;
    }
    
    LoginResult loginUser(const std::string& email, const std::string& password) {
        LoginResult result;

        if (email.empty() || password.empty()) {
            result.success = false;
            result.errorCode = 400;
            result.errorMessage = "Email or password is missing!";
            return result;
        }

        try {
            bool isCreated = repo->personExist(email, password);
           
            if (isCreated) {
                result.success = false;
                result.errorCode = 401;
                result.errorMessage = "Login failed: no matching user";
            } else {
                result.token = repo->loginPerson(email, password);
                result.success = true;
                result.errorCode = 0;
            }
        }
        catch (const odb::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Database error: " + std::string(e.what());
        }
        catch (const std::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Unexpected error: " + std::string(e.what());
        }

        return result;
    }

    TokenValidationResult validateToken(const std::string& token) {
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

    PasswordChangeResult changePassword(const std::string& email, const std::string& oldPassword, const std::string& newPassword) {
        PasswordChangeResult result;
        
        if (email.empty() || oldPassword.empty() || newPassword.empty()) {
            result.success = false;
            result.errorCode = 400;
            result.errorMessage = "Email, old or new password missing!";
            return result;
        }
        
        try {
            bool isCreated = repo->personExist(email, oldPassword);
            
            if (isCreated) {
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
        catch (const odb::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Error while changing password:" + std::string(e.what());
        }
        catch (const std::exception& e) {
            result.success = false;
            result.errorCode = 500;
            result.errorMessage = "Unexpected error: " + std::string(e.what());
        }
        
        return result;
    }

private:
    std::shared_ptr<PersonRepository> repo;
};
