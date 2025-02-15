#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <map>

enum TokenType{
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,// �˺�
    TOKEN_SLASH,// ����
    TOKEN_LPAREN,//������
    TOKEN_RPAREN,//������
    TOKEN_EQUAL,//�Ⱥ�
    TOKEN_EOF,
    TOKEN_INVALID
};

struct Token
{
    TokenType type;
    std::string value;
};

class Lexer
{
public:
    Lexer(const char* input) : input(input), pos(0) {}

    Token getNextToken()
    {
        //�����հ��ַ����ļ�����
        while (pos < strlen(input) && std::isspace(input[pos])){
            ++pos;
        }

        if (pos >= strlen(input)) {
            return { TOKEN_EOF, "" };
        }

        //��ʶ��������
        char currentChar = input[pos];

        if (std::isalpha(currentChar) || currentChar == '_')
        {
            return T_identifier();
        }

        if (std::isdigit(currentChar))
        {
            return T_number();
        }

        //������

        if (currentChar == '+')
        {
            ++pos;
            return {TOKEN_PLUS, "+"};
        }
        
        if (currentChar == '-')
        {
            ++pos;
            return { TOKEN_MINUS, "-" };
        }

        if (currentChar == '*')
        {
            ++pos;
            return { TOKEN_STAR, "*" };
        }

        if (currentChar == '/')
        {
            ++pos;
            return { TOKEN_SLASH, "/" };
        }

        if (currentChar == '(')
        {
            ++pos;
            return { TOKEN_LPAREN, "(" };
        }

        if (currentChar == ')')
        {
            ++pos;
            return { TOKEN_RPAREN, ")" };
        }

        if (currentChar == '=')
        {
            ++pos;
            return { TOKEN_EQUAL, "=" };
        }

        return {TOKEN_INVALID, ""};
    }

private:
    const char* input;
    size_t pos;

    //ʶ���ʶ��
    Token T_identifier()
    {
        size_t startPos = pos;
        while (pos < strlen(input) && (std::isalnum(input[pos]) || input[pos] == '_'))
        {
            ++pos;
        }
        return {TOKEN_IDENTIFIER, std::string(input + startPos, pos - startPos)};
    }

    //ʶ������
    Token T_number()
    {
        size_t startPos = pos;
        while (pos < strlen(input) && std::isdigit(input[pos]))
        {
            ++pos;
        }
        return {TOKEN_NUMBER, std::string(input + startPos,pos-startPos)};
    }
};

class Parser
{
public:
    Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.getNextToken()) {}

    // ������������ʽ
    void parse()
    {
        while (currentToken.type != TOKEN_EOF) {
            statement();
        }
    }

    Lexer& lexer;
    Token currentToken;
    std::map<std::string, double> symbolTable; // ���ڴ洢��ʶ�������ǵ�ֵ

    // ��ȡ��һ��token
    void eat(TokenType type)
    {
        if (currentToken.type == type) {
            currentToken = lexer.getNextToken();
        }
        else {
            throw std::runtime_error("Syntax error: unexpected token");
        }
    }

    // ����һ����䣨��ֵ����ʽ��
    void statement()
    {
        if (currentToken.type == TOKEN_IDENTIFIER)
        {
            assignment();
        }
        else
        {
            double result = expression();
            std::cout << "Result: " << result << std::endl;
        }
    }

    // ������ֵ��䣺identifier = expression
    void assignment()
    {
        std::string varName = currentToken.value; // ��ȡ��ʶ��������
        eat(TOKEN_IDENTIFIER);  // �Ե���ʶ��

        eat(TOKEN_EQUAL);  // �ڴ� '=' ����

        double result = expression(); // �����ұߵı��ʽ

        symbolTable[varName] = result; // �洢��������ű���

        std::cout << "Assigned " << result << " to " << varName << std::endl;
    }

    // �������ʽ
    double expression()
    {
        double result = term();

        while (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS)
        {
            if (currentToken.type == TOKEN_PLUS) {
                eat(TOKEN_PLUS);
                result += term();
            }
            else if (currentToken.type == TOKEN_MINUS) {
                eat(TOKEN_MINUS);
                result -= term();
            }
        }

        return result;
    }

    // ������
    double term()
    {
        double result = factor();

        while (currentToken.type == TOKEN_STAR || currentToken.type == TOKEN_SLASH)
        {
            if (currentToken.type == TOKEN_STAR) {
                eat(TOKEN_STAR);
                result *= factor();
            }
            else if (currentToken.type == TOKEN_SLASH) {
                eat(TOKEN_SLASH);
                result /= factor();
            }
        }

        return result;
    }

    // ��������
    double factor()
    {
        if (currentToken.type == TOKEN_NUMBER)
        {
            double result = std::stod(currentToken.value);
            eat(TOKEN_NUMBER);
            return result;
        }
        else if(currentToken.type == TOKEN_IDENTIFIER){
            // ����Ǳ�ʶ����������ֵ
            std::string varName = currentToken.value;
            eat(TOKEN_IDENTIFIER);
            if (symbolTable.find(varName) != symbolTable.end())
            {
                return symbolTable[varName]; // ���ظñ�ʶ����ֵ
            }
            else
            {
                throw std::runtime_error("Undefined variable: " + varName);
            }
        }
        else if (currentToken.type == TOKEN_LPAREN)
        {
            eat(TOKEN_LPAREN);
            double result = expression();
            eat(TOKEN_RPAREN);
            return result;
        }
        else
        {
            throw std::runtime_error("Syntax error: expected number or '('");
        }
    }
};

int main(int argc, char* args[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << args[0] << " <input file>" << std::endl;
        return 1;
    }

    std::string infile = args[1];
    std::ifstream vssfile(infile);
    if (!vssfile) {
        std::cerr << "Could not open file: " << infile << std::endl;
        return 1;
    }

    std::stringstream vssstream;
    vssstream << vssfile.rdbuf();
    std::string vsscode = vssstream.str();

    const char* input = vsscode.c_str();
    Lexer lexer(input);
   // Token token = lexer.getNextToken();
    Parser parser(lexer);

   // while (token.type != TOKEN_EOF)
    //{
      //  std::cout << "Token: " << token.type << " Value: " << token.value << std::endl;
        //token = lexer.getNextToken();
    //}

    try {
        parser.parse();
        // ��ӡһ�·��ű��鿴������ֵ
        //std::cout << "Symbol Table:" << std::endl;
        //for (const auto& entry : parser.symbolTable) {
          //  std::cout << entry.first << " = " << entry.second << std::endl;
        //}

        std::string map_key = args[2];
        auto it = parser.symbolTable.find(map_key);
        if (it != parser.symbolTable.end()) {
            std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
        }
        else {
            std::cout << "Key not found." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    system("pause");
    return 0;
}