#include "preprocessor.hpp"

Preprocessor::Preprocessor(File source, File &out): m_in(source), m_out(out), m_state(Preprocessor::State::Base) {
    m_line = 1;
    m_file_name = source.get_file_descriptor();
}
Preprocessor::Preprocessor(File source): m_in(source), m_state(Preprocessor::State::Base) {
    m_line = 1;
    m_out.source.clear();
    m_file_name = source.get_file_descriptor();
}


void Preprocessor::run(){
    Lexer lex(m_in.source);
    std::string s;
    m_out.source.append("# " + std::to_string(m_line) + " \"" + m_file_name + "\"\n");
    for (Token token = lex.next(); not token.is_one_of(Token::Type::End, Token::Type::Unexpected); token = lex.next()) {
        /*
        m_out.source.append(out(token));
        State next = next_state(token);
        m_state = next;
        */
        if (token.type() == Token::Type::Newline) {  // always output newline to conserve line number of the file
            m_line++;
            m_out.source.append("\n");
        }
        /* state machine to find / or # */
        switch (m_state) {
        case State::Base:
            if (token.type() == Token::Type::Slash) {
                m_state = State::Slash;
            }
            else if (token.type() == Token::Type::Hash) {
                m_state = State::hash;
            }
            else {
                if (token.type() != Token::Type::Newline) {
                    m_out.source.append(token.lex());
                }
            }
            break;
        case State::Slash:
            if (token.type() == Token::Type::Slash) {
                m_state = State::Line_comment;
            }
            else if (token.type() == Token::Type::Asterisk) {
                m_state = State::C_comment_start;            
            }
            else {
                m_state = State::Base;
                m_out.source.append("/");
                m_out.source.append(token.lex());
            }
            break;
        case State::Line_comment:
            if (token.type() == Token::Type::Newline ||token.type() == Token::Type::End) {
                m_state = State::Base;
            }
            break;
        case State::C_comment_start:
        /*
            if (token.type() == Token::Type::Newline) {
                m_out.append("\n");
            }
        */
            if (token.type() == Token::Type::Asterisk) {
                m_state = State::C_comment_find_asterisk;
            }
            break;
        case State::C_comment_find_asterisk:
            if (token.type() == Token::Type::Slash) {
                m_state = State::Base;
            }
            else {
                m_state = State::C_comment_start;
            }
            break;
        default:
            m_state = State::Base;
            break;
        }
    }
    std::cout << std::endl << "result"  << std::endl << m_out.source << std::endl;
}
#if 0
Preprocessor::State Preprocessor::next_state(Token token) {
        Token::Type T = token.type();
        switch (m_state) {

        case State::Base:
            if (T == Token::Type::Slash)    { return State::Slash;  }
            if (T == Token::Type::Hash)     { return State::hash;   }
            return State::Base;

        case State::Slash:
            if (T == Token::Type::Slash)    { return State::Line_comment;       }
            if (T == Token::Type::Asterisk) { return State::C_comment_start;    }
            return State::Base;

        case State::Line_comment:
            if (T == Token::Type::Newline || T == Token::Type::End)      { return State::Base; }
            return State::Line_comment;

        case State::C_comment_start:
            if (T == Token::Type::Asterisk)  { return State::C_comment_find_asterisk; }
            return State::C_comment_start;

        case State::C_comment_find_asterisk:
            if (T == Token::Type::Slash)    { return State::Base; }
            return State::C_comment_start;

        default:
            std::cout << "preprocessor: unimplemented state" << std::endl;

            token.lex()
            
    }
}

std::string Preprocessor::out(Token token){
if (token.type() == Token::Type::Newline) {  // always output newline to conserve line number of the file
            m_line++;
            return "\n";
        }
        /* state machine to find / or # */
        switch (m_state) {
        case State::Base:
                if (token.type() != Token::Type::Newline) {
                    return token.lex();
                }

        case State::Slash:
            if (token.type() == Token::Type::Slash) {
                state = State::Line_comment;
            }
            else if (token.type() == Token::Type::Asterisk) {
                state = State::C_comment_start;            
            }
            else {
                state = State::Base;
                m_out.append("/");
                m_out.append(token.lex());
            }
            break;
        case State::Line_comment:
            if (token.type() == Token::Type::Newline ||token.type() == Token::Type::End) {
                state = State::Base;
            }
            break;
        case State::C_comment_start:
        /*
            if (token.type() == Token::Type::Newline) {
                m_out.append("\n");
            }
        */
            if (token.type() == Token::Type::Asterisk) {
                state = State::C_comment_find_asterisk;
            }
            break;
        case State::C_comment_find_asterisk:
            if (token.type() == Token::Type::Slash) {
                state = State::Base;
            }
            else {
                state = State::C_comment_start;
            }
            break;
        default:
            state = State::Base;
            break;
        }
}

#endif