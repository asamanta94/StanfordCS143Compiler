
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"
#include <string>
#include <typeinfo>
#include <vector>
#include <algorithm>

extern void emit_string_constant(ostream& str, char *s);
extern int cgen_debug;

#ifndef TEMP_DEBUG
#define TEMP_DEBUG 0
#endif
//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
       arg,
       arg2,
       Bool,
       concat,
       cool_abort,
       copy,
       Int,
       in_int,
       in_string,
       IO,
       length,
       Main,
       main_meth,
       No_class,
       No_type,
       Object,
       out_int,
       out_string,
       prim_slot,
       self,
       SELF_TYPE,
       Str,
       str_field,
       substr,
       type_name,
       val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
  arg         = idtable.add_string("arg");
  arg2        = idtable.add_string("arg2");
  Bool        = idtable.add_string("Bool");
  concat      = idtable.add_string("concat");
  cool_abort  = idtable.add_string("abort");
  copy        = idtable.add_string("copy");
  Int         = idtable.add_string("Int");
  in_int      = idtable.add_string("in_int");
  in_string   = idtable.add_string("in_string");
  IO          = idtable.add_string("IO");
  length      = idtable.add_string("length");
  Main        = idtable.add_string("Main");
  main_meth   = idtable.add_string("main");
//   _no_class is a symbol that can't be the name of any 
//   user-defined class.
  No_class    = idtable.add_string("_no_class");
  No_type     = idtable.add_string("_no_type");
  Object      = idtable.add_string("Object");
  out_int     = idtable.add_string("out_int");
  out_string  = idtable.add_string("out_string");
  prim_slot   = idtable.add_string("_prim_slot");
  self        = idtable.add_string("self");
  SELF_TYPE   = idtable.add_string("SELF_TYPE");
  Str         = idtable.add_string("String");
  str_field   = idtable.add_string("_str_field");
  substr      = idtable.add_string("substr");
  type_name   = idtable.add_string("type_name");
  val         = idtable.add_string("_val");
}

static int label_counter = 0;
static int stack_index = 0;

static char *gc_init_names[] =
  { "_NoGC_Init", "_GenGC_Init", "_ScnGC_Init" };
static char *gc_collect_names[] =
  { "_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect" };

std::map<Symbol, CgenNodeP> class_tab;

//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);

  os << "\n# end of generated code\n";
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(char *dest_reg, int offset, char *source_reg, ostream& s)
{
  s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")" 
    << endl;
}

static void emit_store(char *source_reg, int offset, char *dest_reg, ostream& s)
{
  s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")"
      << endl;
}

static void emit_load_imm(char *dest_reg, int val, ostream& s)
{ s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, ostream& s)
{ s << LA << dest_reg << " " << address << endl; }

static void emit_partial_load_address(char *dest_reg, ostream& s)
{ s << LA << dest_reg << " "; }

static void emit_load_bool(char *dest, const BoolConst& b, ostream& s)
{
  emit_partial_load_address(dest,s);
  b.code_ref(s);
  s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, ostream& s)
{
  emit_partial_load_address(dest,s);
  str->code_ref(s);
  s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, ostream& s)
{
  emit_partial_load_address(dest,s);
  i->code_ref(s);
  s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, ostream& s)
{ s << MOVE << dest_reg << " " << source_reg << endl; }

static void emit_neg(char *dest, char *src1, ostream& s)
{ s << NEG << dest << " " << src1 << endl; }

static void emit_add(char *dest, char *src1, char *src2, ostream& s)
{ s << ADD << dest << " " << src1 << " " << src2 << endl; }

static void emit_addu(char *dest, char *src1, char *src2, ostream& s)
{ s << ADDU << dest << " " << src1 << " " << src2 << endl; }

static void emit_addiu(char *dest, char *src1, int imm, ostream& s)
{ s << ADDIU << dest << " " << src1 << " " << imm << endl; }

static void emit_div(char *dest, char *src1, char *src2, ostream& s)
{ s << DIV << dest << " " << src1 << " " << src2 << endl; }

static void emit_mul(char *dest, char *src1, char *src2, ostream& s)
{ s << MUL << dest << " " << src1 << " " << src2 << endl; }

static void emit_sub(char *dest, char *src1, char *src2, ostream& s)
{ s << SUB << dest << " " << src1 << " " << src2 << endl; }

static void emit_sll(char *dest, char *src1, int num, ostream& s)
{ s << SLL << dest << " " << src1 << " " << num << endl; }

static void emit_jalr(char *dest, ostream& s)
{ s << JALR << "\t" << dest << endl; }

static void emit_jal(char *address,ostream &s)
{ s << JAL << address << endl; }

static void emit_return(ostream& s)
{ s << RET << endl; }

static void emit_gc_assign(ostream& s)
{ s << JAL << "_GenGC_Assign" << endl; }

static void emit_disptable_ref(Symbol sym, ostream& s)
{  s << sym << DISPTAB_SUFFIX; }

static void emit_init_ref(Symbol sym, ostream& s)
{ s << sym << CLASSINIT_SUFFIX; }

static void emit_label_ref(int l, ostream &s)
{ s << "label" << l; }

static void emit_protobj_ref(Symbol sym, ostream& s)
{ s << sym << PROTOBJ_SUFFIX; }

static void emit_method_ref(Symbol classname, Symbol methodname, ostream& s)
{ s << classname << METHOD_SEP << methodname; }

static void emit_label_def(int l, ostream &s)
{
  emit_label_ref(l,s);
  s << ":" << endl;
}

static void emit_beqz(char *source, int label, ostream &s)
{
  s << BEQZ << source << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s)
{
  s << BEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s)
{
  s << BNE << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s)
{
  s << BLEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s)
{
  s << BLT << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blti(char *src1, int imm, int label, ostream &s)
{
  s << BLT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s)
{
  s << BGT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_branch(int l, ostream& s)
{
  s << BRANCH;
  emit_label_ref(l,s);
  s << endl;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char *reg, ostream& str)
{
  emit_store(reg,0,SP,str);
  emit_addiu(SP,SP,-4,str);
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream& s)
{ emit_load(dest, DEFAULT_OBJFIELDS, source, s); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }


static void emit_test_collector(ostream &s)
{
  emit_push(ACC, s);
  emit_move(ACC, SP, s); // stack end
  emit_move(A1, ZERO, s); // allocate nothing
  s << JAL << gc_collect_names[cgen_Memmgr] << endl;
  emit_addiu(SP,SP,4,s);
  emit_load(ACC,0,SP,s);
}

static void emit_gc_check(char *source, ostream &s)
{
  if (source != (char*)A1) emit_move(A1, source, s);
  s << JAL << "_gc_check" << endl;
}


///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream& s)
{
  s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream& s, int stringclasstag)
{
  IntEntryP lensym = inttable.add_int(len);

  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s  << LABEL                                             // label
      << WORD << stringclasstag << endl                                 // tag
      << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len+4)/4) << endl // size
      << WORD;


 /***** Add dispatch information for class String ******/

      emit_disptable_ref(Str, s);
      s << endl;                                              // dispatch table
      s << WORD;  lensym->code_ref(s);  s << endl;            // string length
  emit_string_constant(s,str);                                // ascii string
  s << ALIGN;                                                 // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream& s, int stringclasstag)
{  
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s)
{
  s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                // label
      << WORD << intclasstag << endl                      // class tag
      << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl  // object size
      << WORD; 

 /***** Add dispatch information for class Int ******/

      emit_disptable_ref(Int, s);
      s << endl;                                          // dispatch table
      s << WORD << str << endl;                           // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag)
{
  for (List<IntEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,intclasstag);
}


//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream& s) const
{
  s << BOOLCONST_PREFIX << val;
}
  
//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream& s, int boolclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                  // label
      << WORD << boolclasstag << endl                       // class tag
      << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl   // object size
      << WORD;

 /***** Add dispatch information for class Bool ******/

      emit_disptable_ref(Bool, s);
      s << endl;                                            // dispatch table
      s << WORD << val << endl;                             // value (0 or 1)
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data()
{
  Symbol main    = idtable.lookup_string(MAINNAME);
  Symbol string  = idtable.lookup_string(STRINGNAME);
  Symbol integer = idtable.lookup_string(INTNAME);
  Symbol boolc   = idtable.lookup_string(BOOLNAME);

  str << "\t.data\n" << ALIGN;
  //
  // The following global names must be defined first.
  //
  str << GLOBAL << CLASSNAMETAB << endl;
  str << GLOBAL; emit_protobj_ref(main,str);    str << endl;
  str << GLOBAL; emit_protobj_ref(integer,str); str << endl;
  str << GLOBAL; emit_protobj_ref(string,str);  str << endl;
  str << GLOBAL; falsebool.code_ref(str);  str << endl;
  str << GLOBAL; truebool.code_ref(str);   str << endl;
  str << GLOBAL << INTTAG << endl;
  str << GLOBAL << BOOLTAG << endl;
  str << GLOBAL << STRINGTAG << endl;

  //
  // We also need to know the tag of the Int, String, and Bool classes
  // during code generation.
  //
  str << INTTAG << LABEL
      << WORD << intclasstag << endl;
  str << BOOLTAG << LABEL 
      << WORD << boolclasstag << endl;
  str << STRINGTAG << LABEL 
      << WORD << stringclasstag << endl;    
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text()
{
  str << GLOBAL << HEAP_START << endl
      << HEAP_START << LABEL 
      << WORD << 0 << endl
      << "\t.text" << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Main"), str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Int"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("String"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Bool"),str);
  str << endl << GLOBAL;
  emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
  str << endl;
}

void CgenClassTable::code_bools(int boolclasstag)
{
  falsebool.code_def(str,boolclasstag);
  truebool.code_def(str,boolclasstag);
}

void CgenClassTable::code_select_gc()
{
  //
  // Generate GC choice constants (pointers to GC functions)
  //
  str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
  str << "_MemMgr_INITIALIZER:" << endl;
  str << WORD << gc_init_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
  str << "_MemMgr_COLLECTOR:" << endl;
  str << WORD << gc_collect_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_TEST" << endl;
  str << "_MemMgr_TEST:" << endl;
  str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants()
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(str,stringclasstag);
  inttable.code_string_table(str,intclasstag);
  code_bools(boolclasstag);
}

/* Emit code for one object initializer.
 */
void CgenClassTable::emit_object_init(CgenNodeP class_)
{
  /* addiu   $sp $sp -12
  sw      $fp 12($sp)
  sw      $s0 8($sp)
  sw      $ra 4($sp)
  addiu   $fp $sp 16
  move    $s0 $a0
  # Init code
  move    $a0 $s0
  lw      $fp 12($sp)
  lw      $s0 8($sp)
  lw      $ra 4($sp)
  addiu   $sp $sp 12
  jr      $ra */

  // <Class name>_init:
  emit_init_ref(class_->get_name(), str);
  str << LABEL;

  emit_addiu(SP, SP, -12, str);     // addiu   $sp $sp -12
  emit_store(FP, 3, SP, str);       // sw      $fp 12($sp)
  emit_store(SELF, 2, SP, str);     // sw      $s0 8($sp)
  emit_store(RA, 1, SP, str);       // sw      $ra 4($sp)
  emit_addiu(FP, SP, 16, str);      // addiu   $fp $sp 16
  emit_move(SELF, ACC, str);        // move    $s0 $a0

  // Init
  // Create an environment for the class
  EnvP e = new ClassEnv(class_);

  // Initialize parent class if parent class exists.
  CgenNodeP parent = class_->get_parentnd();
  if (parent->get_name() != No_class)
  {
    std::string parent_init_method = parent->get_name()->get_string();
    parent_init_method += CLASSINIT_SUFFIX;
    emit_jal(((char *) parent_init_method.c_str()), str);
  }

  // Initialize all attributes
  std::map<Symbol, ainfo_t>::iterator it;
  cinfo_t cinfo = class_->get_cinfo();
  if (cinfo)
  {
    Features features = class_->get_features();
    for (int i = features->first(); features->more(i); i = features->next(i))
    {
      Feature f = features->nth(i);
      if (!f->is_method())
      {
        ainfo_t ainfo = cinfo->attr_tab[f->get_name()];
        #if TEMP_DEBUG
          cout << f->get_name() << ":" << ainfo->index << endl;
        #endif
        attr_class * attr = ((attr_class *) (ainfo->attr));
        Expression init = attr->get_init_expr();
        Symbol type_decl = attr->get_type_decl();

	      // Basic types.
        if (init->is_no_expr())
        {
          if (type_decl == Int)
          {
            emit_load_int(ACC, inttable.lookup_string("0"), str);
          }
          else if (type_decl == Str)
          {
            emit_load_string(ACC, stringtable.lookup_string(""), str);
          }
          else if (type_decl == Bool)
          {
            emit_load_bool(ACC, falsebool, str);
          }
          else
          {
            init->code(str, e);
          }
          emit_store(ACC, ainfo->index + DEFAULT_OBJFIELDS, SELF, str);
        }
        else
        {
          attr->get_init_expr()->code(str, e);
          emit_store(ACC, ainfo->index + DEFAULT_OBJFIELDS, SELF, str);
          if (cgen_Memmgr)
          {
            emit_addiu(A1, SELF, 4 * (ainfo->index + DEFAULT_OBJFIELDS), str); // addiu $a1 $s0 12
            emit_gc_assign(str);         
          }
        }
      }
    }
  }

  emit_move(ACC, SELF, str);        // move    $a0 $s0
  emit_load(FP, 3, SP, str);        // lw      $fp 12($sp)
  emit_load(SELF, 2, SP, str);      // lw      $s0 8($sp)
  emit_load(RA, 1, SP, str);        // lw      $ra 4($sp)
  emit_addiu(SP, SP, 12, str);      // addiu   $sp $sp 12
  emit_return(str);                 // jr      $ra
}

/* Emit code for all object initializers.
 */
void CgenClassTable::code_object_initializer()
{
  if (cgen_debug)
  {
    cout << typeid(*this).name() << "::" << __func__ << endl;
  }

  for(List<CgenNode> *l = nds; l; l = l->tl())
  {
    emit_object_init(l->hd());
  }
}

void CgenClassTable::code_single_obj(CgenNodeP class_)
{
  str << WORD;
  emit_protobj_ref(class_->get_name(), str);
  str << endl << WORD;
  emit_init_ref(class_->get_name(), str);
  str << endl;

  for (List<CgenNode> * c = class_->get_children(); c; c = c->tl())
  {
    code_single_obj(c->hd());
  }
}

/* Emit code for class object table.
 */
void CgenClassTable::code_class_objtab()
{
  if (cgen_debug)
  {
    cout << typeid(*this).name() << "::" << __func__ << endl;
  }

  str << CLASSOBJTAB << LABEL;
  code_single_obj(class_tab[Object]);
}

/* Emit code for a single prototype object for each class
 */
void CgenClassTable::emit_prototype_object(CgenNodeP class_)
{
  if (class_->get_name() == No_class)
  {
    return;
  }

  // TODO: _val, _prim_slot

  std::map<Symbol, ainfo_t>::iterator it;

  emit_protobj_ref(class_->get_name(), str);
  str << LABEL;

  cinfo_t info = class_->get_cinfo();
  if (info)
  {
    str << WORD << info->class_tag << endl;
    str << WORD << info->class_size << endl;
    str << WORD;
    emit_disptable_ref(class_->get_name(), str);
    str << endl;

    for (it = info->attr_tab.begin(); it != info->attr_tab.end(); ++it)
    {
      str << WORD;
      #if TEMP_DEBUG
        cout << it->first << ":" << it->second;
      #endif

      ainfo_t ainfo = it->second;
      attr_class * attr = ((attr_class *) ainfo->attr);
      Symbol type_decl = attr->get_type_decl();

      if (attr->name == val)      // Initialize the _val for basic classes.
      {
        if (class_->get_name() == Str)
        {
          str << "0";
        }
        else
        {
          str << "0";
        }
      }
      else if (attr->name == str_field)
      {
          IntEntry * e = inttable.lookup_string("0");
          e->code_ref(str);
      }
      else if (type_decl == Int)
      {
        IntEntry * e = inttable.lookup_string("0");
        e->code_ref(str);
      }
      else if (type_decl == Str)
      {
        StringEntry * e = stringtable.lookup_string("");
        e->code_ref(str);
      }
      else if (type_decl == Bool)
      {
        falsebool.code_ref(str);
      }
      else
      {
        str << 0;
      }
      str << endl;
    }
  }

  str << WORD << "-1" << endl;
}

/* Emit code for prototype object for each class
 */
void CgenClassTable::code_prototype_objects()
{
  if (cgen_debug)
  {
    cout << typeid(*this).name() << "::" << __func__ << endl;
  }

  for(List<CgenNode> *l = nds; l; l = l->tl())
  {
    CgenNodeP c = l->hd();

    emit_prototype_object(c);
  }
}

/* Emit code for class_nameTab
 */
void CgenClassTable::code_class_nameTab()
{
  if (cgen_debug)
  {
    cout << typeid(*this).name() << "::" << __func__ << endl;
  }

  str << CLASSNAMETAB << LABEL;
  for(List<CgenNode> *l = nds; l; l = l->tl())
  {
    CgenNodeP class_ = l->hd();

    StringEntry * entry = stringtable.lookup_string(class_->get_name()->get_string());

    str << WORD;
    entry->code_ref(str);
    str << endl;
  }
}

/* Emit code for all the dispatch tables.
 */
void CgenClassTable::code_dispatch_tables()
{
  if (cgen_debug)
  {
    cout << typeid(*this).name() << "::" << __func__ << endl;
  }

  std::map<Symbol, Symbol>::iterator it;

  // Iterate through all the classes, get their info and emit code.
  for(List<CgenNode> *l = nds; l; l = l->tl())
  {
    CgenNodeP class_ = l->hd();

    cinfo_t info = class_->get_cinfo();

    emit_disptable_ref(class_->get_name(), str);
    str << LABEL;

    if (info)
    {
      for (unsigned int i = 0; i < info->method_stack.size(); i++)
      {
        str << WORD;
        emit_method_ref(info->disp_tab[info->method_stack[i]], info->method_stack[i], str);
        str << endl;
      }
    }
  }
}

void CgenClassTable::code_class_methods()
{
  if (cgen_debug)
  {
    cout << typeid(*this).name() << "::" << __func__ << endl;
  }

  Features features = NULL;
  Feature f = NULL;

  // Iterate through all the classes, and emit the code for their methods.
  for(List<CgenNode> *l = nds; l; l = l->tl())
  {
    CgenNodeP class_ = l->hd();

    // No need to generate methods for basic classes. They're included in runtime env.
    if (class_->basic())
    {
      continue;
    }

    features = class_->get_features();

    for (int i = features->first(); features->more(i); i = features->next(i))
    {
      f = features->nth(i);
      if (f->is_method())
      {
        EnvP e = new ClassEnv(class_);
        f->code(e, str);
      }
    } 
  }
}

CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL) , str(s)
{
   stringclasstag = 0 /* Change to your String class tag here */;
   intclasstag =    0 /* Change to your Int class tag here */;
   boolclasstag =   0 /* Change to your Bool class tag here */;

   enterscope();
   if (cgen_debug) cout << "Building CgenClassTable" << endl;
   install_basic_classes();
   install_classes(classes);
   build_inheritance_tree();

   get_class_info();

   stringclasstag = class_tab[Str]->get_cinfo()->class_tag; /* Change to your String class tag here */;
   intclasstag = class_tab[Int]->get_cinfo()->class_tag; /* Change to your Int class tag here */;
   boolclasstag = class_tab[Bool]->get_cinfo()->class_tag; /* Change to your Bool class tag here */;

   code();
   exitscope();
}

void CgenClassTable::install_basic_classes()
{

// The tree package uses these globals to annotate the classes built below.
  //curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

//
// A few special class names are installed in the lookup table but not
// the class list.  Thus, these classes exist, but are not part of the
// inheritance hierarchy.
// No_class serves as the parent of Object and the other special classes.
// SELF_TYPE is the self class; it cannot be redefined or inherited.
// prim_slot is a class known to the code generator.
//
  addid(No_class,
	new CgenNode(class_(No_class,No_class,nil_Features(),filename),
			    Basic,this));
  addid(SELF_TYPE,
	new CgenNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
			    Basic,this));
  addid(prim_slot,
	new CgenNode(class_(prim_slot,No_class,nil_Features(),filename),
			    Basic,this));

// 
// The Object class has no parent class. Its methods are
//        cool_abort() : Object    aborts the program
//        type_name() : Str        returns a string representation of class name
//        copy() : SELF_TYPE       returns a copy of the object
//
// There is no need for method bodies in the basic classes---these
// are already built in to the runtime system.
//
  install_class(
   new CgenNode(
    class_(Object, 
	   No_class,
	   append_Features(
           append_Features(
           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
           single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	   filename),
    Basic,this));

// 
// The IO class inherits from Object. Its methods are
//        out_string(Str) : SELF_TYPE          writes a string to the output
//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
//        in_string() : Str                    reads a string from the input
//        in_int() : Int                         "   an int     "  "     "
//
   install_class(
    new CgenNode(
     class_(IO, 
            Object,
            append_Features(
            append_Features(
            append_Features(
            single_Features(method(out_string, single_Formals(formal(arg, Str)),
                        SELF_TYPE, no_expr())),
            single_Features(method(out_int, single_Formals(formal(arg, Int)),
                        SELF_TYPE, no_expr()))),
            single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
            single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	   filename),	    
    Basic,this));

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
   install_class(
    new CgenNode(
     class_(Int, 
	    Object,
            single_Features(attr(val, prim_slot, no_expr())),
	    filename),
     Basic,this));

//
// Bool also has only the "val" slot.
//
    install_class(
     new CgenNode(
      class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename),
      Basic,this));

//
// The class Str has a number of slots and operations:
//       val                                  ???
//       str_field                            the string itself
//       length() : Int                       length of the string
//       concat(arg: Str) : Str               string concatenation
//       substr(arg: Int, arg2: Int): Str     substring
//       
   install_class(
    new CgenNode(
      class_(Str, 
	     Object,
             append_Features(
             append_Features(
             append_Features(
             append_Features(
             single_Features(attr(val, Int, no_expr())),
            single_Features(attr(str_field, prim_slot, no_expr()))),
            single_Features(method(length, nil_Formals(), Int, no_expr()))),
            single_Features(method(concat, 
				   single_Formals(formal(arg, Str)),
				   Str, 
				   no_expr()))),
	    single_Features(method(substr, 
				   append_Formals(single_Formals(formal(arg, Int)), 
						  single_Formals(formal(arg2, Int))),
				   Str, 
				   no_expr()))),
	     filename),
        Basic,this));

}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_class(CgenNodeP nd)
{
  Symbol name = nd->get_name();

  if (probe(name))
    {
      return;
    }

  // The class name is legal, so add it to the list of classes
  // and the symbol table.
  nds = new List<CgenNode>(nd,nds);
  addid(name,nd);
}

void CgenClassTable::install_classes(Classes cs)
{
  for(int i = cs->first(); cs->more(i); i = cs->next(i))
    install_class(new CgenNode(cs->nth(i),NotBasic,this));
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
  for(List<CgenNode> *l = nds; l; l = l->tl())
      set_relations(l->hd());
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
  CgenNode *parent_node = probe(nd->get_parent());
  nd->set_parentnd(parent_node);
  parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
  children = new List<CgenNode>(n,children);
}

void CgenNode::set_parentnd(CgenNodeP p)
{
  assert(parentnd == NULL);
  assert(p != NULL);
  parentnd = p;
}

static int attr_idx = 0;

/* Recursively construct class information. Add inherited
 * attributes by recursively walking up the AST.
 */
void CgenClassTable::construct_cinfo(cinfo_t cinfo, CgenNodeP class_)
{
  // Nothing to do if class is No_class.
  if (class_->get_name() != No_class)
  {
    construct_cinfo(cinfo, class_->get_parentnd());
  }

  Feature f = NULL;
  Features features = class_->get_features();

  // Get all features and check if it's a method. It it's a method, add it to the dispatch table.
  for (int i = features->first(); features->more(i); i = features->next(i))
  {
    f = features->nth(i);
    if (f->is_method())
    {
      cinfo->disp_tab[f->get_name()] = class_->get_name();
      if (std::find(cinfo->method_stack.begin(), cinfo->method_stack.end(), f->get_name()) == cinfo->method_stack.end())
      {
        cinfo->method_stack.push_back(f->get_name());
      }
    }
    else
    {
      ainfo_t info = new struct attr_info;
      info->index = attr_idx++;
      info->attr = f;
      info->type = ATTRIBUTE;
      cinfo->attr_tab[f->get_name()] = info;
    }
  }
}

int CgenClassTable::set_idx(CgenNodeP class_, int index)
{
  cinfo_t info = class_->get_cinfo();
  info->class_tag = index;
  List<CgenNode> * children = class_->get_children();

  int max_idx = index;
  int curr_idx = index;
  for (List<CgenNode> * c = children; c; c = c->tl())
  {
    max_idx = set_idx(c->hd(), curr_idx + 1);
    if(max_idx > curr_idx)
      curr_idx = max_idx;
  }

  info->class_max_tag = max_idx;
  return max_idx;
}

/* Get class information for all classes.
 * Create a record for each class i.e.
 * 1. Class Tag
 * 2. Class Size
 * 3. Methods (inherited also)
 * 4. Attributes (inherited also)
 */
void CgenClassTable::get_class_info()
{
  if (cgen_debug)
  {
    cout << typeid(*this).name() << "::" << __func__ << endl;
  }

  int i = -1;
  int j = 0;

  std::map<Symbol, ainfo_t>::iterator it;

  for(List<CgenNode> *l = nds; l; l = l->tl())
  {
    CgenNodeP class_ = l->hd();

    cinfo_t info = new struct class_info;

    attr_idx = 0;

    construct_cinfo(info, class_);

    info->class_tag = -1;
    info->class_max_tag = -1;

    info->class_size = info->attr_tab.size() + DEFAULT_OBJFIELDS;

    class_->set_cinfo(info);

    class_tab[class_->get_name()] = class_;
  }

  CgenNodeP class_ = class_tab[Object];
  j = set_idx(class_, i + 1);

  print_cinfo();
}

void CgenClassTable::print_cinfo()
{
  #if TEMP_DEBUG
    for(List<CgenNode> *l = nds; l; l = l->tl())
    {
      CgenNodeP class_ = l->hd();
      cout << class_->get_name() << ":" << endl;
      cinfo_t info = class_->get_cinfo();
      if (info)
      {
        cout << "HERE" << endl;
      }
    }
  #endif
}

void CgenClassTable::code()
{
  if (cgen_debug) cout << "coding global data" << endl;
  code_global_data();

  if (cgen_debug) cout << "choosing gc" << endl;
  code_select_gc();

  if (cgen_debug) cout << "coding constants" << endl;
  code_constants();

  // get_class_info();

  //                 Add your code to emit
  //                   - class_nameTab
  if (cgen_debug) cout << "coding class nameTab" << endl;
  code_class_nameTab();

  //                   - dispatch tables
  if (cgen_debug) cout << "coding dispatch tables" << endl;
  code_dispatch_tables();
  
  //                  - class_objTab
  if (cgen_debug) cout << "coding class objTab" << endl;
  code_class_objtab();

  //                   - prototype objects
  if (cgen_debug) cout << "coding prototype objects" << endl;
  code_prototype_objects();

  if (cgen_debug) cout << "coding global text" << endl;
  code_global_text();

//                 Add your code to emit
//                   - object initializer
  if (cgen_debug) cout << "coding object initializer" << endl;
  code_object_initializer();

//                   - the class methods
  if (cgen_debug) cout << "coding class methods" << endl;
  code_class_methods();
//                   - etc...

}


CgenNodeP CgenClassTable::root()
{
   return probe(Object);
}


///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) :
   class__class((const class__class &) *nd),
   parentnd(NULL),
   children(NULL),
   basic_status(bstatus)
{ 
   stringtable.add_string(name->get_string());          // Add class name to string table
}


//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

void add_formal_to_env(EnvP e, Formal formal, int i)
{
  ainfo_t ainfo = new struct attr_info;
  ainfo->index = i;
  ainfo->type = FORMAL;
  ainfo->formal = formal;
  ainfo->attr = NULL;
  e->add_var(formal->get_name(), ainfo);
}

void method_class::code(EnvP env, ostream& str) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  // <Class name>_init:
  emit_method_ref(env->get_class()->get_name(), name, str);
  str << LABEL;

  str << ADDIU << SP << " " << SP << " " << -12 << "#" << env->get_class()->get_name() << ":" <<  name << endl;
  // emit_addiu(SP, SP, -12, str);    str << "#" << name << endl; // addiu   $sp $sp -12
  emit_store(FP, 3, SP, str);       // sw      $fp 12($sp)
  emit_store(SELF, 2, SP, str);     // sw      $s0 8($sp)
  emit_store(RA, 1, SP, str);       // sw      $ra 4($sp)
  emit_addiu(FP, SP, 16, str);      // addiu   $fp $sp 16
  emit_move(SELF, ACC, str);        // move    $s0 $a0  

  // Enter new scope to add formals. Modify environment.
  env->enter_scope();

  // Add formals to scope.
  for (int i = formals->first(); formals->more(i); i = formals->next(i))
  {
    Formal formal = formals->nth(i);
    add_formal_to_env(env, formal, formals->len() - i - 1);
  }

  // Code Method body
  expr->code(str, env);

  // Exit scope. We're done with the formals in the method.
  env->exit_scope();

  emit_load(FP, 3, SP, str);        // lw      $fp 12($sp)
  emit_load(SELF, 2, SP, str);      // lw      $s0 8($sp)
  emit_load(RA, 1, SP, str);        // lw      $ra 4($sp)
  emit_addiu(SP, SP, 12, str);      // addiu   $sp $sp 12
  
  // Pop all the vars from the stack.
 for (int i = formals->first(); formals->more(i); i = formals->next(i))
  {
    emit_addiu(SP, SP, 4, str);
  }

  emit_return(str);                 // jr      $ra
}

void assign_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  // TODO:

  expr->code(s, e);                                               // Evaluate expr first.

  ainfo_t info = e->get_var(name);
  if (info)
  {
    if (info->type == ATTRIBUTE)
    {
      // Check Attributes.
      emit_store(ACC, info->index + DEFAULT_OBJFIELDS, SELF, s);  // sw $a0 12($s0) ; Store result of expression in self object.
      if (cgen_Memmgr)
      {
        emit_addiu(A1, SELF, 4 * (info->index + DEFAULT_OBJFIELDS), s); // addiu $a1 $s0 12
        emit_gc_assign(s);         
      }
    }
    else if (info->type == FORMAL)
    {
      // Check Formal parameters.
      emit_store(ACC, info->index /* + DEFAULT_OBJFIELDS */, FP, s);
    }
    else if (info->type == LOCAL)
    {
      // Check Local Variables.
      emit_store(ACC, stack_index - info->index, SP, s);
    }
  }
}

void static_dispatch_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  // TODO:
  Symbol class_t;
  int f_jump_label = label_counter++;
  std::string type_disp_tab = type_name->get_string();
  type_disp_tab += DISPTAB_SUFFIX;

  int prev_stack_index = stack_index;
  
  // Evaluate formals and push them to stack.
  for (int i = actual->first(); actual->more(i); i = actual->next(i))
  {
    actual->nth(i)->code(s, e);
    emit_push(ACC, s);
    stack_index++;
  }

  // Evaluate expr to get class object.
  expr->code(s, e);

  class_t = type_name;
  if (class_t == SELF_TYPE)
  {
    class_t = e->get_class()->get_name();
  }

  #if TEMP_DEBUG
    cout << class_t << endl;
  #endif

  // Find method name in dispatch table and get index.
  int method_index = 0;
  cinfo_t info = class_tab[class_t]->get_cinfo();
  std::map<Symbol, Symbol>::iterator it;

  for (unsigned int i = 0; i < info->method_stack.size(); i++)
  {
    if (info->method_stack[i] == name)
    {
      #if TEMP_DEBUG
        cout << "Index: " << method_index << endl;
        cout << it->second << endl;
      #endif
      break;
    }

    method_index++;
  }


  emit_bne(ACC, ZERO, f_jump_label, s);                       //   bne $a0 $zero labeli ; Dispatch if correct object from expr i.e. it's not void.
  emit_load_address(ACC, "str_const0", s);                    //   la  $a0 str_const0   ; Load class name.
  emit_load_imm(T1, get_line_number(), s);                    //   li  $t1 10           ; Unsure about imm value. TODO.
  emit_jal("_dispatch_abort", s);                             //   jal _dispatch_abort  ; Jump to abort current dispatch
  emit_label_def(f_jump_label, s);                            // labeli:
  emit_load_address(T1, (char *) type_disp_tab.c_str(), s);  // la $a0 <type_name>_protObj
  emit_load(T1, method_index, T1, s);                         //   lw  $t1 12($t1)      ; Load function from dispatch table.
  emit_jalr(T1, s);                                           //   jalr    $t1          ; Jump to function.

  // Remove the count of the number of added actuals.
  for (int i = actual->first(); actual->more(i); i = actual->next(i))
  {
    stack_index--;
  }
}

void dispatch_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  Symbol class_t;
  int f_jump_label = label_counter++;

  int prev_stack_index = stack_index;
  // Evaluate formals and push them to stack.
  for (int i = actual->first(); actual->more(i); i = actual->next(i))
  {
    actual->nth(i)->code(s, e);
    emit_push(ACC, s);
    stack_index++;
  }


  // Evaluate expr to get class object.
  expr->code(s, e);

  class_t = expr->get_type();
  if (expr->get_type() == SELF_TYPE)
  {
    class_t = e->get_class()->get_name();
  }

  // Find method name in dispatch table and get index.
  int method_index = 0;
  cinfo_t info = class_tab[class_t]->get_cinfo();
  std::map<Symbol, Symbol>::iterator it;

  for (unsigned int i = 0; i < info->method_stack.size(); i++)
  {
    if (info->method_stack[i] == name)
    {
      #if TEMP_DEBUG
        cout << "Index: " << method_index << endl;
        cout << it->second << endl;
      #endif
      break;
    }

    method_index++;
  }

  s << "#" << class_t << ":" << name << ":" << method_index  << ":" << stack_index << endl;
  emit_bne(ACC, ZERO, f_jump_label, s);             //   bne $a0 $zero labeli ; Dispatch if correct object from expr i.e. it's not void.
  emit_load_address(ACC, "str_const0", s);          //   la  $a0 str_const0   ; Load class name.
  emit_load_imm(T1, get_line_number(), s);          //   li  $t1 10           ; Unsure about imm value. TODO.
  emit_jal("_dispatch_abort", s);                   //   jal _dispatch_abort  ; Jump to abort current dispatch
  emit_label_def(f_jump_label, s);                  // labeli:
  emit_load(T1, 2, ACC, s);                         //   lw  $t1 8($a0)       ; Get dispatch table.
  emit_load(T1, method_index, T1, s);               //   lw  $t1 12($t1)      ; Load function from dispatch table.
  emit_jalr(T1, s);                                 //   jalr    $t1          ; Jump to function.

  // Remove the count of the number of added actuals.
  for (int i = actual->first(); actual->more(i); i = actual->next(i))
  {
    stack_index--;
  }
}

void cond_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  int else_label = label_counter++;
  int end_label = label_counter++;

  pred->code(s, e);
  emit_fetch_int(T1, ACC, s);                 // lw  $t1 12($a0)    ; T1 <- MEM[A0] + 12 (Because int val. after 3 fields.)
  emit_beqz(T1, else_label, s);               // beqz  $t1 label0   ; If false, go to else branch
  then_exp->code(s, e);                       // la  $a0 int_const0 ; Else continue with then branch (ACC saved after then)
  emit_branch(end_label, s);                  // b label1           ; goto end
  emit_label_def(else_label, s);              // label0:            ; else branch begin
  else_exp->code(s, e);                       //    else            ; else branch (ACC saved after else)
  emit_label_def(end_label, s);               // label1:            ; end
}

void loop_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  int loop_label = label_counter++;
  int pool_label = label_counter++;

  emit_label_def(loop_label, s);            // label0:
  pred->code(s, e);                         //    Evaluate predicate
  emit_load(T1, 3, ACC, s);                 //    lw  $t1 12($a0)       ; Get the Int from the Bool
  emit_beq(T1, ZERO, pool_label, s);        //    beq $t1 $zero label1  ; Check if we need to go to end
  body->code(s, e);                         //    Evaluate body
  emit_branch(loop_label, s);               //    b   label0            ; Branch to begining because loop is not over yet
  emit_label_def(pool_label, s);            // label1:
  emit_move(ACC, ZERO, s);                  //    move $a0 $zero        ; While loop  returns nothing.
}

bool type_sort_function(Case c1, Case c2)
{
  CgenNodeP class_1 = class_tab[c1->get_type_decl()];
  CgenNodeP class_2 = class_tab[c2->get_type_decl()];

  return class_1->get_cinfo()->class_tag > class_2->get_cinfo()->class_tag;
}

void typcase_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  // TODO:

  int non_void_label = label_counter++;
  int case_abort_label = 0;

  expr->code(s, e);                               // Evaluate input to case.

  emit_bne(ACC, ZERO, label_counter, s);          // bne $a0 $zero labelk ; Verify the result of the expression is not NULL. Branch to first case if it isn't.
  emit_load_address(ACC, "str_const0", s);        // la  $a0 str_const0   ; If it is, we need to abort the case expression evaluation.
  emit_load_imm(T1, get_line_number(), s);        // li  $t1 line_no      ; Get line number.
  emit_jal("_case_abort2", s);                    // jal _case_abort2     ; Abort.

  std::vector<int> case_labels;

  for (int i = cases->first(); cases->more(i); i = cases->next(i))
  {
    case_labels.push_back(label_counter);
    label_counter++;
  }

  case_abort_label = label_counter;
  case_labels.push_back(case_abort_label);
  label_counter++;

  int j = 0;

  std::vector<Case> cases_;

  for (int i = cases->first(); cases->more(i); i = cases->next(i))
  {
    cases_.push_back(cases->nth(i));
  }

  std::sort (cases_.begin(), cases_.end(), type_sort_function);

  for (unsigned int i = 0; i < cases_.size(); i++)
  {
    emit_label_def(case_labels[i], s);              // labeli:

    if (j == 0)
    {
      emit_load(T2, 0, ACC, s);             //    lw  $t2 0($a0)
      j = 1;
    }

    cases_[i]->code(s, e, case_labels[i + 1]);    // code the case branch

    emit_branch(non_void_label, s);               // b labelkbranch to exit when done.
  }

  emit_label_def(case_abort_label, s);            // labelj:
  emit_jal("_case_abort", s);                      //    jal _case_abort
  emit_label_def(non_void_label, s);              // labelk:
}

CgenNodeP get_smallest_child(CgenNodeP class_)
{
  return NULL;
}

void branch_class::code(ostream &s, EnvP e, int next_case_label) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  // TODO:

  CgenNodeP class_ = class_tab[type_decl];
  cinfo_t cinfo = class_->get_cinfo();

  emit_blti(T2, cinfo->class_tag, next_case_label, s);      // blt $t2 <class tag> <next case>
  emit_bgti(T2, cinfo->class_max_tag, next_case_label, s);  // bgt $t2 <max child class tag> <next case>

  // Enter a new scope tp add variable.
  e->enter_scope();

  // Add identitfier to environment.
  ainfo_t ainfo = new struct attr_info;
  ainfo->index = stack_index;
  ainfo->type = LOCAL;
  ainfo->formal = NULL;
  ainfo->attr = NULL;
  e->add_var(name, ainfo);

  emit_push(ACC, s);                                        // Push result of accumulator to stack for later use.
  stack_index++;

  // Evaluate expression here.
  expr->code(s, e);

  emit_addiu(SP, SP, 4, s);                                 // Pop accumulator from the stack.
  stack_index--;

  // We are done with the variables in this scope so we can remove them.
  e->exit_scope();
}

void block_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  
  // Iterate over all the expressions of the block
  for (int i = body->first(); body->more(i); i = body->next(i))
  {
    body->nth(i)->code(s, e);
  }
}

void let_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  e->enter_scope();                     // Enter a new scope to add variables to environment.

  #if TEMP_DEBUG
    cout << "ID: " << identifier << endl;
    cout << "stack_index: " << stack_index << endl;
    cout << "Type: " << ainfo->type << endl;
    cout << "Index: " << ainfo->index << endl;
  #endif

  // Evaluate init expression for variable.
  if (init->is_no_expr())
  {
    if (type_decl == Int)
    {
      emit_load_int(ACC, inttable.lookup_string("0"), s);
    }
    else if (type_decl == Str)
    {
      emit_load_string(ACC, stringtable.lookup_string(""), s);
    }
    else if (type_decl == Bool)
    {
      emit_load_bool(ACC, falsebool, s);
    }
    else
    {
      init->code(s, e);
    }
  }
  else
  {
    init->code(s, e);
  }

  // Add variable to environment.
  ainfo_t ainfo = new struct attr_info;
  ainfo->index = stack_index;
  ainfo->type = LOCAL;
  ainfo->formal = NULL;
  ainfo->attr = NULL;
  e->add_var(identifier, ainfo);

  emit_push(ACC, s);                    // Push variable to stack.
  stack_index++;
  body->code(s, e);                     // Evaluate body
  emit_addiu(SP, SP, 4, s);             // Pop variable from stack because we're exiting scope.
  stack_index--;

  e->exit_scope();                      // Exit the scope because we're done with the variables.
}

void plus_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  // Evaluate E1
  e1->code(s, e);

  // Push Result from E1 (ACC) to Stack
  emit_push(ACC, s);
  stack_index++;

  // Evaluate E2
  e2->code(s, e);
  emit_jal("Object.copy", s);

  // Return Stack to same place
  emit_addiu(SP, SP, 4, s);
  stack_index--;

  // Load first Int from Stack into T1
  emit_load(T1, 0, SP, s);
  emit_load(T1, 3, T1, s);

  // Get Int from accumulator into T2
  emit_load(T2, 3, ACC, s);

  // T1 <- T1 + T2
  emit_add(T1, T1, T2, s);

  // Store Int into Object pointed to by ACC.
  emit_store(T1, 3, ACC, s);
}

void sub_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  // Evaluate E1
  e1->code(s, e);

  // Push Result from E1 (ACC) to Stack
  emit_push(ACC, s);
  stack_index++;

  // Evaluate E2
  e2->code(s, e);
  emit_jal("Object.copy", s);

  // Return Stack to same place
  emit_addiu(SP, SP, 4, s);
  stack_index--;

  // Load first Int from Stack into T1
  emit_load(T1, 0, SP, s);
  emit_load(T1, 3, T1, s);

  // Get Int from accumulator into T2
  emit_load(T2, 3, ACC, s);

  // T1 <- T1 - T2
  emit_sub(T1, T1, T2, s);

  // Store Int into Object pointed to by ACC.
  emit_store(T1, 3, ACC, s);
}

void mul_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  // Evaluate E1
  e1->code(s, e);

  // Push Result from E1 (ACC) to Stack
  emit_push(ACC, s);
  stack_index++;

  // Evaluate E2
  e2->code(s, e);
  emit_jal("Object.copy", s);

  // Return Stack to same place
  emit_addiu(SP, SP, 4, s);
  stack_index--;

  // Load first Int from Stack into T1
  emit_load(T1, 0, SP, s);
  emit_load(T1, 3, T1, s);

  // Get Int from accumulator into T2
  emit_load(T2, 3, ACC, s);

  // T1 <- T1 * T2
  emit_mul(T1, T1, T2, s);

  // Store Int into Object pointed to by ACC.
  emit_store(T1, 3, ACC, s);
}

void divide_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  // TODO: (Could also add divide by 0 exception here)

  // Evaluate E1
  e1->code(s, e);

  // Push Result from E1 (ACC) to Stack
  emit_push(ACC, s);
  stack_index++;

  // Evaluate E2
  e2->code(s, e);
  emit_jal("Object.copy", s);

  // Return Stack to same place
  emit_addiu(SP, SP, 4, s);
  stack_index--;

  // Load first Int from Stack into T1
  emit_load(T1, 0, SP, s);
  emit_load(T1, 3, T1, s);

  // Get Int from accumulator into T2
  emit_load(T2, 3, ACC, s);

  // T1 <- T1 / T2
  emit_div(T3, T1, T2, s);

  // Store Int into Object pointed to by ACC.
  emit_store(T3, 3, ACC, s);
}

void neg_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  // TODO:

  // Evaluate E1
  e1->code(s, e);
  emit_jal("Object.copy", s); // jal Object.copy ; Copy object.
  emit_load(T1, 3, ACC, s);   // lw  $t1 12($a0) ; Load int value from ACC to T1.
  emit_neg(T1, T1, s);        // neg $t1 $t1     ; Negate.
  emit_store(T1, 3, ACC, s);  // sw $t1 12($a0)  ; Optimization. Store it in the same Int Object.
}

void lt_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  e1->code(s, e);                       // Evaluate E1
  emit_push(ACC, s);                    // Push ACC to Stack. Temporaries might be reused by other code.
  stack_index++;
  e2->code(s, e);                       // Evaluate E2
  emit_fetch_int(T2, ACC, s);           // lw  $t2 12($a0)     ; Load int value from ACC to T2.
  emit_addiu(SP, SP, 4, s);             // Increase stack pointer to point to pushed value.
  stack_index--;
  emit_load(T1, 0, SP, s);              // Load back pushed value from stack to T1.
  emit_fetch_int(T1, T1, s);            // Fetch the integer from the integer object to T1.
  emit_load_bool(ACC, truebool, s);     // la  $a0 bool_const1 ; Assume T1 < T2 i.e. set ACC <- TRUE.
  emit_blt(T1, T2, label_counter, s);   // blt $t1 $t2 label0  ; if T1 < T2, then goto end.
  emit_load_bool(ACC, falsebool, s);    // la  $a0 bool_const0 ; Otherwise set ACC <- FALSE.
  emit_label_def(label_counter, s);     // label0:             ; end

  // Increment label counter
  label_counter++;
}

void eq_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  // TODO:

  e1->code(s, e);                       // Evaluate E1.
  emit_push(ACC, s);                    // Push ACC to Stack. Temporaries might be reused by other code.
  stack_index++;
  e2->code(s, e);                       // Evaluate E2.
  emit_move(T2, ACC, s);                // lw  $t2 0($a0)     ; Load int value from ACC to T2.
  emit_addiu(SP, SP, 4, s);             // Increase stack pointer to point to pushed value.
  stack_index--;
  emit_load(T1, 0, SP, s);              // Load back pushed value from stack to T1.
  emit_load_bool(ACC, truebool, s);     // la  $a0 bool_const1
  emit_beq(T1, T2, label_counter, s);   // beq $t1 $t2 labeli
  emit_load_bool(A1, falsebool, s);     // la  $a1 bool_const0 TODO: check if ACC can be used.
  emit_jal("equality_test", s);         // jal equality_test
  emit_label_def(label_counter, s);     // labeli

  // Increment label counter
  label_counter++;
}

void leq_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  e1->code(s, e);                          // Evaluate E1
  emit_push(ACC, s);                    // Push ACC to Stack. Temporaries might be reused by other code.
  stack_index++;
  e2->code(s, e);                          // Evaluate E2
  emit_fetch_int(T2, ACC, s);           // lw  $t2 12($a0)     ; Load int value from ACC to T2.
  emit_addiu(SP, SP, 4, s);             // Increase stack pointer to point to pushed value.
  stack_index--;
  emit_load(T1, 0, SP, s);              // Load back pushed value from stack to T1.
  emit_fetch_int(T1, T1, s);            // Fetch the integer from the integer object to T1.
  emit_load_bool(ACC, truebool, s);     // la  $a0 bool_const1 ; Assume T1 < T2 i.e. set ACC <- TRUE.
  emit_bleq(T1, T2, label_counter, s);  // blt $t1 $t2 label0  ; if T1 < T2, then goto end.
  emit_load_bool(ACC, falsebool, s);    // la  $a0 bool_const0 ; Otherwise set ACC <- FALSE.
  emit_label_def(label_counter, s);     // label0:             ; end

  // Increment label counter
  label_counter++;
}

void comp_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  e1->code(s, e);           // Evaluate E1.

  emit_fetch_int(T1, ACC, s);           // lw  $t1 12($a0)
  emit_load_bool(ACC, truebool, s);     // la  $a0 bool_const1
  emit_beqz(T1, label_counter, s);      // beqz  $t1 label0
  emit_load_bool(ACC, falsebool, s);    // la  $a0 bool_const0
  emit_label_def(label_counter, s);     // label0:

  // Increment label counter
  label_counter++;  
}

void int_const_class::code(ostream& s, EnvP e)  
{
  //
  // Need to be sure we have an IntEntry *, not an arbitrary Symbol
  //
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  emit_load_int(ACC,inttable.lookup_string(token->get_string()),s);
}

void string_const_class::code(ostream& s, EnvP e)
{
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  emit_load_string(ACC,stringtable.lookup_string(token->get_string()),s);
}

void bool_const_class::code(ostream& s, EnvP e)
{
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif
  emit_load_bool(ACC, BoolConst(val), s);
}

void new__class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  if (type_name != SELF_TYPE)
  {
    Symbol expr_type = get_type();
    std::string type_prot_obj = expr_type->get_string();
    type_prot_obj += PROTOBJ_SUFFIX;
    std::string type_init = expr_type->get_string();
    type_init += CLASSINIT_SUFFIX;

    emit_load_address(ACC, (char *) type_prot_obj.c_str(), s);  // la $a0 <type_name>_protObj
    emit_jal("Object.copy", s);                                 // jal Object.copy
    emit_jal((char *) type_init.c_str(), s);                    // <type_name>_init
  }
  else
  {
    emit_load_address(T1, CLASSOBJTAB, s);                      // la  $t1 class_objTab
    emit_load(T2, 0, SELF, s);                                  // lw  $t2 0($s0)
    emit_sll(T2, T2, 3, s);                                     // sll $t2 $t2 3
    emit_addu(T1, T1, T2, s);                                   // addu  $t1 $t1 $t2
    emit_push(T1, s);                                           // move  $s1 $t1 -- push
    emit_load(ACC, 0, T1, s);                                   // lw  $a0 0($t1)
    emit_jal("Object.copy", s);                                 // jal Object.copy
    emit_load(T1, 1, SP, s);                                    // lw  $t1 4($s0)
    emit_addiu(SP, SP, 4, s);                                   // pop
    emit_load(T1, 1, T1, s);
    emit_jalr(T1, s);                                           // jalr    $t1
  }
}

void isvoid_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  // Evaluate E1
  e1->code(s, e);

  emit_move(T1, ACC, s);                // move  $t1 $a0        ; T1 <- ACC
  emit_load_bool(ACC, truebool, s);     // la  $a0 bool_const1  ; A0 <- TRUE
  emit_beqz(T1, label_counter, s);      // beqz  $t1 labeli     ; if T1 == TRUE, goto labeli
  emit_load_bool(ACC, falsebool, s);    // la  $a0 bool_const0  ; else ACC <- FALSE
  emit_label_def(label_counter, s);     // labeli:              ; labeli

  // Increment label counter
  label_counter++;
}

void no_expr_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  emit_move(ACC, ZERO, s);
}

void object_class::code(ostream &s, EnvP e) {
  #if TEMP_DEBUG
    cout << typeid(*this).name() << "::" << __func__ << endl;
  #endif

  // SELF_TYPE
  if (name == self)
  {
    emit_move(ACC, SELF, s);  // move $a0 $s0
    return;
  }

  ainfo_t info = e->get_var(name);
  if (info)
  {
    #if TEMP_DEBUG
      cout << "VAR: " << name << endl;
      cout << "stack_index: " << stack_index << endl;
      cout << "Type: " << info->type << endl;
      cout << "Index: " << info->index << endl;
    #endif

    if (info->type == ATTRIBUTE)
    {
      // Check Attributes.
      emit_load(ACC, info->index + DEFAULT_OBJFIELDS, SELF, s);  // sw $a0 12($s0) ; Store result of expression in self object.
    }
    else if (info->type == FORMAL)
    {
      // Check Formal parameters.
      emit_load(ACC, info->index /* + DEFAULT_OBJFIELDS */, FP, s);
    }
    else if (info->type == LOCAL)
    {
      // Check Local Variables.
      emit_load(ACC, stack_index - info->index, SP, s);
    }
  }
}
