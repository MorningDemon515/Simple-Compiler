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
    TOKEN_STAR,// 乘号
    TOKEN_SLASH,// 除号
    TOKEN_LPAREN,//左括号
    TOKEN_RPAREN,//右括号
    TOKEN_EQUAL,//等号
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
        //跳过空白字符和文件结束
        while (pos < strlen(input) && std::isspace(input[pos])){
            ++pos;
        }

        if (pos >= strlen(input)) {
            return { TOKEN_EOF, "" };
        }

        //标识符和数字
        char currentChar = input[pos];

        if (std::isalpha(currentChar) || currentChar == '_')
        {
            return T_identifier();
        }

        if (std::isdigit(currentChar))
        {
            return T_number();
        }

        //操作符

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

    //识别标识符
    Token T_identifier()
    {
        size_t startPos = pos;
        while (pos < strlen(input) && (std::isalnum(input[pos]) || input[pos] == '_'))
        {
            ++pos;
        }
        return {TOKEN_IDENTIFIER, std::string(input + startPos, pos - startPos)};
    }

    //识别数字
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

    // 解析并计算表达式
    void parse()
    {
        while (currentToken.type != TOKEN_EOF) {
            statement();
        }
    }

    Lexer& lexer;
    Token currentToken;
    std::map<std::string, double> symbolTable; // 用于存储标识符和它们的值

    // 读取下一个token
    void eat(TokenType type)
    {
        if (currentToken.type == type) {
            currentToken = lexer.getNextToken();
        }
        else {
            throw std::runtime_error("Syntax error: unexpected token");
        }
    }

    // 解析一个语句（赋值或表达式）
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

    // 解析赋值语句：identifier = expression
    void assignment()
    {
        std::string varName = currentToken.value; // 获取标识符的名字
        eat(TOKEN_IDENTIFIER);  // 吃掉标识符

        eat(TOKEN_EQUAL);  // 期待 '=' 符号

        double result = expression(); // 解析右边的表达式

        symbolTable[varName] = result; // 存储结果到符号表中

        std::cout << "Assigned " << result << " to " << varName << std::endl;
    }

    // 解析表达式
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

    // 解析项
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

    // 解析因子
    double factor()
    {
        if (currentToken.type == TOKEN_NUMBER)
        {
            double result = std::stod(currentToken.value);
            eat(TOKEN_NUMBER);
            return result;
        }
        else if(currentToken.type == TOKEN_IDENTIFIER){
            // 如果是标识符，查找其值
            std::string varName = currentToken.value;
            eat(TOKEN_IDENTIFIER);
            if (symbolTable.find(varName) != symbolTable.end())
            {
                return symbolTable[varName]; // 返回该标识符的值
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
        // 打印一下符号表，查看变量的值
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