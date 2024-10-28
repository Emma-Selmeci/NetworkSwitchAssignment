// Created by Selmeci Emma on 2024. 10. 02.

#include "Switch.h"

#include <iostream>

namespace EmmaLib {

    const char Switch::STATICALLY_ASSIGNED_MAC_ADDRESS[13] = {"424242424222"}; //Fancy programmer number (I originally wanted it to end with 42, but then I remembered that you might be using the same MAC address to test)
    const char Switch::STP_MAC[13] = {"0180C2000005"};

    void Switch::use() {
        std::string s;
        stp();

        for(;;) {
            std::cout << "#Awaiting input\n";
            std::getline(std::cin,s);
            if(s[0] != '#') if(!processFrame(s)) std::cout << "\n#Invalid frame\n";
        }

    }

    //I know this function is way too long but this is still less crumblesome than having it devided into many functions passing the same values around
    //The checks regarding the frame should optimally happen ordered by how likely they are to fail
    bool Switch::processFrame(const std::string& s) {
        //std::cout << "\n#Starting frame validation\n";
        //I'm pretty sure the optimizer will connect these into a single OR statement
        if(s.size() < 60 || s.size() > 1514) return false;
        if(!(s[0] >= '0' && s[0] <= NUMOFPORTS)) return false;
        if(s[1] != '-') return false;
        if(s[2] != '>') return false;

        if(!portStates[s[0] - '0']) {
            std::cout << "\n#Input not processed - port blocked\n";
            return true;
        }

        //std::cout << "#Validating destination address\n";
        //I'm putting the data into these arrays temporarily
        char to[12];
        char from[12];
        int length = 0;
        for(int j = 0, i = 3; j < 12; ++j, ++i) { //Validating the address of the destination device
            if(!validateChar(s[i])) return false;
            to[j] = toupper(s[i]);
        }
        //std::cout << "#Validating source address\n";
        for(int j = 0, i = 3+12; j < 12; ++j, ++i) { //Validating the address of the source device
            if(!validateChar(s[i])) return false;
            from[j] = toupper(s[i]);
        }
        if(from[1] == '1' || from[1] == '3' || from[1] == '5' || from[1] == '7' || from[1] == '9' || from[1] == 'B' || from[1] == 'D' || from[1] == 'F') return false;
        //std::cout << "#Validating length\n";
        if(!validateLength(s,length)) return false;
        if(length <= 1500) {
            if(s.size() < length) return false;
        }
        //std::cout << "#Validating chars\n";
        for(int i = 3+12+12+4; i < s.size(); ++i) {
            if(!validateChar(s[i])) return false;
        }
        //Congratulations, you are a valid frame! (For the purposes of this exercise)

        std::string fromStr{from,12}; //Constructing a string from the first 12 chars of from
        std::string toStr{to,12};

        if(toStr == STP_MAC && fromStr == STATICALLY_ASSIGNED_MAC_ADDRESS) { //This is one of those STP frames we've sent during startup!
            if(portStates[s[0] - '0']) { //If the port we've received this frame from still opened
                portStates[s[3+12+12+4] - '0'] = false; //We're closing the port that sent the message
                std::cout << "#Blocking port : " << s[3+12+12+4] << '\n';
            }
            return true;
        }

        //This is where I used the stackoverflow code - I've since understood why this was necessary
        for(auto it = macAddressTable.begin(); it != macAddressTable.end(); ) {
            if(it->second.defresh() == 0) {
                it = macAddressTable.erase(it);
            } else {
                ++it;
            }
        }

        macAddressTable[fromStr].refresh(s[0] - '0'); //This will insert if it's not there and overwrite if it's there

        auto it = macAddressTable.find(toStr);
        if(it != macAddressTable.end()) {
            std::cout << it->second.port << "<-";
            std::cout << s.substr(3,s.size()) << '\n';
            return true;
        } else {
            for(int i = 0; i < NUMOFPORTSINTEGRAL; ++i) {
                if(portStates[i] && i != s[0] - '0') {
                    std::cout << i << "<-";
                    std::cout << s.substr(3,s.size()) << '\n';
                }
            }
        }

        return true;
    }

    bool Switch::validateChar(const char c) const {
        if(c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F') return true; return false;
    }

    bool Switch::validateLength(const std::string& s, int& length) const {
        for(int j = 0, i = 3+12+12; j < 4; ++i, ++j) {
            length*=16;
            if(s[i] >= '0' && s[i] <= '9') {
                length += s[i] - '0';
            } else if(s[i] >= 'a' && s[i] <= 'f' || s[i] >= 'A' && s[i] <= 'F') {
                char temp = toupper(s[i]);
                length += temp - 'A';
            } else return false; //The character is invalid :(
        }
        return true;
    }

    void Switch::stp() const {
        for(int i = 0; i < NUMOFPORTSINTEGRAL; ++i) {
            std::cout << i << "<-" << STP_MAC << STATICALLY_ASSIGNED_MAC_ADDRESS << "0001" //Magic IP address
            << i << "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\n"; //A for loop would be quite slow :(
        }
    }

} // EmmaLib