#ifndef _READER_HPP_
#define _READER_HPP_

#include <vector>
#include <string>

class file
{
public:
    file(const char * file_path);
    void read();
    void print();

    std::string get_file_descriptor() { return static_cast <std::string> (m_fd); }

    std::vector<std::string> source;
private:
    const char * m_fd;
};
#endif // !_READER_HPP_