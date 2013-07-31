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

struct commandStruct
{
    // No space between ':' and characters, i.e. A:H, not A: H
    char aggression; // A:[H,M,L]     High, Medium, Low
    char strategy;   // S:[P,D,H]     Pass, Dribble, Hold
    char location;   // L:[L,C,R,S]   Left, Center, Right, Sidelines
    
    commandStruct(): aggression('N'), strategy('N'), location('N') { }
};

class Shared
{
public:
    static inline void setCommand(std::string *input)
    {
        commandStruct *cmd = static_cast<commandStruct *>
                (getInstance().region.get_address());
        
        if (input->length() == 0 || input->at(0) == 'N')
        {
            cmd->aggression = cmd->strategy = cmd->location = 'N';
            return;
        }
        
        std::size_t agg = input->find("A:");
        std::size_t str = input->find("S:");
        std::size_t loc = input->find("L:");
        
        if (agg != std::string::npos && agg + 2 < input->length())
            cmd->aggression = input->at(agg +2);
        if (str != std::string::npos && str + 2 < input->length())
            cmd->strategy = input->at(str + 2);
        if (loc != std::string::npos && loc + 2 < input->length())
            cmd->location = input->at(loc + 2);
    }
    
    static inline commandStruct * getCommand()
    {
        return static_cast<commandStruct *>(getInstance().region.get_address());
    }
    
private:
    boost::interprocess::mapped_region region;
        
    inline Shared()
    {
        boost::interprocess::shared_memory_object shared(
                boost::interprocess::open_or_create, "shared",
                boost::interprocess::read_write);
        shared.truncate(sizeof(commandStruct));
        region = boost::interprocess::mapped_region(
                shared, boost::interprocess::read_write);
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
