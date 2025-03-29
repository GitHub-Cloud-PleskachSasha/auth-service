#ifndef IPERSONREPOSITORY_HPP
#define IPERSONREPOSITORY_HPP

#include <string>

class IPersonRepository {
public:
    virtual bool emailExists(const std::string& email) = 0;
    virtual void createPerson(const std::string& email, const std::string& password) = 0;
    
    virtual bool personExist(const std::string& email, const std::string& password) = 0;
    virtual std::string loginPerson(const std::string& email, const std::string& password) = 0;
    virtual bool changePassword(const std::string& email, const std::string& oldPassword, const std::string& newPassword) = 0;
    virtual ~IPersonRepository() = default;
};

#endif 
