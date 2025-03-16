#include <string>
#include <odb/core.hxx>

#pragma db object
class person
{
public:
  person (const std::string& email, const std::string& password) 
            :m_email(email), m_password(password) {}

  const std::string& email () const { return m_email; }
  const std::string& password () const { return m_password; }
  unsigned long getId() const { return id; }

  void setPassword(const std::string& newPassword) { m_password = newPassword; }
private:
  person () {};             

  friend class odb::access; 

  #pragma db id auto
  unsigned long id;

  #pragma db unique
  std::string m_email;
  std::string m_password;
};
  
