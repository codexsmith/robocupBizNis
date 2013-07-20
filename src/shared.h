/* 
 * File:   shared.h
 * Author: beruke
 *
 * Created on July 18, 2013, 8:07 PM
 */

#ifndef SHARED_H
#define	SHARED_H

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;

class Shared
{
public:
    static inline void setCommand(const char command)
    {
        char *c = static_cast<char *>(getInstance().region.get_address());
        *c = command;
    }
    
    static inline char * getCommand()
    {
        return static_cast<char *>(getInstance().region.get_address());
    }
    
private:
    mapped_region region;
        
    inline Shared()
    {
        shared_memory_object shared(open_or_create, "shared", read_write);
        shared.truncate(1024);
        region = mapped_region(shared, read_write);
    };
    
    static inline Shared& getInstance()
    {
        static Shared instance;
        return instance;
    }
    
    inline Shared(Shared const&);
    void inline operator=(Shared const&);
};

#endif	/* SHARED_H */
