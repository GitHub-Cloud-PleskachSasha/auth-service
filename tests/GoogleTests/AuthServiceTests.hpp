#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/services/AuthService.hpp"
#include "MockPersonRepository.hpp"

TEST(AuthServiceTest, RegisterUser_EmailAlreadyExists) {
    auto repo = std::make_shared<MockPersonRepository>();
    AuthService authService(repo);
    
    EXPECT_CALL(*repo, emailExists("test@example.com")).WillOnce(testing::Return(true));
    
    RegistrationResult result = authService.registerUser("test@example.com", "password");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, 409);
    EXPECT_EQ(result.errorMessage, "Email is already in use!");
}

TEST(AuthServiceTest, LoginUser_InvalidCredentials) {
    auto repo = std::make_shared<MockPersonRepository>();
    AuthService authService(repo);
    
    EXPECT_CALL(*repo, personExist("test@example.com", "wrongpassword")).WillOnce(testing::Return(false));
    
    LoginResult result = authService.loginUser("test@example.com", "wrongpassword");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, 409);
    EXPECT_EQ(result.errorMessage, "Login failed: no matching user");
}

TEST(AuthServiceTest, LoginUser_Success) {
    auto repo = std::make_shared<MockPersonRepository>();
    AuthService authService(repo);
    
    EXPECT_CALL(*repo, personExist("test@example.com", "password")).WillOnce(testing::Return(true));
    EXPECT_CALL(*repo, loginPerson("test@example.com", "password")).WillOnce(testing::Return("valid_token"));
    
    LoginResult result = authService.loginUser("test@example.com", "password");
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.errorCode, 0);
    EXPECT_EQ(result.token, "valid_token");
}

TEST(AuthServiceTest, ValidateToken_Invalid) {
    AuthService authService(nullptr);
    TokenValidationResult result = authService.validateToken("invalid_token");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, 401);
}

TEST(AuthServiceTest, ChangePassword_Success) {
    auto repo = std::make_shared<MockPersonRepository>();
    AuthService authService(repo);
    
    EXPECT_CALL(*repo, personExist("test@example.com", "oldpass")).WillOnce(testing::Return(true));
    EXPECT_CALL(*repo, changePassword("test@example.com", "oldpass", "newpass")).WillOnce(testing::Return(true));
    
    PasswordChangeResult result = authService.changePassword("test@example.com", "oldpass", "newpass");
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.errorCode, 0);
    EXPECT_EQ(result.errorMessage, "Password successfully updated");
}

TEST(AuthServiceTest, ChangePassword_InvalidOldPassword) {
    auto repo = std::make_shared<MockPersonRepository>();
    AuthService authService(repo);
    
    EXPECT_CALL(*repo, personExist("test@example.com", "wrongoldpass")).WillOnce(testing::Return(false));
    
    PasswordChangeResult result = authService.changePassword("test@example.com", "wrongoldpass", "newpass");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, 401);
    EXPECT_EQ(result.errorMessage, "Password change failed: incorrect email or old password.");
}
