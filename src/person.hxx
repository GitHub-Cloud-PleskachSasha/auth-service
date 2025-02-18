#include <string>
#include <odb/core.hxx>

#pragma db object
class person
{
public:
  person (const std::string& email_, const std::string& password_) 
          :email_(email_), password_(password_) {}

  const std::string& email () const { return email_; }
  const std::string& password () const { return password_; }

private:
  person () = default;             

  friend class odb::access; 

  #pragma db id auto        
  unsigned long long id_;

  std::string email_;
  std::string password_;
};
  
