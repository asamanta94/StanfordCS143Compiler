#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#include <map>

#define TRUE 1
#define FALSE 0
#define TEMP_DEBUG FALSE

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;

  /* Inheitance Graph */
  std::map<Symbol, Class_> inheritance_graph;

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);

  /* Symbol Table for a class */
  std::map<Symbol, SymbolTable<Symbol, Feature_class> > symbol_table;
  std::map<Symbol, SymbolTable<Symbol, Feature_class> > attribute_table;

  /* Functions for the Inheritance Graph */
  void build_ig(Classes classes);
  void check_cycle_ig();
  void print_ig();

  /* Functions for the Symbol Table */
  void print_symbol_table();
  void generate_symbol_table();
  void create_symbol_table();

  bool compare_formals(Formals f1, Formals f2, Symbol m);
  bool compare_methods(Feature m1, Feature m2);
  bool redefined(Feature f, Symbol class_name);
  void check_inherited_features();

  /* Evaluate program */
  void evaluate_program();
  void get_inheritance_list(Symbol class_name, std::stack<Symbol>& st);
  Symbol lub(Symbol t1, Symbol t2);
  Symbol find_type(Symbol name, Symbol class_name);
  bool does_conform(Symbol base, Symbol class_name);
  Symbol find_class_type(Symbol class_name);
  Class_ get_class(Symbol class_name);
  Feature find_method(Symbol method_name, Symbol class_name);
  void add_attribute(Symbol class_name, Symbol name, Symbol type_decl, Expression init);
  void remove(Symbol class_name);
  void enter_scope(Symbol class_name);
};


#endif

