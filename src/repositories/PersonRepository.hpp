#include <iostream>
#include <string>
#include <memory>

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/query.hxx>

#include "person-odb.hxx"

class PersonRepository {
public:
    PersonRepository(std::shared_ptr<odb::database> db) : m_db(std::move(db)) {
        if (!m_db) {
            throw std::runtime_error("Database pointer is null!");
        }
        try 
        {
            odb::transaction t(m_db->begin());
            m_db->execute("CREATE TABLE IF NOT EXISTS person (id INT AUTO_INCREMENT PRIMARY KEY, email VARCHAR(255) UNIQUE, password VARCHAR(255))");
            t.commit();
        } catch (const std::exception& e) {
            std::cerr << "Database initialization error: " << e.what() << std::endl;
            throw;
        }
    }

    bool emailExists(std::string email) {
        odb::transaction t(m_db->begin());
        odb::result<person> r(m_db->query<person>(odb::query<person>::email == email));
        t.commit();

        return !r.empty();
    }
    void createPerson(std::string email, std::string password) {
        odb::transaction t(m_db->begin());
        person newPerson(email, password);
        m_db->persist(newPerson);
        t.commit();
    }
    bool personExist(std::string email, std::string password) {
        odb::transaction t(m_db->begin());
   
        odb::result<person> p(m_db->query<person>(
            odb::query<person>::email == email &&
            odb::query<person>::password == password
        ));

        return p.empty();
    }
    odb::result<person> getPerson(std::string email, std::string password) {
        odb::result<person> p(m_db->query<person>(
            odb::query<person>::email == email &&
            odb::query<person>::password == password
        ));
            
        return p;
    }

    auto loginPerson(std::string email, std::string password) {
        odb::transaction t(m_db->begin());

        odb::result<person> p = getPerson(email, password);

        person user = *p.begin();

        auto token = jwt::create()
            .set_issuer("authservice")        
            .set_type("JWS")                      
            .set_issued_at(std::chrono::system_clock::now()) 
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24)) 
            .set_payload_claim("user_id", jwt::claim(std::to_string(user.getId())))   
            .set_payload_claim("email", jwt::claim(user.email()))                      
            .sign(jwt::algorithm::hs256{"secret_key"}); 


        return token;
    }

private:
    std::shared_ptr<odb::database> m_db;
};
