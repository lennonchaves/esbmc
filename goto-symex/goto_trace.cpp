/*******************************************************************\

Module: Traces of GOTO Programs

Author: Daniel Kroening

  Date: July 2005

\*******************************************************************/

#include <assert.h>
#include <string.h>

#include <ansi-c/printf_formatter.h>
#include <langapi/language_util.h>
#include <arith_tools.h>


#include "goto_trace.h"
#include "VarMap.h"
#include <std_types.h>

void goto_tracet::output(
  const class namespacet &ns,
  std::ostream &out) const
{
  for(stepst::const_iterator it=steps.begin();
      it!=steps.end();
      it++)
    it->output(ns, out);
}

void goto_trace_stept::output(
  const namespacet &ns,
  std::ostream &out) const
{
  out << "*** ";

  switch(type)
  {
  case goto_trace_stept::ASSERT: out << "ASSERT"; break;
  case goto_trace_stept::ASSUME: out << "ASSUME"; break;
  case goto_trace_stept::ASSIGNMENT: out << "ASSIGNMENT"; break;
  default: assert(false);
  }

  if(type==ASSERT || type==ASSUME)
    out << " (" << guard << ")";

  out << std::endl;

  if(!pc->location.is_nil())
    out << pc->location << std::endl;

  if(pc->is_goto())
    out << "GOTO   ";
  else if(pc->is_assume())
    out << "ASSUME ";
  else if(pc->is_assert())
    out << "ASSERT ";
  else if(pc->is_other())
    out << "OTHER  ";
  else if(pc->is_assign())
    out << "ASSIGN ";
  else if(pc->is_function_call())
    out << "CALL   ";
  else
    out << "(?)    ";

  out << std::endl;

  if(pc->is_other() || pc->is_assign())
  {
    irep_idt identifier;

    if (!is_nil_expr(original_lhs))
      identifier = to_symbol2t(original_lhs).name;
    else
      identifier = to_symbol2t(lhs).name;

    out << "  " << identifier
        << " = " << from_expr(ns, identifier, value)
        << std::endl;
  }
  else if(pc->is_assert())
  {
    if(!guard)
    {
      out << "Violated property:" << std::endl;
      if(pc->location.is_nil())
        out << "  " << pc->location << std::endl;

      if(comment!="")
        out << "  " << comment << std::endl;
      out << "  " << from_expr(ns, "", pc->guard) << std::endl;
      out << std::endl;
    }
  }

  out << std::endl;
}

void counterexample_value(
  std::ostream &out,
  const namespacet &ns,
  const expr2tc &lhs,
  const expr2tc &value)
{
  const irep_idt &identifier = to_symbol2t(lhs).name;
  std::string value_string;

  if (is_nil_expr(value))
    value_string="(assignment removed)";
  else
  {
    value_string=from_expr(ns, identifier, value);

    if (is_constant_expr(value))
    {
      if(is_bv_type(value->type)) {
        value_string+= " (" +
          integer2string(to_constant_int2t(value).constant_value) + ")";
      } else if (is_fixedbv_type(value->type)) {
        value_string+= " (" +
          to_constant_fixedbv2t(value).value.to_ansi_c_string() + ")";
      }
    }
  }

  std::string name=id2string(identifier);

  const symbolt *symbol;
  if(!ns.lookup(identifier, symbol))
    if(symbol->pretty_name!="")
      name=id2string(symbol->pretty_name);

  out << "  " << name << "=" << value_string
      << std::endl;
}

void show_goto_trace_gui(
  std::ostream &out,
  const namespacet &ns,
  const goto_tracet &goto_trace)
{
  locationt previous_location;

  for(goto_tracet::stepst::const_iterator
      it=goto_trace.steps.begin();
      it!=goto_trace.steps.end();
      it++)
  {
    const locationt &location=it->pc->location;

    if(it->type==goto_trace_stept::ASSERT &&
       !it->guard)
    {
      out << "FAILED" << std::endl
          << it->comment << std::endl // value
          << std::endl // PC
          << location.file() << std::endl
          << location.line() << std::endl
          << location.column() << std::endl;
    }
    else if(it->type==goto_trace_stept::ASSIGNMENT)
    {
      irep_idt identifier;

      if (!is_nil_expr(it->original_lhs))
        identifier = to_symbol2t(it->original_lhs).name;
      else
        identifier = to_symbol2t(it->lhs).name;

      std::string value_string=from_expr(ns, identifier, it->value);

      const symbolt *symbol;
      irep_idt base_name;
      if(!ns.lookup(identifier, symbol))
        base_name=symbol->base_name;

      out << "TRACE" << std::endl;

      out << identifier << ","
          << base_name << ","
          << get_type_id(it->value->type) << ","
          << value_string << std::endl
          << it->step_nr << std::endl
          << it->pc->location.file() << std::endl
          << it->pc->location.line() << std::endl
          << it->pc->location.column() << std::endl;
    }
    else if(location!=previous_location)
    {
      // just the location

      if(location.file()!="")
      {
        out << "TRACE" << std::endl;

        out << ","             // identifier
            << ","             // base_name
            << ","             // type
            << "" << std::endl // value
            << it->step_nr << std::endl
            << location.file() << std::endl
            << location.line() << std::endl
            << location.column() << std::endl;
      }
    }

    previous_location=location;
  }
}

void show_state_header(
  std::ostream &out,
  const goto_trace_stept &state,
  const locationt &location,
  unsigned step_nr)
{
  out << std::endl;

  if(step_nr==0)
    out << "Initial State";
  else
    out << "State " << step_nr;

  out << " " << location
      << " thread " << state.thread_nr << std::endl;

  // Print stack trace

  std::vector<dstring>::const_iterator it;
  for (it = state.stack_trace.begin(); it != state.stack_trace.end(); it++)
    out << it->as_string() << std::endl;

  out << "----------------------------------------------------" << std::endl;
}

std::string get_varname_from_guard (
	goto_tracet::stepst::const_iterator &it,
	const goto_tracet &goto_trace)
{
  std::cerr << "XXXjmorse - get_varname_from_guard victim of migration" << std::endl;
  abort();
#if 0
  std::string varname;
  exprt guard_operand = it->pc->guard.op0();
  if (!guard_operand.operands().empty()) {
    if(!guard_operand.op0().identifier().as_string().empty()) {
      char identstr[guard_operand.op0().identifier().as_string().length()];
      strcpy(identstr, guard_operand.op0().identifier().c_str());
      //std::cout<<"Guard "<<it->pc->guard<<std::endl;
      int j=0;
      char * tok;
      tok = strtok (identstr,"::");
      while (tok != NULL) {
        if (j==4)
          varname = tok;
        tok = strtok (NULL, "::");
        j++;
      }
    }
  }
  return varname;
#endif
}

void get_metada_from_llvm(
  goto_tracet::stepst::const_iterator &it,
  const goto_tracet &goto_trace)
{
  char line[it->pc->location.get_line().as_string().length()];
  strcpy(line,it->pc->location.get_line().c_str());
  if (is_struct_type(it->rhs->type)) {
          struct_type2t &struct_type = const_cast<struct_type2t&>(to_struct_type(it->original_lhs->type));

	  std::string ident = to_symbol2t(it->original_lhs).name.as_string();
	  std::string struct_name = ident.substr(ident.find_last_of(":")+1);

	  u_int i = 0, j = 0;
	  for (std::vector<type2tc>::const_iterator itt = struct_type.members.begin();
	       itt != struct_type.members.end(); itt++, i++)
	  {
		  std::string comp_name = struct_type.member_names[j].as_string();
		  std::string key_map = struct_name + "." + struct_type.member_names[j].as_string().c_str();
		  if (goto_trace.llvm_varmap.find(key_map) != goto_trace.llvm_varmap.end() ){
			  std::string newname = goto_trace.llvm_varmap.find(key_map)->second;
			  struct_type.member_names[j] = irep_idt(newname);
		  }
	      j++;
	  }
  }
  if (!goto_trace.llvm_linemap.find(line)->second.empty()){
	  char VarInfo[goto_trace.llvm_linemap.find(line)->second.length()];
      if(!goto_trace.llvm_linemap.find(line)->second.empty()) {
     	  strcpy(VarInfo,goto_trace.llvm_linemap.find(line)->second.c_str());
      }
      char * pch;
      pch = strtok (VarInfo,"@#");
      int k=0;
      while (pch != NULL) {
        if (k==0) const_cast<goto_tracet*>(&goto_trace)->FileName = pch;
        if (k==1) const_cast<goto_tracet*>(&goto_trace)->LineNumber = pch;
        if (k==2) const_cast<goto_tracet*>(&goto_trace)->FuncName = pch;
        if (k==3) const_cast<goto_tracet*>(&goto_trace)->VarName = pch;
        //std::cout<<"varname - "<<goto_trace.VarName<<std::endl;
        if (k==4) const_cast<goto_tracet*>(&goto_trace)->OrigVarName = pch;
        pch = strtok (NULL, "@#");
        k++;
      }
      //********************change indentifier************************************/
                expr2tc &lhs = const_cast<expr2tc&>(it->original_lhs);
//		exprt* lhs = const_cast<exprt*>(&it->original_lhs);
		char identstr[to_symbol2t(it->original_lhs).name.as_string().size()];
		strcpy(identstr, to_symbol2t(it->original_lhs).name.as_string().c_str());
		int j=0;
		char * tok;
		tok = strtok (identstr,"::");
		std::string newidentifier;
		while (tok != NULL) {
		  if (j<=1) newidentifier = newidentifier + tok + "::";
		  if (j==2) newidentifier = newidentifier + goto_trace.FuncName + "::";
		  if (j==3) newidentifier = newidentifier + tok + "::";
		  if (j==4) {
			  if (!goto_trace.OrigVarName.empty()) newidentifier = newidentifier + goto_trace.OrigVarName;
			  else newidentifier = newidentifier + tok;
		  }
		  tok = strtok (NULL, "::");
		  j++;
		}
                to_symbol2t(lhs).name = irep_idt(newidentifier);
		//**********************************************************************/

        const_cast<locationt*>(&it->pc->location)->set_file(goto_trace.FileName);
        const_cast<locationt*>(&it->pc->location)->set_line(goto_trace.LineNumber);
        const_cast<locationt*>(&it->pc->location)->set_function(goto_trace.FuncName);
  }
}

void show_goto_trace(
  std::ostream &out,
  const namespacet &ns,
  const goto_tracet &goto_trace)
{
  unsigned prev_step_nr=0;
  bool first_step=true;

  if (!goto_trace.metadata_filename.empty())
    const_cast<goto_tracet*>(&goto_trace)->open_llvm_varmap();

  for(goto_tracet::stepst::const_iterator
      it=goto_trace.steps.begin();
      it!=goto_trace.steps.end();
      it++)
  {
    switch(it->type)
    {
    case goto_trace_stept::ASSERT:
      if(!it->guard)
      {
        out << std::endl;
        out << "Violated property:" << std::endl;
        if(!it->pc->location.is_nil()) {
            if (!goto_trace.metadata_filename.empty()) {
              get_metada_from_llvm(it, goto_trace);
             }
            out << "  " << it->pc->location << std::endl;
        }
        //std::cout<<"comment "<<it->comment<<std::endl;
        out << "  " << it->comment << std::endl;

        if(it->pc->is_assert())
#if 0
        	if (!goto_trace.metadata_filename.empty() && !it->pc->guard.operands().empty()) {
				std::string assertsrt, varname;
				assertsrt = from_expr(ns, "", it->pc->guard);
				varname=get_varname_from_guard(it,goto_trace);
				if(!goto_trace.llvm_varmap.find(varname)->second.empty()) {
					assertsrt.replace(assertsrt.find(varname),varname.length(),goto_trace.llvm_varmap.find(varname)->second);
					out << "  " << assertsrt<< std::endl;
				}
        	}
        	else
#endif
        		out << "  " << from_expr(ns, "", it->pc->guard)<< std::endl;
        //std::cout<<"VarName "<<goto_trace.VarName<<" OrigVarName "<<goto_trace.OrigVarName<<std::endl;
        out << std::endl;
      }
      break;

    case goto_trace_stept::ASSUME:
      break;

    case goto_trace_stept::ASSIGNMENT:
      if(it->pc->is_assign() ||
         (it->pc->is_other() && is_nil_expr(it->lhs)))
      {
        if(prev_step_nr!=it->step_nr || first_step)
        {
          first_step=false;
          prev_step_nr=it->step_nr;
          if (!goto_trace.metadata_filename.empty()) {
            get_metada_from_llvm(it, goto_trace);
          }
          show_state_header(out, *it, it->pc->location, it->step_nr);
        }
         counterexample_value(out, ns, it->original_lhs, it->value);
      }
      break;

    case goto_trace_stept::OUTPUT:
      {
        printf_formattert printf_formatter;

        std::list<exprt> vec;

        for (std::list<expr2tc>::const_iterator it2 = it->output_args.begin();
             it2 != it->output_args.end(); it2++) {
          vec.push_back(migrate_expr_back(*it2));
        }

        printf_formatter(it->format_string, vec);
        printf_formatter.print(out);
        out << std::endl;
      }
      break;

    default:
      assert(false);
    }
  }
}

