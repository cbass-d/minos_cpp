#pragma once

#include <regex>
#include <string_view>
#include <vector>
#include <sstream>
#include <iostream>
#include <cmath>
#include <bitset>


namespace util {

    enum MaskFormat {
        Octets,
        CIDR,
    };

    struct SubnetInfo {
        std::string subnet;
        std::string network;
        std::string first;
        std::string last;
        std::string broadcast;
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

    unsigned long long ipToDecimal(const std::string_view& address) {
        auto octets = splitAddress(address);
        auto ip_binary = ipToBinary(octets);

        return std::bitset<32>(ip_binary).to_ullong();
    }

    std::string  decimalToIp(const unsigned long long decimal) {
        auto ip_binary = std::bitset<32>(decimal).to_string();

        std::vector<std::string> binary_octets {};
        for (int i = 0; i < ip_binary.length(); i += 8) {
            binary_octets.push_back(ip_binary.substr(i, 8));
        }

        int ind = 0;
        std::string ip_str { "" };
        for (auto& binary_octet : binary_octets) {
            auto octet = std::bitset<8>(binary_octet).to_ulong();
            ip_str += std::to_string(octet);

            if (ind < 3) {
                ip_str += ".";
                ind++;
            }
        }

        return ip_str;
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

    std::string subnet_cidr(unsigned long long address, int subnet_bits) {
        auto ip = decimalToIp(address);
        auto formatted = std::format("{}\\{}", ip, subnet_bits);

        return formatted;
    }

    std::string first_host(unsigned long long address) {
        return decimalToIp(address + 1);
    }

    std::string last_host(unsigned long long address, int num_hosts) {
        return decimalToIp(address + num_hosts);
    }

    std::string broadcast_address(unsigned long long address, int num_hosts) {
        return decimalToIp(address + num_hosts + 1);
    }

    std::vector<std::vector<std::string>> buildSubnets(const std::string_view& address, int network_bits, int subnet_bits, int num_subnets) {
        auto octets { splitAddress(address) };
        auto ip_decimal { ipToDecimal(address) };

        int new_subnetbits = network_bits + subnet_bits;
        int increment = std::pow(2, 32 - new_subnetbits);
        int subnet_hosts = increment - 2;

        std::string mask_binary { "" };
        int network_bits_tmp { network_bits - 1 };
        for (int i = 0; i < 31; i++) {
            if (network_bits_tmp > 0) {
                mask_binary += "1";
                network_bits_tmp--;
            }
            else {
                mask_binary += "0";
            }
        }
        auto mask_decimal { std::bitset<32>(mask_binary).to_ullong() };

        std::vector<std::vector<std::string>> networks {};
        unsigned long long last_address {};
        for (int i = 0; i < num_subnets; i++) {
            if (i == 0) {
                auto new_address_decimal = ip_decimal & mask_decimal;
                last_address = new_address_decimal;
            }
            else {
                auto new_address_decimal = last_address + increment;
                last_address = new_address_decimal;
            }

            std::string cidr = subnet_cidr(last_address, new_subnetbits);
            std::string network = decimalToIp(last_address);
            std::string first = first_host(last_address);
            std::string last = last_host(last_address, subnet_hosts);
            std::string broadcast = broadcast_address(last_address, subnet_hosts);

            std::vector<std::string> subnet { cidr, network, first, last, broadcast };
            networks.push_back(subnet);

        }

        return networks;
    }
}
