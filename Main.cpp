#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

enum TokenType {
    TOK_EOF,//������
    TOK_NUMBER,//����
    TOK_PLUS,//�Ӻ�
    TOK_MINUS,//����
    TOK_MUL,//�˺�
    TOK_DIV,//����
    TOK_LPAREN,//������
    TOK_RPAREN//������
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t = TOK_EOF, std::string v = "")
        : type(t), value(v) {
    }
};

class Tokenizer {
    std::string input;
    size_t position;
public:
    Tokenizer(const std::string& str) : input(str), position(0) {}

    Token nextToken() {
        while (position < input.size() && isspace(input[position])) {
            ++position;
        }//�����հ��ַ�

        if (position >= input.size()) return Token(TOK_EOF);//�Ѿ������ַ���ĩβ

        char c = input[position++];
        if (isdigit(c)) {
            std::string num(1, c);
            while (position < input.size() && isdigit(input[position])) {
                num += input[position++];
            }//��ȡ����
            return Token(TOK_NUMBER, num);
        }//����

        switch (c) {
        case '+': return Token(TOK_PLUS);
        case '-': return Token(TOK_MINUS);
        case '*': return Token(TOK_MUL);
        case '/': return Token(TOK_DIV);
        case '(': return Token(TOK_LPAREN);
        case ')': return Token(TOK_RPAREN);
        default: throw std::runtime_error("Unexpected character");
        }
    }
};

class Parser {
    Tokenizer& tokenizer;
    Token currentToken;

    void eat(TokenType type) {
        if (currentToken.type == type) {
            currentToken = tokenizer.nextToken();
        }
        else {
            throw std::runtime_error("Syntax error");
        }
    }//��鵱ǰtoken�Ƿ�Ϊָ�����ͣ������ȡ��һ��token�������׳��쳣

public:
    Parser(Tokenizer& t) : tokenizer(t) {
        currentToken = tokenizer.nextToken();
    }

    // �������ʽ������Ӽ���
    double parseExpr() {
        double result = parseTerm();

        while (currentToken.type == TOK_PLUS || currentToken.type == TOK_MINUS) {
            Token op = currentToken;
            eat(currentToken.type);

            double right = parseTerm();
            if (op.type == TOK_PLUS) {
                result += right;
            }
            else {
                result -= right;
            }
        }
        return result;
    }

    // ���������˳���
    double parseTerm() {
        double result = parseFactor();

        while (currentToken.type == TOK_MUL || currentToken.type == TOK_DIV) {
            Token op = currentToken;
            eat(currentToken.type);

            double right = parseFactor();
            if (op.type == TOK_MUL) {
                result *= right;
            }
            else {
                if (right == 0) throw std::runtime_error("Division by zero");
                result /= right;
            }
        }
        return result;
    }

    // �������ӣ����ֻ����ű��ʽ��
    double parseFactor() {
        Token token = currentToken;

        if (token.type == TOK_NUMBER) {
            eat(TOK_NUMBER);
            return std::stod(token.value);
        }
        else if (token.type == TOK_LPAREN) {
            eat(TOK_LPAREN);
            double result = parseExpr();
            eat(TOK_RPAREN);
            return result;
        }
        else {
            throw std::runtime_error("Unexpected token");
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
    try {
        Tokenizer tokenizer(input);
        Parser parser(tokenizer);
        double result = parser.parseExpr();
        std::cout << "Result: " << result << std::endl;  
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    system("pause");
    return 0;
}