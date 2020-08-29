#include <assert.h>
#include <stdio.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

#include <map>
#include <vector>

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

enum VAR_TYPE
{
  ATTRIBUTE = 0,
  FORMAL,
  LOCAL,
  ERROR
};

struct attr_info
{
  int index;
  VAR_TYPE type;
  Feature attr;
  Formal formal;
};

typedef attr_info *ainfo_t;

struct class_info
{
  int class_tag;
  int class_max_tag;
  int class_size;
  std::map<Symbol, Symbol> disp_tab;
  std::map<Symbol, ainfo_t> attr_tab;
  std::vector<Symbol> method_stack;
};

typedef class_info *cinfo_t;

struct method_info
{
  Symbol method_name;
  Symbol class_name;
};

class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   List<CgenNode> *nds;
   ostream& str;
   int stringclasstag;
   int intclasstag;
   int boolclasstag;


// The following methods emit code for
// constants and global declarations.

    void code_global_data();
    void code_global_text();
    void code_bools(int);
    void code_select_gc();
    void code_constants();
    void code_prototype_objects();
    void code_class_nameTab();
    void code_dispatch_tables();
    void code_class_objtab();
    void code_object_initializer();
    void code_class_methods();

    void get_methods(std::map<Symbol, Symbol>& disp_tab, CgenNodeP class_);
    void emit_prototype_object(CgenNodeP class_);
    void emit_object_init(CgenNodeP class_);

    void construct_cinfo(cinfo_t cinfo, CgenNodeP class_);
    void get_class_info();

    void print_cinfo();

// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

   void install_basic_classes();
   void install_class(CgenNodeP nd);
   void install_classes(Classes cs);
   void build_inheritance_tree();
   void set_relations(CgenNodeP nd);
   void code_single_obj(CgenNodeP class_);
   int set_idx(CgenNodeP class_, int index);

public:
   CgenClassTable(Classes, ostream& str);
   void code();
   CgenNodeP root();
};

class CgenNode : public class__class {
private: 
   CgenNodeP parentnd;                        // Parent of class
   List<CgenNode> *children;                  // Children of class
   Basicness basic_status;                    // `Basic' if class is basic
                                              // `NotBasic' otherwise
   cinfo_t cinfo;

public:
   CgenNode(Class_ c,
            Basicness bstatus,
            CgenClassTableP class_table);

   void add_child(CgenNodeP child);
   List<CgenNode> *get_children() { return children; }
   void set_parentnd(CgenNodeP p);
   CgenNodeP get_parentnd() { return parentnd; }
   int basic() { return (basic_status == Basic); }
   cinfo_t get_cinfo() { return cinfo; }
   void set_cinfo(cinfo_t cinfo) { this->cinfo = cinfo; }
};

class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};

class ClassEnv
{
  CgenNodeP class_;

  SymbolTable<Symbol, struct attr_info> var_tab;

public:
  ClassEnv(CgenNodeP c) { this->class_ = c; }

  CgenNodeP get_class() { return class_; }

  ainfo_t get_var(Symbol id)
  {
    // Add other. This will contain formals and inner defs.
    if (var_tab.lookup(id))
    {
      return var_tab.lookup(id);
    }

    cinfo_t cinfo = class_->get_cinfo();
    if (cinfo)
    {
      if (cinfo->attr_tab.find(id) != cinfo->attr_tab.end())
      {
        return cinfo->attr_tab[id];
      }
    }

    return NULL;
  }

  void enter_scope()
  {
    var_tab.enterscope();
  }

  void exit_scope()
  {
    var_tab.exitscope();
  }

  void add_var(Symbol var, ainfo_t info)
  {
    var_tab.addid(var, info);
  }
};

typedef ClassEnv *EnvP;