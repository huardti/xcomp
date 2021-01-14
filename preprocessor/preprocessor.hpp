#ifndef _PREPROCESSOR_HPP_
#define _PREPROCESSOR_HPP_

#include "lexer.hpp"
#include "file_manager.hpp"

#include <map>

class defined
{
public:
    defined();
    ~defined();

    static void define();
    static  void undef();

protected:
    static std::map<std::string, std::string> def_tab;
    
};
class Preprocessor
{
public:
    enum class State {
        Base,
        Slash,
            Line_comment,
            C_comment_start,
            C_comment_find_asterisk,
        hash,
            Define,
            If,
            Line,



    };

    Preprocessor(File source);
    Preprocessor(File source, File &out);

    void run();

    void put_file_info();

protected:
    File m_in;
    File m_out;
    State m_state;

    size_t m_line;
    size_t m_physical_line;
    std::string m_file_name;

private:
    State next_state(Token t);
    std::string out(Token token);



};
#endif // !_PREPROCESSOR_HPP_ 
