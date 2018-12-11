/* a modern c++ calcultor using c++11
 * refer https://zhuanlan.zhihu.com/p/23139200
 */
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <memory>
#include "dxh/testult.h"
using namespace std;

typedef int NumType;
/* AST define
* num = [0-9][.][0-9]f
* term = num|"("expr")"
* factor = term|factor "*" term|factor "/" term;
* expr = factor|expr "+" factor|expr "-" factor
*/

class Expression;
static shared_ptr<Expression> match_term(const char *&stream);
static shared_ptr<Expression> match_factor(const char *&stream);
static shared_ptr<Expression> match_expr(const char *&stream);

// test if stream started with text. skip whitespace.
bool match(const char *&stream, const char *text){
    size_t len = strlen(text);
    const char *start = stream;
    while(*start == ' ')    {++start;}
    if (strncmp(start, text, len) == 0){
        stream = start + len;
        return true;
    }
    return false;
}


class Exception{
public:
    const char *_pos;
    const char *_err;
    Exception(const char *pos, const char *error):_pos(pos), _err(error){
        // d
    }
};

class Expression{
public:
    bool _isLeft; // left expr
    Expression():_isLeft(false){}
    virtual ~Expression() = default;
    virtual double eval() = 0;
};

enum class BinaryOperator
{
	Plus, // +
	Minus, // -
	Multiply, //*
	Divide, // /
	Exp, // ^
};

class NumbericExpr :public Expression{
public:
    NumType _value;
    NumbericExpr(NumType v):_value(v){}
    virtual double eval()override{
        return _value;
    }

};

class BinaryExpr : public Expression{
public:
     shared_ptr<Expression> _fst;
     shared_ptr<Expression> _snd;
     BinaryOperator Op;
     BinaryExpr(BinaryOperator theOp,
          shared_ptr<Expression> theLeft, shared_ptr<Expression> theRight)
           : Op(theOp), _fst(theLeft), _snd(theRight)
   {
   }
   virtual double eval()override{
        double fst = _fst->eval();
        double snd = _snd->eval();
        switch (Op){
            case BinaryOperator::Plus:
                return fst + snd;
            case BinaryOperator::Minus:
                return fst - snd;
            case BinaryOperator::Multiply:
                return fst * snd;
            case BinaryOperator::Divide:
                return fst / snd;
            case BinaryOperator::Exp:
                return pow(fst, snd);
        }
        return 0;
    }
};


static shared_ptr<NumbericExpr> match_number(const char *&stream)
{
    NumType result = 0;
    bool bGot = false;
    const char *pos = stream;
    while (*pos == ' ')++pos;
    while (1){
        if (*pos >='0' && *pos <= '9'){
            bGot = true;
            result = result * 10 + *pos - '0';
            ++pos;
        }
        else
            break;
    }
    if (bGot){
        // make expr
        stream = pos;
        return make_shared<NumbericExpr>(result);
    }
    else
        throw Exception(stream, "here need a number.");
}

static shared_ptr<Expression> match_term(const char *&stream){
    // term = num|"("expr")"
    try{
        return match_number(stream);
    }
    catch (Exception &e){
        // match expression
        const char *read = stream;
        if (match(read, "(")){
            auto exp = match_expr(read);
            if (match(read, ")")){
                // a valid () pair
                stream = read;
                return exp;
            }
            else{
               throw Exception(stream, "hear need  ')'.");
            }
        }
        else{
            throw e;
        }
    }
}


static shared_ptr<Expression> match_factor(const char *&stream){
    // factor = factor "*" term | factor "/" term;
    const char *read = stream;
    auto result = match_term(read);
    while (true){
        BinaryOperator op;
        if (match(read, "*")){
            op = BinaryOperator::Multiply;
        }
        else if (match(read, "/")){
            op = BinaryOperator::Divide;
        }
        else if (match(read, "^")){
            op = BinaryOperator::Exp;
        }
        else
            break;
        result = make_shared<BinaryExpr>(op, result, match_term(read));
    }
    stream = read;
    return result;
}


static shared_ptr<Expression> match_expr(const char *&stream){
    // expr = expr "+" factor | expr "-" factor
    const char *read = stream;
    auto result = match_factor(read);
    while (true){
        BinaryOperator op;
        if (match(read, "+")){
            op = BinaryOperator::Plus;
        }
        else if (match(read, "-")){
            op = BinaryOperator::Minus;
        }
        else
            break;
        result = make_shared<BinaryExpr>(op, result, match_factor(read));
    }
    stream = read;
    return result;
}

int main()
{

    std::string s;
    do {
        PrintWithColor(Aqua, Black, "input a expression:");
        cin >> s;
        try{
            const char *p = &s[0];
            auto expr = match_expr(p);
            PrintWithColor(Green, Black, "%f\n", expr->eval());
        }catch (Exception &e){
            cout << e._pos <<":"<<e._err<< endl;
            continue;
        }
    }while (!s.empty());
   return 0;
}
