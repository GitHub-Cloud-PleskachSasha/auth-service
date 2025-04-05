#ifndef MOCKPERSONREPOSITORY_HPP
#define MOCKPERSONREPOSITORY_HPP

#include <gmock/gmock.h>
#include "../src/repositories/IPersonRepository.hpp"

class MockPersonRepository : public IPersonRepository {
public:
    MOCK_METHOD(bool, emailExists, (const std::string& email), (override));
    MOCK_METHOD(void, createPerson, (const std::string& email, const std::string& password), (override));
    MOCK_METHOD(bool, personExist, (const std::string& email, const std::string& password), (override));
    MOCK_METHOD(std::string, loginPerson, (const std::string& email, const std::string& password), (override));
    MOCK_METHOD(bool, changePassword, (const std::string& email, const std::string& oldPassword, const std::string& newPassword), (override));
};

#endif