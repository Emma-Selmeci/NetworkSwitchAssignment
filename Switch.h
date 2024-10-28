// Created by Selmeci Emma on 2024. 10. 02.
/*
    Sources:
    I used the C++ reference to learn more about the unordered map: https://cplusplus.com/reference/unordered_map/unordered_map/
    I couldn't iterate through a map without invalidating the iterator, so I used this handy stackoverflow question: https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
    I used this wiki page for the char magic: https://en.wikipedia.org/wiki/ISO/IEC_8859-1
    I used this page for the STP MAC address: https://standards.ieee.org/products-programs/regauth/grpmac/public/
    I've selected one that is not used currently, hopefully I won't cause any harm with that
    And of course I used this wonderful book by Bjarne Stroustrup, the sassiest computer scientist known to man: https://www.amazon.com/C-Programming-Language-4th/dp/0321563840
*/

#ifndef SWITCH_H
#define SWITCH_H

#include <unordered_map>
#include <string>

namespace EmmaLib {

class Switch {

    struct AddressEntry {
        int port;
        int remaining_time = 10; //Decremented after every input
        void refresh(int newPort) {
            remaining_time = 10;
            port = newPort;
        }
        int defresh() {
            return --remaining_time;
        }
    };

    std::unordered_map<std::string, AddressEntry> macAddressTable;

    //So if I'm correct, switches have their own MAC address for the purposes of STP. I'll give one myself for now
    static const char STATICALLY_ASSIGNED_MAC_ADDRESS[13];
    static const char STP_MAC[13];

    static constexpr char NUMOFPORTS = '8';
    static constexpr char NUMOFPORTSINTEGRAL = 8;

    bool portStates[NUMOFPORTS]{};
    bool processFrame(const std::string&);
    inline bool validateChar(char) const;
    bool validateLength(const std::string&, int&) const;
    void stp() const;
public:
    Switch() {
        for(int i = 0; i < NUMOFPORTSINTEGRAL; ++i) portStates[i] = true;
    }
    void use();
};

} // EmmaLib

#endif //SWITCH_H
