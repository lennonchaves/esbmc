/*******************************************************************\

Module: Expression Representation

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <stdlib.h>
#include <assert.h>

#include "mp_arith.h"
#include "fixedbv.h"
#include "ieee_float.h"
#include "expr.h"

/*******************************************************************\

Function: exprt::move_to_operands

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::move_to_operands(exprt &expr)
{
  operandst &op=operands();
  op.push_back(static_cast<const exprt &>(get_nil_irep()));
  op.back().swap(expr);
}

/*******************************************************************\

Function: exprt::move_to_operands

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::move_to_operands(exprt &e1, exprt &e2)
{
  operandst &op=operands();
  #ifndef USE_LIST
  op.reserve(op.size()+2);
  #endif
  op.push_back(static_cast<const exprt &>(get_nil_irep()));
  op.back().swap(e1);
  op.push_back(static_cast<const exprt &>(get_nil_irep()));
  op.back().swap(e2);
}

/*******************************************************************\

Function: exprt::move_to_operands

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::move_to_operands(exprt &e1, exprt &e2, exprt &e3)
{
  operandst &op=operands();
  #ifndef USE_LIST
  op.reserve(op.size()+3);
  #endif
  op.push_back(static_cast<const exprt &>(get_nil_irep()));
  op.back().swap(e1);
  op.push_back(static_cast<const exprt &>(get_nil_irep()));
  op.back().swap(e2);
  op.push_back(static_cast<const exprt &>(get_nil_irep()));
  op.back().swap(e3);
}

/*******************************************************************\

Function: exprt::copy_to_operands

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::copy_to_operands(const exprt &expr)
{
  operands().push_back(expr);
}

/*******************************************************************\

Function: exprt::copy_to_operands

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::copy_to_operands(const exprt &e1, const exprt &e2)
{
  operandst &op=operands();
  #ifndef USE_LIST
  op.reserve(op.size()+2);
  #endif
  op.push_back(e1);
  op.push_back(e2);
}

/*******************************************************************\

Function: exprt::copy_to_operands

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::copy_to_operands(const exprt &e1, const exprt &e2,
                             const exprt &e3)
{
  operandst &op=operands();
  #ifndef USE_LIST
  op.reserve(op.size()+3);
  #endif
  op.push_back(e1);
  op.push_back(e2);
  op.push_back(e3);
}

/*******************************************************************\

Function: exprt::make_typecast

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::make_typecast(const typet &_type)
{
  exprt new_expr("typecast");

  new_expr.move_to_operands(*this);
  new_expr.set("type", _type);

  swap(new_expr);
}

/*******************************************************************\

Function: exprt::make_not

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::make_not()
{
  if(is_true())
  {
    make_false();
    return;
  }
  else if(is_false())
  {
    make_true();
    return;
  }

  exprt new_expr;

  if(id()=="not" && operands().size()==1)
  {
    new_expr.swap(operands().front());
  }
  else
  {
    new_expr=exprt("not", type());
    new_expr.move_to_operands(*this);
  }

  swap(new_expr);
}

/*******************************************************************\

Function: exprt::is_constant

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::is_constant() const
{
  return id()=="constant";
}

/*******************************************************************\

Function: exprt::is_true

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::is_true() const
{
  return is_constant() &&
         type().id()=="bool" &&
         get("value")!="false";
}

/*******************************************************************\

Function: exprt::is_false

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::is_false() const
{
  return is_constant() &&
         type().id()=="bool" &&
         get("value")=="false";
}

/*******************************************************************\

Function: exprt::make_bool

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::make_bool(bool value)
{
  *this=exprt("constant", typet("bool"));
  set("value", value?"true":"false");
}

/*******************************************************************\

Function: exprt::make_true

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::make_true()
{
  *this=exprt("constant", typet("bool"));
  set("value", "true");
}

/*******************************************************************\

Function: exprt::make_false

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::make_false()
{
  *this=exprt("constant", typet("bool"));
  set("value", "false");
}

/*******************************************************************\

Function: operator<

  Inputs:

 Outputs:

 Purpose: defines ordering on expressions for canonicalization

\*******************************************************************/

bool operator<(const exprt &X, const exprt &Y)
{
  return (irept &)X < (irept &)Y;
}

/*******************************************************************\

Function: exprt::negate

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void exprt::negate()
{
  const irep_idt &type_id=type().id();

  if(type_id=="bool")
    make_not();
  else if(type_id=="integer")
  {
    if(is_constant())
      set("value", integer2string(-string2integer(get_string("value"))));
    else if(id()=="unary-")
    {
      exprt tmp;
      assert(operands().size()==1);
      tmp.swap(op0());
      swap(tmp);
    }
    else
    {
      exprt tmp("unary-", type());
      tmp.move_to_operands(*this);
      swap(tmp);
    }
  }
  else
    make_nil();
}

/*******************************************************************\

Function: exprt::is_boolean

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::is_boolean() const
{
  return type().id()=="bool";
}

/*******************************************************************\

Function: exprt::is_zero

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::is_zero() const
{
  if(is_constant())
  {
    const std::string &value=get_string("value");
    const irep_idt &type_id=type().id_string();

    if(type_id=="integer" || type_id=="natural")
    {
      mp_integer int_value=string2integer(value);
      if(int_value==0) return true;
    }
    else if(type_id=="unsignedbv" || type_id=="signedbv")
    {
      mp_integer int_value=binary2integer(value, false);
      if(int_value==0) return true;
    }
    else if(type_id=="fixedbv")
    {
      if(fixedbvt(*this)==0) return true;
    }
    else if(type_id=="floatbv")
    {
      if(ieee_floatt(*this)==0) return true;
    }
    else if(type_id=="pointer")
    {
      if(value=="NULL") return true;
    }
  }

  return false;
}

/*******************************************************************\

Function: exprt::is_one

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::is_one() const
{
  if(is_constant())
  {
    const std::string &value=get_string("value");
    const irep_idt &type_id=type().id_string();

    if(type_id=="integer" || type_id=="natural")
    {
      mp_integer int_value=string2integer(value);
      if(int_value==1) return true;
    }
    else if(type_id=="unsignedbv" || type_id=="signedbv")
    {
      mp_integer int_value=binary2integer(value, false);
      if(int_value==1) return true;
    }
    else if(type_id=="fixedbv")
    {
      if(fixedbvt(*this)==1)
        return true;
    }
    else if(type_id=="floatbv")
    {
      if(ieee_floatt(*this)==1)
        return true;
    }
  }

  return false;
}

/*******************************************************************\

Function: exprt::sum

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::sum(const exprt &expr)
{
  if(!is_constant() || !expr.is_constant()) return true;
  if(type()!=expr.type()) return true;

  const irep_idt &type_id=type().id();

  if(type_id=="integer" || type_id=="natural")
  {
    set("value", integer2string(
      string2integer(get_string("value"))+
      string2integer(expr.get_string("value"))));
    return false;
  }
  else if(type_id=="unsignedbv" || type_id=="signedbv")
  {
    set("value", integer2binary(
      binary2integer(get_string("value"), false)+
      binary2integer(expr.get_string("value"), false),
      atoi(type().get("width").c_str())));
    return false;
  }
  else if(type_id=="fixedbv")
  {
    set("value", integer2binary(
      binary2integer(get_string("value"), false)+
      binary2integer(expr.get_string("value"), false),
      atoi(type().get("width").c_str())));
    return false;
  }
  else if(type_id=="floatbv")
  {
    ieee_floatt f(*this);
    f+=ieee_floatt(expr);
    *this=f.to_expr();
    return false;
  }

  return true;
}

/*******************************************************************\

Function: exprt::mul

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::mul(const exprt &expr)
{
  if(!is_constant() || !expr.is_constant()) return true;
  if(type()!=expr.type()) return true;

  const irep_idt &type_id=type().id();

  if(type_id=="integer" || type_id=="natural")
  {
    set("value", integer2string(
      string2integer(get_string("value"))*
      string2integer(expr.get_string("value"))));
    return false;
  }
  else if(type_id=="unsignedbv" || type_id=="signedbv")
  {
    set("value", integer2binary(
      binary2integer(get_string("value"), false)*
      binary2integer(expr.get_string("value"), false),
      atoi(type().get("width").c_str())));
    return false;
  }
  else if(type_id=="fixedbv")
  {
    fixedbvt f(*this);
    f*=fixedbvt(expr);
    *this=f.to_expr();
    return false;
  }
  else if(type_id=="floatbv")
  {
    ieee_floatt f(*this);
    f*=ieee_floatt(expr);
    *this=f.to_expr();
    return false;
  }

  return true;
}

/*******************************************************************\

Function: exprt::subtract

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool exprt::subtract(const exprt &expr)
{
  if(!is_constant() || !expr.is_constant()) return true;

  if(type()!=expr.type()) return true;

  const irep_idt &type_id=type().id();

  if(type_id=="integer" || type_id=="natural")
  {
    set("value", integer2string(
      string2integer(get_string("value"))-
      string2integer(expr.get_string("value"))));
    return false;
  }
  else if(type_id=="unsignedbv" || type_id=="signedbv")
  {
    set("value", integer2binary(
      binary2integer(get_string("value"), false)-
      binary2integer(expr.get_string("value"), false),
      atoi(type().get("width").c_str())));
    return false;
  } else if(type_id=="fixedbv") {
    set("value", integer2binary(
      binary2integer(get_string("value"), false)-
      binary2integer(expr.get_string("value"), false),
      atoi(type().get("width").c_str())));
    return false;
  }

  return true;
}

/*******************************************************************\

Function: exprt::find_location

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

const locationt &exprt::find_location() const
{
  const locationt &l=location();

  if(l.is_not_nil()) return l;

  forall_operands(it, (*this))
  {
    const locationt &l=it->find_location();
    if(l.is_not_nil()) return l;
  }

  return static_cast<const locationt &>(get_nil_irep());
}
