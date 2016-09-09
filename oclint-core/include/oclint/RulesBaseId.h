/**
    \brief Rule base ID management
    The implementation is based on the STL library
    \todo Read the <name, id> association from a file, for now it is statically initiliazed in cpp
*/

#ifndef __RULESBASEID
#define __RULESBASEID

#include <string>
#include <map>
#include <iostream>

/**
 \brief convenient definition
 \todo define it private, no use in public
*/
typedef std::map<std::string, int> MapBaseId;
typedef MapBaseId::value_type BaseIdPair;

//! Rules ID management <name, id> pair
/*
    pair <name,ID> shall be unique
 */
class RulesBaseId
{
// Methods
public:

    /**
     \brief Get the ID rule for a rule name
     \param[in] szName Rule name
     \return the Id value
     \retval > 0
     */
    static RulesBaseId & getInstance(void) ;
    
    /**
     \brief Get the ID rule for a rule name
     \param[in] szName Rule name
     \return the Id value
     \retval >= 0 ID rule
     \retval == -1 Name not found
    */
    int getIdForRule( const std::string & szName );
    
    /**
     \brief Print to stdout base content
     */
    void printContent( void );
    
private:

    /**
     \brief Constructor private because we use singleton
     */
    RulesBaseId(){};

    /**
     \brief Constructor private because we use singleton
     */
    RulesBaseId( RulesBaseId const & ){};

    /**
     \brief copy private because we use singleton
     */
    RulesBaseId& operator=( RulesBaseId const& );
    
    /**
     \brief copy private because we use singleton
     */
    ~RulesBaseId() {};
    
// Members
public:
    
private:
    // The MAP Id statically initialized (C++11 support)
    static MapBaseId m_eBaseId;
};

/**
 \brief Display a pair content (for debug/test puprose)
 \param[in] &a stream
 \param[in] &b the pair to display
 \return stream output
 */
std::ostream & operator<<(std::ostream &a, const BaseIdPair &b);

#endif
