#include <iostream>
#include <sstream>

#include <parser.h>
#include <stack>

class RExpression {
 public:
  virtual std::string to_string() const = 0;
};

class RGenericExpression : public RExpression {
  std::string str;
 public:
  RGenericExpression(std::string str) {
    this->str = str;
  }
  std::string to_string() const {
    return str;
  }
};

class RInt : public RExpression {
  int n;
 public:
  RInt(int n) {
    this->n = n;
  }
  RInt(std::string s) : RInt(stoi(s)) {}
  std::string to_string() const {
    return std::to_string(n);
  }
};

class RId : public RGenericExpression {
 public:
  RId(std::string s) : RGenericExpression(s) {};
};

class RAssignment : public RExpression {
  RId * id;
  RExpression * expr;
 public:
  RAssignment(RId * id, RExpression * expr) {
    this->id   = id;
    this->expr = expr;
  }
  std::string to_string() const {
    return id->to_string() + "=" + expr->to_string();
  }
};


std::ostream& operator<<(std::ostream& s, const RExpression* e) {
  s << e->to_string();
  return s;
}


//class RFunction {
//  std::list<RExpression> expressions;
//
//  void add(RExpression e) {
//    expressions.push_back(e);
//  }
//};

using namespace axe;

template <class I>
void R_function(I begin, I end) {
//  auto ws = r_skip(axe::r_any(" \t\n"));

  std::stack<RExpression*> expstack;
  std::stack<RExpression*> formstack;

  auto function_lit = r_lit("function");

  auto _comma = r_lit(',');
  auto _sep   = r_lit(';');
  auto _nl    = r_lit('\n');
  auto _lbrac = r_lit('(');
  auto _rbrac = r_lit(')');
  auto _lbl   = r_lit('{');
  auto _rbl   = r_lit('}');

  auto int_m = r_numstr();
  auto int_e = int_m >> e_ref([&](I a, I b) {
      expstack.push(new RInt(std::string(a, b)));
  });

  auto _dt = r_lit('.');
  auto _us = r_lit('_');

  auto _idStart = r_alpha() || _us || _dt;
  auto _idCont  = r_alnum() || _us || _dt;
  auto _id      = _idStart & *_idCont;

  auto id_m = ((_dt & _id) | _id);
  auto id_e = id_m >> e_ref([&](I a, I b) {
    expstack.push(new RId(std::string(a, b)));
  });

  axe::r_rule<I> expr_e;
  auto exprlist_e = expr_e & *((_sep | _nl) & ~expr_e);
  expr_e =
    ((_lbl & exprlist_e & _rbl) |
     int_e |
     id_e);

  auto assign_m = r_lit('=') & expr_e;
  auto form_assign_e = assign_m >> e_ref([&](I a, I b) {
      RExpression * val = expstack.top(); expstack.pop();
      RId         * id  = static_cast<RId*>(expstack.top()); expstack.pop();
      formstack.push(new RAssignment(id, val));
  });

  auto form_id_or_assign_e = form_assign_e | (r_empty() >> e_ref([&](I a, I b) {
        formstack.push(expstack.top());
        expstack.pop();
  }));

  auto form_e =
    r_lit("...") |
    (id_e & form_id_or_assign_e);

  auto formlist_e = form_e % _comma;

  auto function_m = function_lit &
    _lbrac & ~formlist_e & _rbrac >
    expr_e;

  auto result = function_m(begin, end);

  while(!formstack.empty()) {
    std::cout << formstack.top() << std::endl;
    formstack.pop();
  }
  while(!expstack.empty()) {
    std::cout << expstack.top() << std::endl;
    expstack.pop();
  }
}

void parse(const char * input) {
  std::stringstream ss;
  ss << input;
  std::string str = ss.str();
  R_function(str.begin(), str.end());
}
