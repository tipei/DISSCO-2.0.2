#ifndef MODPARSER_H
#define MODPARSER_H

#include <map>

#include "Define.h"
#include "Libraries.h"

class ModParser {
private:
    std::vector<int> _mods;
    std::vector<int> _offsets;
    std::list<int> _elements;

    // Helper class used as a placeholder for a number or a list while parsing.
    // Facilitates implementation of polymorphic operators (for example, a list
    // offset by an integer, if such functionality is ever needed.
    class Token {
    private:
        int n;
        int minVal;
        int maxVal;
        int offset;
        std::list<int> l;
        bool hasList;

    public:
        Token(int n, int minVal, int maxVal, int offset = 0);
        Token(std::list<int> l);
        const std::list<int>& getList();
        int getInt();
        int getIndex();
    };

    // Offsets every element on a list by a number
    static std::list<int> offsetList(const std::list<int>& l, int offset);

    // Produces a list of elements that are divisible by input mod.
    // Range is [minVal, maxVal]
    static std::list<int> modList(int mod, int minVal, int maxVal, int offset = 0);

    // Self explanatory functions that produce a new list by combining two
    static std::list<int> listUnion(const std::list<int>& a, const std::list<int>& b);
    static std::list<int> listIntersection(const std::list<int>& a, const std::list<int>& b);
    static std::list<int> listDifference(const std::list<int>& a, const std::list<int>& b);
    static std::list<int> listComplement(const std::list<int>& l, int minVal, int maxVal);

    // Hardcoded precedence of all operator characters allowed in expressions
    static int precedence(char c);

public:
    ModParser(std::vector<int> offsets);

    // Helper method for parseExpr
    void parseOperator(std::vector<ModParser::Token>& operands, std::stack<char>& operators,
                       int minVal, int maxVal);
    // Parses an expression and stores the result for future access
    void parseExpr(const std::string& exp, int minVal, int maxVal);

    // Accessors
    std::vector<int> getOffsets() { return _offsets; };
    std::list<int> getElements() { return _elements; };
    std::vector<int> getMods() { return _mods; };
};

#endif
