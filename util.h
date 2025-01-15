#ifndef UTIL_H
#define UTIL_H

#include <regex>
#include <string_view>
#include <vector>
#include <sstream>
#include <iostream>
#include <bitset>


namespace util {

    enum MaskFormat {
        Octets,
        CIDR,
    };

    bool validAddress(const std::string_view addr) {
        std::regex validIPv4(R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$)");

        std::string addr_str { addr };
        if (std::regex_match(addr_str, validIPv4)) {
            return true;
        }
        else {
            return false;
        }
    }

    bool validMask(const std::string_view mask, const MaskFormat format) {
        std::string mask_str { mask };

        if (format == MaskFormat::Octets) {
            std::regex validMask(R"(^(255)\.(0|128|192|224|240|248|252|254|255)\.(0|128|192|224|240|248|252|254|255)\.(0|128|192|224|240|248|252|254|255))");
            if (std::regex_match(mask_str, validMask)) {
                return true;
            }
            else {
                return false;
            }
        }

        if (format == MaskFormat::CIDR) {
            std::regex validMask(R"(^\/(3[0-2]|[12]?[0-9])$)");
            if (std::regex_match(mask_str, validMask)) {
                return true;
            }
            else {
                return false;
            }

        }

        return false;
    }

    std::vector<std::string> splitAddress(const std::string_view address) {
        std::istringstream stream(address.data());
        std::vector<std::string> octets;
        std::string token;

        while (std::getline(stream, token, '.')) {
            if (!token.empty()) {
                octets.push_back(token);
                std::cout << token << std::endl;
            }
        }

        return octets;
    }

    std::string ipToBinary(const std::vector<std::string>& octets) {
        std::string binary { "" };
        for (auto& octet : octets) {
            int octet_int = std::stoi(octet);
            std::string octet_binary = std::bitset<8>(octet_int).to_string();
            binary += octet_binary;
        }

        return binary;
    }

    int networkBits(const std::string_view& mask, const MaskFormat format) {
        int bits { 0 };
        if (format == MaskFormat::CIDR) {
            std::string_view bits_substr = mask.substr(1); // Gets the digits from '/24'
            bits = std::stoi(std::string { bits_substr });
        }

        else {
            std::vector<std::string> octets = splitAddress(mask);
            for (auto& octet_str : octets) {
                auto octet_int = std::stoi(octet_str);
                if (octet_int != 0) {
                    while (octet_int > 0) {
                        bits += octet_int % 2;
                        octet_int >>= 1;
                    }
                }
                else {
                    break;
                }
            }
        }

        return bits;
    }

    void buildSubnets(const std::string_view& address, int subnetBits) {
        auto octets = splitAddress(address);
        auto ip_binary = ipToBinary(octets);
        std::cout << ip_binary << std::endl;
    }
}

#endif // UTIL_H
