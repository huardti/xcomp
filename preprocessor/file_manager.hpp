#ifndef _READER_HPP_
#define _READER_HPP_

#include <vector>
#include <string>

class File
{
public:
    File();
    File(const char * file_path);
    File(const File &) = default;
    File &operator=(File &&) = default;
    File &operator=(const File &) = default;

    bool read();
    bool write();
    void print();

    std::string get_file_descriptor() { return static_cast <std::string> (m_fd); }

    std::string source;
private:
    const char * m_fd;
};
#endif // !_READER_HPP_