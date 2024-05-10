
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <iterator>
#include <vector>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <sstream>

typedef uint8_t byte_t;


std::string int_to_hex(uint8_t integer)
{
    std::stringstream sstream;
    sstream << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << unsigned(integer);
    return sstream.str();
}

std::string int_to_hex(uint16_t integer)
{
    std::stringstream sstream;
    sstream << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << unsigned(integer);
    return sstream.str();
}

class Instruction
{

private:
    byte_t high;
    byte_t low;
    byte_t msb;
    uint8_t n, x, y; // 4 bits
    uint8_t kk;      // 8 bits
    uint16_t nnn;    // 12 bits

public:
    Instruction(const byte_t high_byte, const byte_t low_byte)
    {
        high = high_byte;
        low = low_byte;
        msb = high >> 4;

        n = low & 0xF;
        x = high & 0xF;
        y = low >> 4;
        kk = low;
        nnn = ((uint16_t)x << 8) | low;
    }

    Instruction(const uint16_t bytes)
    {
        Instruction(bytes >> 8, bytes & 0xFF);
    }

    uint16_t get_instruction() { return (high << 8) | low; }
    uint8_t get_most_significant_byte() { return msb; }
    uint8_t get_least_significant_byte() { return n; }
    uint8_t get_n() { return n; }
    uint8_t get_x() { return x; }
    uint8_t get_y() { return y; }
    uint8_t get_kk() { return kk; }
    uint16_t get_nnn() { return nnn; }
    byte_t get_high() { return high; }
    byte_t get_low() { return low; }
};

std::string translate_instruction(Instruction instruction)
{
    uint8_t nibble = instruction.get_n();
    uint8_t x = instruction.get_x();
    uint8_t y = instruction.get_y();
    uint8_t byte = instruction.get_kk();
    uint16_t addr = instruction.get_nnn();

    switch (instruction.get_most_significant_byte())
    {
    case 0x0:
        switch (instruction.get_instruction())
        {
        case 0x00E0:
            // 00E0 - CLS
            return "CLS";
            break;
        case 0x00EE:
            // 00EE - RET
            return "RET";
            break;
        default:
            // 0nnn - SYS addr
            return "SYS   #" + int_to_hex(addr);
            break;
        }
        break;
    case 0x1:
        // 1nnn - JP addr
        return "JP    #" + int_to_hex(addr);
        break;
    case 0x2:
        // 2nnn - CALL addr
        return "CALL  #" + int_to_hex(addr);
        break;
    case 0x3:
        // 3xkk - SE Vx, byte
        return "SE    V" + std::to_string(x) + ", #" + int_to_hex(byte);
        break;
    case 0x4:
        // 4xkk - SNE Vx, byte
        return "SNE   V" + std::to_string(x) + ", #" + int_to_hex(byte);
        break;
    case 0x5:
        // 5xy0 - SE Vx, Vy
        return "SNE   V" + std::to_string(x) + ", V" + std::to_string(y);
        break;
    case 0x6:
        // 6xkk - LD Vx, byte
        return "LD    V" + std::to_string(x) + ", #" + int_to_hex(byte);
        break;
    case 0x7:
        // 7xkk - ADD Vx, byte
        return "ADD   V" + std::to_string(x) + ", #" + int_to_hex(byte);
        break;
    case 0x8:
        // 8xy0, 8xy1, 8xy2, 8xy3, 8xy4, 8xy5, 8xy6, 8xy7, 8xyE
        switch (instruction.get_least_significant_byte())
        {
        case 0:
            // 8xy0 - LD Vx, Vy
            return "LD    V" + std::to_string(x) + ", V" + std::to_string(y);
            break;
        case 0X1:
            // 8xy1 - OR Vx, Vy
            return "OR    V" + std::to_string(x) + ", V" + std::to_string(y);
            break;
        case 0X2:
            // 8xy2 - AND Vx, Vy
            return "AND   V" + std::to_string(x) + ", V" + std::to_string(y);
            break;
        case 0X3:
            // 8xy3 - XOR Vx, Vy
            return "XOR   V" + std::to_string(x) + ", V" + std::to_string(y);
            break;
        case 0X4:
            // 8xy4 - ADD Vx, Vy
            return "ADD   V" + std::to_string(x) + ", V" + std::to_string(y);
            break;
        case 0X5:
            // 8xy5 - SUB Vx, Vy
            return "SUB   V" + std::to_string(x) + ", V" + std::to_string(y);
            break;
        case 0X6:
            // 8xy6 - SHR Vx {, Vy}
            return "SHR   V" + std::to_string(x) + "{, V" + std::to_string(y) + "}";
            break;
        case 0x7:
            // 8xy7 - SUBN Vx, Vy
            return "SUBN  V" + std::to_string(x) + ", V" + std::to_string(y);
            break;
        case 0xE:
            // 8xyE - SHL Vx {, Vy}
            return "SHL   V" + std::to_string(x) + "{, V" + std::to_string(y) + "}";
            break;
        }
        break;
    case 0x9:
        // 9xy0 - SNE Vx, Vy
        return "SNE   V" + std::to_string(x) + ", V" + std::to_string(y);
        break;
    case 0xA:
        // Annn - LD I, addr
        return "LD    I, #" + int_to_hex(addr);
        break;
    case 0xB:
        // Bnnn - JP V0, addr
        return "JP    V0, #" + int_to_hex(addr);
        break;
    case 0xC:
        // Cxkk - RND Vx, byte
        return "RND   V" + int_to_hex(x) + ", #" + int_to_hex(byte);
        break;
    case 0xD:
        // Dxyn - DRW Vx, Vy, nibble
        return "DRW   V" + std::to_string(x) + ", V" + std::to_string(y) + ", #" + int_to_hex(nibble);
        break;
    case 0xE:
        // Ex9E and ExA1
        switch (instruction.get_least_significant_byte())
        {
        case 0x9E:
            // Ex9E - SKP Vx
            return "SKP   V" + std::to_string(x);
            break;
        case 0xA1:
            // ExA1 - SKNP Vx
            return "SKNP  V" + std::to_string(x);
            break;
        }
        break;
    case 0xF:
        // Fx07, Fx0A, Fx15, Fx18, Fx1E, Fx29, Fx33, Fx55, Fx65
        switch (instruction.get_least_significant_byte())
        {
        case 0x07:
            // Fx07 - LD Vx, DT
            return "LD    V" + std::to_string(x) + ", DT";
            break;
        case 0x0A:
            // Fx0A - LD Vx, K
            return "LD    V" + std::to_string(x) + ", K";
            break;
        case 0x15:
            // Fx15 - LD DT, Vx
            return "LD    DT, V" + std::to_string(x);
            break;
        case 0x18:
            // Fx18 - LD ST, Vx
            return "LD    ST, V" + std::to_string(x);
            break;
        case 0x1E:
            // Fx1E - ADD I, Vx
            return "ADD   I, V" + std::to_string(x);
            break;
        case 0x29:
            // Fx29 - LD F, Vx
            return "LD    F, V" + std::to_string(x);
            break;
        case 0x33:
            // Fx33 - LD B, Vx
            return "LD    B, V" + std::to_string(x);
            break;
        case 0x55:
            // Fx55 - LD [I], Vx
            return "LD    [I], V" + std::to_string(x);
            break;
        case 0x65:
            // Fx65 - LD Vx, [I]
            return "LD    V" + std::to_string(x) + ", [I]";
            break;
        }
        break;
    }

    return "??";
}

const std::vector<byte_t> read_file(const std::string file_name)
{
    // Read segment data
    std::ifstream rom_file(file_name, std::ios::binary);

    // Output error if error
    if (!rom_file.is_open())
    {
        std::cerr << "Error opening " << file_name << ":\n";
        if (rom_file.fail())
            std::cerr << std::strerror(errno) << std::endl;

        exit(EXIT_FAILURE);
    }

    // Put file into modifiable buffer
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(rom_file), {});

    // Get segment length
    size_t segment_length = rom_file.tellg();

    // Close file
    rom_file.close();

    // Create return output vector
    std::vector<byte_t> output_file;

    // Put segment into output
    for (unsigned char i : buffer)
        output_file.push_back((byte_t)i);

    return output_file;
}

void write_file(const std::string file_name, const std::vector<std::string> output_vector)
{
    std::ofstream output_file(file_name);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(output_vector.begin(), output_vector.end(), output_iterator);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: chism <filename> {output_file_name}\n";
        return EXIT_SUCCESS;
    }

    std::string input_file_name(argv[1]);
    std::string output_file_name(input_file_name.substr(0, input_file_name.size() - 3) + "asm");

    if (argc > 3)
        output_file_name = argv[2];

    std::vector<byte_t> binary_data = read_file(input_file_name);
    std::vector<std::string> output_stream;

    // Iterate over binary data
    for (unsigned int i = 0; i < binary_data.size(); i += 2)
    {
        Instruction current_instruction(binary_data[i], binary_data[i + 1]);
        output_stream.push_back(translate_instruction(current_instruction));
    }

    write_file(output_file_name, output_stream);
    return EXIT_SUCCESS;
}