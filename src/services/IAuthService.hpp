#include <string>

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

class IAuthService {
public:
    virtual RegistrationResult registerUser(const std::string& email, const std::string& password) = 0;
    virtual LoginResult loginUser(const std::string& email, const std::string& password) = 0;
    virtual TokenValidationResult validateToken(const std::string& token) = 0;
    virtual PasswordChangeResult changePassword(const std::string& email, const std::string& oldPassword, const std::string& newPassword) = 0;
    virtual ~IAuthService() = default;
};


