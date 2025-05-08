#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::string filename = "test_file.txt";
    std::string text = "Hello, C++ World!";
    std::string buffer;

    // Open file for writing
    std::ofstream outfile(filename);
    if (!outfile.is_open())
    {
        std::cerr << "Error opening file for writing" << std::endl;
        return 1;
    }
    outfile << text << std::endl;
    outfile.close();

    // Open file for reading
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
        std::cerr << "Error opening file for reading" << std::endl;
        return 1;
    }
    std::getline(infile, buffer);
    infile.close();

    std::cout << "Read from file: " << buffer << std::endl;
    return 0;
}
