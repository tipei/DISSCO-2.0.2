#include "ModParser.h"

ModParser::Token::Token(int n, int minVal, int maxVal, int offset)
                       : n(n)
                       , minVal(minVal)
                       , maxVal(maxVal)
                       , offset(offset)
                       , hasList(false) {}

ModParser::Token::Token(std::list<int> l)
                       : l(l)
                       , hasList(true) {}

//----------------------------------------------------------------------------//

const std::list<int>& ModParser::Token::getList() {
  if (hasList) return l;
  hasList = true;
  l = modList(n, minVal, maxVal, offset);

  return l;
}


//----------------------------------------------------------------------------//

int ModParser::Token::getInt() {
  return n;
}


//----------------------------------------------------------------------------//

std::list<int> ModParser::modList(int mod, int min, int max, int offset) {
  std::list<int> result;
  int startNum = min + offset + (mod - (min % mod)) % mod;
  startNum -= mod * ((startNum - min)/mod);
       
  for (int i = startNum; i <= max; i += mod) {
    result.push_back(i);
  }

  return result;
}


//----------------------------------------------------------------------------//

std::list<int> ModParser::offsetList(const std::list<int>& l, int offset) {
  std::list<int> result;
  for (std::list<int>::const_iterator it = l.begin(); it != l.end(); ++it) {
    result.push_back(*it + offset);
  }
  return result;
}


//----------------------------------------------------------------------------//

std::list<int> ModParser::listUnion(const std::list<int>& a, const std::list<int>& b) {
  std::list<int> result(a.size() + b.size());
  std::list<int>::iterator it = set_union(a.begin(), a.end(), b.begin(), b.end(), result.begin());
  result.erase(it, result.end());
  return result;
}


//----------------------------------------------------------------------------//

std::list<int> ModParser::listIntersection(const std::list<int>& a, const std::list<int>& b) {
  std::list<int> result(std::min(a.size(), b.size()));
  std::list<int>::iterator it = set_intersection(a.begin(), a.end(), b.begin(), b.end(), result.begin());
  result.erase(it, result.end());
  return result;
}


//----------------------------------------------------------------------------//

std::list<int> ModParser::listDifference(const std::list<int>& a, const std::list<int>& b) {
  std::list<int> result(a.size());
  std::list<int>::iterator it = set_difference(a.begin(), a.end(), b.begin(), b.end(), result.begin());
  result.erase(it, result.end());
  return result;
}


//----------------------------------------------------------------------------//

std::list<int> ModParser::listComplement(const std::list<int>& l, int minVal, int maxVal) {
  std::list<int> result;
  int elem = minVal;
  for (std::list<int>::const_iterator it = l.begin(); it != l.end() && elem <= *it && elem <= maxVal; ++it) {
    while (elem < *it && elem <= maxVal) {
      result.push_back(elem);
      ++elem;
    }
    ++elem;
  }
  return result;
}


//----------------------------------------------------------------------------//

void ModParser::parseOperator(std::vector<ModParser::Token>& operands, std::stack<char>& operators, int minVal, int maxVal) {
  char op = operators.top();
  operators.pop();
  std::list<int> result;

  if (op == '~') {
    result = listComplement(operands.back().getList(), minVal, maxVal);
    operands.pop_back();
    operands.push_back(result);
    return;
  }

  Token& b = operands.back();
  Token& a = operands[operands.size() - 2];

  switch (op) {
    case 'U':
      result = listUnion(a.getList(), b.getList());
      break;
    case 'I':
      result = listIntersection(a.getList(), b.getList());
      break;
    case '-':
      result = listDifference(a.getList(), b.getList());
      break;
  }

  operands.pop_back();
  operands.pop_back();
  operands.push_back(result);
}


//----------------------------------------------------------------------------//

int ModParser::precedence(char c) {
  switch (c) {
    case 'U': case 'I': return 1;
    case '~': return 99;
    default: return -1;
  }
}


//----------------------------------------------------------------------------//

void ModParser::parseExpr(const std::string& exp, int minVal, int maxVal) {
  std::vector<ModParser::Token> operands;
  std::stack<char> operators;
  int chNum = 0;
  int modIndex = 0;
  while (chNum < exp.size()) {
    char ch = exp[chNum];
    if (isdigit(ch)) {
      int num = 0;
      while (isdigit(ch) && chNum < exp.size()) {
        num *= 10;
        num += ch - '0';
        ++chNum;
        ch = exp[chNum];
      }
      operands.push_back(Token(num, minVal, maxVal, _offsets[modIndex]));
      _mods.push_back(num);
      ++modIndex;
    } else if (precedence(ch) != -1) {
      while (!operators.empty()
             && precedence(operators.top()) != -1
             && precedence(operators.top()) >= precedence(ch)) {
        parseOperator(operands, operators, minVal, maxVal);
      }
      operators.push(ch);
      ++chNum;
    } else if (ch == '(') {
      operators.push('(');
      ++chNum;
    } else if (ch == ')') {
      while (operators.top() != '(') {
        parseOperator(operands, operators, minVal, maxVal);
      }
      operators.pop();
      ++chNum;
    } else {
      ++chNum;
    }
  }
  while (!operators.empty()) {
    parseOperator(operands, operators, minVal, maxVal);
  }
  _elements = operands.back().getList();
}


//----------------------------------------------------------------------------//

ModParser::ModParser(std::vector<int> offsets) : _offsets(offsets) {
}
