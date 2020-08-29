#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stack>
#include <set>
#include "semant.h"
#include "utilities.h"

extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
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

ClassTable *classtable;
Symbol curr_class_name;
Symbol curr_file_name;

void ClassTable::print_ig()
{
    #if TEMP_DEBUG
        std::map<Symbol, Class_>::iterator it;

        cout << "Inheritance Graph: " << endl;
        for (it = inheritance_graph.begin(); it != inheritance_graph.end(); ++it)
        {
            if (it->second->get_parent() == Object)
            {
                cout << it->first << " : " << "Object" << endl;
            }
            else if (inheritance_graph.find(it->second->get_parent()) != inheritance_graph.end())
            {
                cout << it->first << " : " << inheritance_graph.find(it->second->get_parent())->first << endl;
            }
        }
        cout << endl << endl;
    #endif
}

void ClassTable::print_symbol_table()
{
    #if TEMP_DEBUG
        // std::map<Symbol, SymbolTable<Symbol, Feature> >::iterator it;

        // cout << "Symbol Table: " << endl;
        // for (it = attribute_table.begin(); it != attribute_table.end(); ++it)
        // {
        //     cout << "Class " << it->first << " attributes: " << endl;
        //     it->second.dump();
        //     cout << endl << endl;
        // }

        // cout << "Symbol Table: " << endl;
        // for (it = symbol_table.begin(); it != symbol_table.end(); ++it)
        // {
        //     cout << "Class " << it->first << " features: " << endl;
        //     it->second.dump();
        //     cout << endl << endl;
        // }
    #endif
}

/* Enter a new scope.
 */
void ClassTable::enter_scope(Symbol class_name)
{
    attribute_table[class_name].enterscope();
}

/* Create a new scope and add an attribute to the class.
 */
void ClassTable::add_attribute(Symbol class_name, Symbol name, Symbol type_decl, Expression init)
{
    attr_class * a = new attr_class(copy_Symbol(name), copy_Symbol(type_decl), init->copy_Expression());

    attribute_table[class_name].addid(name, ((Feature) a));
}

/* Remove the most recent scope from the class.
 */
void ClassTable::remove(Symbol class_name)
{
    attribute_table[class_name].exitscope();
}

/* Find a method in a class or in the superclass.
 */
Feature ClassTable::find_method(Symbol method_name, Symbol class_name)
{
    Feature f;
    Symbol c = class_name;

    if (c == SELF_TYPE)
    {
        c = curr_class_name;
    }

    while (c != No_class)
    {
        #if TEMP_DEBUG
            cout << c << endl;
        #endif

        if ((f = symbol_table[c].lookup(method_name)))
        {
            return f;
        }

        c = inheritance_graph[c]->get_parent();
    }

    return NULL;
}

/* Get class by class name.
 */
Class_ ClassTable::get_class(Symbol class_name)
{
    return inheritance_graph[class_name];
}

/*
 * Check if a class a subclass of base class.
 */
bool ClassTable::does_conform(Symbol base, Symbol class_type)
{
    if (base == class_type)
    {
        return true;
    }

    if (base == SELF_TYPE)
    {
        return false;
    }

    if (class_type == SELF_TYPE)
    {
        class_type = curr_class_name;
    }

    std::stack<Symbol> st;

    get_inheritance_list(class_type, st);

    while (!st.empty())
    {
        if (st.top() == base)
        {
            return true;
        }

        st.pop();
    }

    return false;
}

/* Verify whether a particular class exists.
 */
Symbol ClassTable::find_class_type(Symbol class_name)
{
    if (class_name == SELF_TYPE)
    {
        return SELF_TYPE;
    }

    if (inheritance_graph.find(class_name) != inheritance_graph.end())
    {
        return class_name;
    }

    return No_class;
}

/* Find type for a given attribute.
 */
Symbol ClassTable::find_type(Symbol name, Symbol class_name)
{
    if (name == self)
    {
        return SELF_TYPE;
    }

    if (class_name == Object)
    {
        return No_type;
    }

    Symbol attribute_type = No_type;
    Feature f = attribute_table[class_name].lookup(name);
    attr_class * attribute = ((attr_class *) f);

    #if TEMP_DEBUG
        attribute_table[class_name].dump();
    #endif

    if (!attribute)
    {
        attribute_type = find_type(name, inheritance_graph[class_name]->get_parent());
    }
    else
    {
        attribute_type = attribute->get_type_decl();
    }

    return attribute_type;
}

/* Get inheritance list.
 */
void ClassTable::get_inheritance_list(Symbol class_name, std::stack<Symbol>& st)
{
    #if TEMP_DEBUG
        cout << "IG graph for: " << class_name << endl << endl;
    #endif
    
    if (class_name == No_type)
    {
        return;
    }

    Symbol c = class_name;

    if (c == SELF_TYPE)
    {
        c = curr_class_name;
    }

    while (c != No_class)
    {
        #if TEMP_DEBUG
            cout << c << endl;
        #endif
        st.push(c);
        c = inheritance_graph[c]->get_parent();
    }
}

/* Least upper bound function.
 */
Symbol ClassTable::lub(Symbol t1, Symbol t2)
{
    if (t1 == No_type)
    {
        #if TEMP_DEBUG
            cout << "Got lub type for " << t1 << " and " << t2 << " : " << t1 << endl;
        #endif
        return t2;
    }
    else if (t2 == No_type)
    {
        #if TEMP_DEBUG
            cout << "Got lub type for " << t1 << " and " << t2 << " : " << t2 << endl;
        #endif
        return t1;
    }
    else if (t2 == t1 && t1 == SELF_TYPE)
    {
        #if TEMP_DEBUG
            cout << "Got lub type for " << t1 << " and " << t2 << " : " << t2 << endl;
        #endif
        return t1;
    }

    Symbol lub_type = No_type;
    std::stack<Symbol> s1;
    std::stack<Symbol> s2;

    get_inheritance_list(t1, s1);
    get_inheritance_list(t2, s2);

    while (!s1.empty() && !s2.empty())
    {
        if (s2.top() != s1.top())
        {
            break;
        }

        lub_type = s1.top();

        s1.pop();
        s2.pop();
    }

    #if TEMP_DEBUG
        cout << "Got lub type for " << t1 << " and " << t2 << " : " << lub_type << endl;
    #endif

    return lub_type;
}

/* Check whether the Inheritance Graph has cycles on not.
 */
void ClassTable::check_cycle_ig()
{
    bool done = false;
    Symbol class_name;
    Symbol class_parent;

    std::map<Symbol, Class_>::iterator it;
    std::map<Symbol, Class_>::iterator search_it;

    // Run DFS on every class node
    for (it = inheritance_graph.begin(); it != inheritance_graph.end(); ++it)
    {
        class_name = it->first;

        if (class_name == Object)
        {
            continue;
        }

        done = false;

        // Run DFS to check for cycles
        while (!done)
        {
            class_parent = inheritance_graph[class_name]->get_parent();
            search_it = inheritance_graph.find(class_parent);

            // Classes can't inherit from basic types.
            if (class_parent == Int ||
                class_parent == Str ||
                class_parent == Bool ||
                class_parent == SELF_TYPE)
            {
                semant_error(it->second) << "Class " << class_name << " cannot inherit class " << class_parent << "." << endl;
                break;
            }

            if (class_parent == Object)
            {
                done = true;
                break;
            }
            else if (search_it == inheritance_graph.end())
            {
                // Parent class not defined.
                semant_error(it->second) << "Class " << class_name << " inherits from an undefined class " << class_parent << "." << endl;
                done = true;
            }
            else if (class_parent == it->first)
            {
                // If we come back to the first class, cyclic inheritance.
                semant_error(it->second) << "Class " << it->first << ", or an ancestor of " << it->first << ", is involved in an inheritance cycle." << endl;
                done = true;
            }
            else
            {
                class_name = class_parent;
            }
        }
    }
}

/* Add all classes to Map. The Map should look like:
 * Symbol -> Class_. */
void ClassTable::build_ig(Classes classes)
{
    Symbol class_name;

    /* Need this to add basic classes */
    install_basic_classes();

    for (int i = classes->first(); classes->more(i); i = classes->next(i))
    {
        Class_ curr = classes->nth(i);
        class_name = curr->get_name();

        /* Check if class already exists. If it does, throw error. */
        if ( inheritance_graph.find(class_name) != inheritance_graph.end() )
        {
            semant_error(curr) << "Class " << class_name << " was previously defined." << endl;
        }

        inheritance_graph[class_name] = curr;
    }
}

/* Check if:
 * 1. If formal list length is the same.
 * 2. If formals have the same types in the same order.
 */
bool ClassTable::compare_formals(Formals f1, Formals f2, Symbol m)
{
    if ((f1 == NULL) != (f2 == NULL))
    {
        return false;
    }

    int i = 0;
    int j = 0;
    int l1 = f1->len();
    int l2 = f2->len();

    if (l1 != l2)
    {
        semant_error(curr_file_name, f2) << "Incompatible number of formal parameters in redefined method " << m << "." << endl;
        return false;
    }

    for (i = f1->first(), j = f2->first();
        f1->more(i), f2->more(j);
        i = f1->next(i), j = f2->next(j))
    {
        Formal f_1 = f1->nth(i);
        Formal f_2 = f2->nth(j);

        if (f_1->get_type_decl() != f_2->get_type_decl())
        {
            semant_error(curr_file_name, f_2) << "In redefined method " << m << ", parameter type " << f_2->get_type_decl() << " is different from original type " << f_1->get_type_decl() << endl;
            return false;
        } 
    }

    return true;
}

/* 
 * Compare 2 methods. Check return type and formals.
 */
bool ClassTable::compare_methods(Feature m1, Feature m2)
{
    method_class m_1 = *((method_class *) m1);
    method_class m_2 = *((method_class *) m2);

    Formals f1 = m_1.get_formals();
    Formals f2 = m_2.get_formals();

    if (!compare_formals(f1, f2, m_2.get_name()))
    {
        return false;
    }

    if (m_1.get_return_type() != m_2.get_return_type())
    {
        semant_error(curr_file_name, m2) << "In redefined method " << m_2.get_name() << ", return type " << m_2.get_return_type() << " is different from original return type " << m_1.get_return_type() << "." << endl;
        return false;
    }
    
    return true;
}

/*
 * Check if a feature is redefined. Attribute redefinition not allowed at all.
 * However, method redefinition is allowed as long as the formals and the return
 * types are equal.
 */
bool ClassTable::redefined(Feature f, Symbol class_name)
{
    if (class_name == Object)
    {
        return false;
    }

    Class_ c = inheritance_graph[class_name];

    if (!f->is_method())
    {
        if (attribute_table[class_name].lookup(f->get_name()))
        {
            return true;
        }
        else
        {
            return redefined(f, c->get_parent());
        }
    }
    else
    {
        Feature lookup_f = symbol_table[class_name].lookup(f->get_name());
        if (lookup_f)
        {
            if (!compare_methods(lookup_f, f))
            {
                return true;
            }
        }

        return redefined(f, c->get_parent());
    }
}

/*
 * Check inherited methods and variables.
 */
void ClassTable::check_inherited_features()
{
    std::map<Symbol, Class_>::iterator it;

    std::stack<Symbol> st;

    /* Check inherited features */
    for (it = inheritance_graph.begin(); it != inheritance_graph.end(); ++it)
    {
        if (it->first == Object || 
            it->first == Int || 
            it->first == Str || 
            it->first == Bool || 
            it->first == IO)
        {
            continue;
        }

        Class_ curr_class = it->second;

        Features features = curr_class->get_features();

        curr_file_name = curr_class->get_filename();

        /* Iterate through all the features */
        for (int i = features->first(); features->more(i); i = features->next(i))
        {
            Feature curr_feature = features->nth(i);

            /* if (redefined(curr_feature, curr_class->get_parent()))
            {
                if (!curr_feature->is_method())
                {
                    semant_error(curr_file_name, curr_feature) << "Attribute " << curr_feature->get_name() << " is an attribute of an inherited class." << endl;
                }
            } */

            get_inheritance_list(curr_class->get_name(), st);

            while (!st.empty())
            {
                if (st.top() == curr_class->get_name())
                {
                    st.pop();
                    break;
                }

                if (!curr_feature->is_method())
                {
                    if (attribute_table[st.top()].lookup(curr_feature->get_name()))
                    {
                        semant_error(curr_file_name, curr_feature) << "Attribute " << curr_feature->get_name() << " is an attribute of an inherited class." << endl;
                        break;
                    }
                }
                else
                {
                    Feature lookup_f = symbol_table[st.top()].lookup(curr_feature->get_name());
                    if (lookup_f)
                    {
                        if (!compare_methods(lookup_f, curr_feature))
                        {
                            break;
                        }
                    }
                }

                st.pop();
            }

            while (!st.empty()) { st.pop(); }
        }
    }
}

/*
 * Generate first pass symbol table.
 */
void ClassTable::generate_symbol_table()
{
    std::map<Symbol, Class_>::iterator it;
    Feature f;

    for (it = inheritance_graph.begin(); it != inheritance_graph.end(); ++it)
    {
        Class_ curr_class = it->second;
        Features features = curr_class->get_features();

        SymbolTable<Symbol, Feature_class> class_symbol_table;
        SymbolTable<Symbol, Feature_class> class_attributes;

        class_symbol_table.enterscope();
        class_attributes.enterscope();

        /* Iterate through all the features */
        for (int i = features->first(); features->more(i); i = features->next(i))
        {
            Feature curr_feature = features->nth(i);
            f = curr_feature->copy_Feature();

            if (curr_feature->is_method())
            {
                /* Check for multiple definitions within same class */
                if (class_symbol_table.lookup(curr_feature->get_name()) != NULL)
                {
                    classtable->semant_error(curr_class->get_filename(), curr_feature) << "Method " << curr_feature->get_name() << " is multiply defined." << endl;
                }

                class_symbol_table.addid(curr_feature->get_name(), f);
            }
            else
            {
                if (curr_feature->get_name() == self)
                {
                    classtable->semant_error(curr_class->get_filename(), curr_feature) << "'self' cannot be the name of an attribute." << endl;   
                }
                else
                {
                    class_attributes.addid(curr_feature->get_name(), f);
                }
            }
        }

        if (curr_class->get_name() == Main && !class_symbol_table.lookup(main_meth))
        {
            classtable->semant_error(curr_class)  << "No 'main' method in class Main." << endl;
        }

        /* Add attributes and methods */
        symbol_table[curr_class->get_name()] = class_symbol_table;
        attribute_table[curr_class->get_name()] = class_attributes;

        class_attributes.exitscope();
        class_symbol_table.exitscope();
    }
}

/*
 * Evaluate the whole program.
 */
void ClassTable::evaluate_program()
{
    std::map<Symbol, Class_>::iterator it;
    for (it = inheritance_graph.begin(); it != inheritance_graph.end(); ++it)
    {
        if (it->first == Object || 
            it->first == Int || 
            it->first == Str || 
            it->first == Bool || 
            it->first == IO)
        {
            continue;
        }

        Class_ curr_class = it->second;
        curr_class_name = curr_class->get_name();
        Features features = curr_class->get_features();
        for (int i = features->first(); features->more(i); i = features->next(i))
        {
            Feature curr_feature = features->nth(i);
            curr_feature->eval();
        }
    }
}

void ClassTable::create_symbol_table()
{
    generate_symbol_table();

    check_inherited_features();

    evaluate_program();

    #if TRUE_DEBUG
        print_symbol_table();
    #endif
}

/* Feature evaluation */
/* Evaluate Attributes. */
void attr_class::eval()
{
    #if TEMP_DEBUG
        cout << "Evaluating: " << name << endl;
    #endif

    Symbol init_t;

    // Check expression
    init_t = init->eval_expr();
    if (init_t != No_type && !(classtable->does_conform(type_decl, init_t)))
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Inferred type " << init_t << " of initialization of attribute " 
            << name << " does not conform to declared type " << type_decl << "." << endl;
    }
}

/* Evaluate method. */
void method_class::eval()
{
    #if TEMP_DEBUG
        cout << "Evaluating: " << name << " for Class: " << curr_class_name << endl;
    #endif

    Symbol expr_type = No_type;
    Symbol formal_name;
    Symbol formal_type_decl;
    std::set<Symbol> formal_names;

    // Check return type
    if (return_type != SELF_TYPE && classtable->find_class_type(return_type) == No_class)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Undefined return type " << return_type << " in method " << name << "." << endl;
    }

    // Enter a new scope
    classtable->enter_scope(curr_class_name);

    // Add formals to attribute list. Verify whether formals are redeclared.
    for (int i = formals->first(); formals->more(i); i = formals->next(i))
    {
        Formal formal = formals->nth(i);
        formal_name = formal->get_name();
        formal_type_decl = formal->get_type_decl();

        // Check if formals are being redeclared.
        if (formal_names.find(formal_name) != formal_names.end())
        {
            classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), formal) << "Formal parameter " << formal_name << " is multiply defined." << endl;
            continue;
        }

        formal_names.insert(formal_name);

        // Check if formal types have been declared correctly.
        if (classtable->find_class_type(formal_type_decl) == No_class)
        {
            classtable->semant_error(classtable->get_class(curr_class_name)) << "Class " << formal_type_decl << " of formal parameter " << formal_name << " is undefined." << endl;
            continue;
        }

        classtable->add_attribute(curr_class_name, formal_name, formal_type_decl, no_expr());
    }

    // Evaluate expression type.
    expr_type = expr->eval_expr();

    if (!(classtable->does_conform(return_type, expr_type)))
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Inferred return type " << expr_type << " of method " << name << " does not conform to declared return type " << return_type << "." << endl;
    }

    // Exit the current scope because we finished type-checking the method.
    classtable->remove(curr_class_name);
    formal_names.clear();

    return;
}

/* Expression evaluation - All the eval_expr needs to be completed. */

/* Assignment */
Symbol assign_class::eval_expr()
{
    Symbol expr_t = expr->eval_expr();
    Symbol name_t = classtable->find_type(name, curr_class_name);

    if (!(classtable->does_conform(name_t, expr_t)))
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), expr) << "Type " << expr_t << " of assigned expression does not conform to declared type " 
            << name_t << " of identifier " << name << "." << endl;
    }

    set_type(expr_t);

    return expr_t;
}

/* Static Dispatch */
Symbol static_dispatch_class::eval_expr()
{
    Symbol class_t = No_type;
    Symbol f_type = No_type;
    Symbol a_type = No_type;
    Symbol f_name = No_type;
    method_class * m = NULL;
    Formals m_formals = NULL;

    // Evaluate expression and find class.
    class_t = expr->eval_expr();
    if (class_t == SELF_TYPE)
    {
        class_t = curr_class_name;
    }

    // Check if static type is self type.
    if (type_name == SELF_TYPE)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)) << "Static dispatch to SELF_TYPE." << endl;
        set_type(Object);
        return Object;
    }

    // Check if the type of the class conforms to expression type.
    if (!(classtable->does_conform(type_name, class_t)))
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Expression type " << class_t << " does not conform to declared static dispatch type " << type_name << "." << endl;
        set_type(Object);
        return Object;
    }

    // Find method.
    Feature f = classtable->find_method(name, type_name);
    if (!f)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), ((method_class *) f)) << "Dispatch to undefined method " << name << "." << endl;
        set_type(Object);
        return Object;
    }

    // Typecast feature to method and get the formal parameters.
    m = ((method_class *) f);
    m_formals = m->get_formals();

    // Compare the number of formals and actuals.
    if (m_formals->len() != actual->len())
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), actual) << "Method " << name << " called with wrong number of arguments." << endl;
    }

    // Compare formals and actuals.
    for (int i = actual->first(); actual->more(i); i = actual->next(i))
    {
        a_type = actual->nth(i)->eval_expr();

        // Check whether the formal exists or not.
        if (!m_formals->more(i))
        {
            break;
        }

        f_type = m_formals->nth(i)->get_type_decl();
        f_name = m_formals->nth(i)->get_name();
        if (!classtable->does_conform(f_type, a_type))
        {
            classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), actual->nth(i)) << "In call of method "<< name << ", type " << a_type << " of parameter " 
                << f_name << " does not conform to declared type " << f_type << "." << endl;
        }
    }

    /* We can set the return type now.
     * If the return type is self type, it has to be the
     * return type of the expression - From the COOL manual.
     */
    Symbol t = m->get_return_type();
    if (t == SELF_TYPE)
    {
        t = class_t;
    }

    set_type(t);
    return get_type();
}

/* Dispatch */
Symbol dispatch_class::eval_expr()
{
    Symbol class_t = No_type;
    Symbol f_type = No_type;
    Symbol a_type = No_type;
    Symbol f_name = No_type;
    method_class * m = NULL;
    Formals m_formals = NULL;

    // Evaluate expression and find class.
    class_t = expr->eval_expr();

    // Find method.
    Feature f = classtable->find_method(name, class_t);
    if (!f)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Dispatch to undefined method " << name << "." << endl;
        set_type(Object);
        return Object;
    }

    // Typecast feature to method and get the formal parameters.
    m = ((method_class *) f);
    m_formals = m->get_formals();

    // Compare the number of formals and actuals.
    if (m_formals->len() != actual->len())
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), actual) << "Method " << name << " called with wrong number of arguments." << endl;
    }

    // Compare formals and actuals.
    for (int i = actual->first(); actual->more(i); i = actual->next(i))
    {
        a_type = actual->nth(i)->eval_expr();

        // Check whether the formal exists or not.
        if (!m_formals->more(i))
        {
            break;
        }

        f_type = m_formals->nth(i)->get_type_decl();
        f_name = m_formals->nth(i)->get_name();
        if (!classtable->does_conform(f_type, a_type))
        {
            classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), actual->nth(i)) << "In call of method "<< name << ", type " << a_type << " of parameter " 
                << f_name << " does not conform to declared type " << f_type << "." << endl;
        }
    }

    /* We can set the return type now.
     * If the return type is self type, it has to be the
     * return type of the expression - From the COOL manual.
     */
    Symbol t = m->get_return_type();
    if (t == SELF_TYPE)
    {
        t = class_t;
    }

    #if TEMP_DEBUG
        cout << "Type for dispatch: " << t << endl;
    #endif

    set_type(t);
    return get_type();
}

/* Conditional */
Symbol cond_class::eval_expr()
{
    Symbol pred_t = pred->eval_expr();
    Symbol then_t = then_exp->eval_expr();
    Symbol else_t = else_exp->eval_expr();
    Symbol if_type = No_type;

    if (pred_t != Bool)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), pred) << "Predicate of 'if' does not have type Bool." << endl;
    }

    if_type = classtable->lub(then_t, else_t);

    set_type(if_type); 

    #if TEMP_DEBUG
        cout << "If type: " << if_type << endl;
    #endif

    return if_type;
}

/* Loop */
Symbol loop_class::eval_expr()
{
    Symbol pred_t = pred->eval_expr();

    if (pred_t != Bool)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), pred) << "Loop condition does not have type Bool." << endl;
    }

    body->eval_expr();
    set_type(Object);
    return Object;
}

/* Case statement. */
Symbol typcase_class::eval_expr()
{
    Symbol expr_t = expr->eval_expr();
    Symbol branch_decl_t = No_type;
    Symbol branch_t = No_type;
    Symbol case_t = No_type;
    std::set<Symbol> branch_types;

    /* Evaluate all branches and keep getting the lub type. The lub type for
     * all the cases is going to be the type for the case statement.
     */
    for (int i = cases->first(); cases->more(i); i = cases->next(i))
    {
        branch_t = cases->nth(i)->eval_case();
        branch_decl_t = cases->nth(i)->get_type_decl();

        if (branch_types.find(branch_decl_t) != branch_types.end())
        {
            classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), cases->nth(i)) << "Duplicate branch " << branch_decl_t << " in case statement." << endl;
        }

        // Add declarations for further checking.
        branch_types.insert(branch_decl_t);

        // Keep getting the lub type for all branches.
        case_t = classtable->lub(branch_t, case_t);
    }

    set_type(case_t);
    branch_types.clear();

    return get_type();
}

/* Branch */
Symbol branch_class::eval_case()
{
    // Enter a scope.
    classtable->enter_scope(curr_class_name);

    if (name == self)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "'self' bound in 'case'." << endl;
    }

    if (type_decl == SELF_TYPE)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Identifier " << name << " declared with type " << type_decl << " in case branch." << endl;
    }

    // Add attribute because it is local to the branch.
    classtable->add_attribute(curr_class_name, name, type_decl, expr);

    // Evaluate branch expression to get type.
    Symbol t = expr->eval_expr();

    // Remove attribute, we don't need it.
    classtable->remove(curr_class_name);

    return t;
}

/* Block of statements, evaluate all */
Symbol block_class::eval_expr()
{
    Symbol e_t;

    for (int i = body->first(); body->more(i); i = body->next(i))
    {
        Expression e = body->nth(i);
        e_t = e->eval_expr();
    }

    set_type(e_t);
    return e_t;
}

/* Let class -- check carefully */
Symbol let_class::eval_expr()
{
    Symbol decl_t;
    Symbol init_t;
    Symbol body_t;

    // Check if identifier is self.
    if (identifier == self)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "'self' cannot be bound in a 'let' expression." << endl; 
    }

    decl_t = type_decl;

    /* Check if init expression type confirmss to declared type,
     * only if it's a let-init expression.
     */
    init_t = init->eval_expr();
    if (init_t != No_type && !(classtable->does_conform(decl_t, init_t)))
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), init) << "Inferred type " << init_t << " of initialization of " << identifier 
            << " does not conform to identifier's declared type " << decl_t << "." << endl;
    }

    // Enter scope so that the variable can be stored.
    classtable->enter_scope(curr_class_name);
    classtable->add_attribute(curr_class_name, identifier, decl_t, init);

    body_t = body->eval_expr();
    set_type(body_t);

    // Exit scope because now we're done evaluating body.
    classtable->remove(curr_class_name);

    return body_t;
}

/* Addition */
Symbol plus_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();
    Symbol e2_t = e2->eval_expr();

    if (e1_t != Int || e2_t != Int)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "non-Int arguments: " << e1_t << " + " << e2_t << endl;
    }

    set_type(Int);

    return get_type();
}

/* Subtraction */
Symbol sub_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();
    Symbol e2_t = e2->eval_expr();

    if (e1_t != Int || e2_t != Int)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "non-Int arguments: " << e1_t << " - " << e2_t << endl;
        set_type(Object);
    }

    set_type(Int);

    return get_type();
}

/* Multiply */
Symbol mul_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();
    Symbol e2_t = e2->eval_expr();

    if (e1_t != Int || e2_t != Int)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "non-Int arguments: " << e1_t << " * " << e2_t << endl;
        set_type(Object);
    }

    set_type(Int);

    return get_type();
}

/* Divide */
Symbol divide_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();
    Symbol e2_t = e2->eval_expr();

    if (e1_t != Int || e2_t != Int)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "non-Int arguments: " << e1_t << " / " << e2_t << endl;
        set_type(Object);
    }

    set_type(Int);

    return get_type();
}

/* Negation */
Symbol neg_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();

    if (e1_t != Int)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Argument of '~' has type " << e1_t << " instead of Int." << endl;
        set_type(Object);
    }

    set_type(Int);

    return Int;
}

/* Less than comparison */
Symbol lt_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();
    Symbol e2_t = e2->eval_expr();

    if (e1_t != Int || e2_t != Int)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "non-Int arguments: " << e1_t << " < " << e2_t << endl;
        set_type(Object);
    }

    set_type(Bool);

    return get_type();
}

/* Equal */
Symbol eq_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();
    Symbol e2_t = e2->eval_expr();

    if (((e1_t == Int) || (e1_t == Str) || (e1_t == Bool)) && 
        ((e2_t == Int) || (e2_t == Str) || (e2_t == Bool)) &&
        (e1_t != e2_t))
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Illegal comparison with a basic type." << endl;
    }

    set_type(Bool);

    return get_type();
}

/* Less than or Equal */
Symbol leq_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();
    Symbol e2_t = e2->eval_expr();

    if (e1_t != Int || e2_t != Int)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "non-Int arguments: " << e1_t << " <= " << e2_t << endl;
        set_type(Object);
    }

    set_type(Bool);

    return get_type();
}

/* Not */
Symbol comp_class::eval_expr()
{
    Symbol e1_t = e1->eval_expr();

    if (e1_t != Bool)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), e1) << "Argument of 'not' has type Int instead of Bool." << endl;
    }

    set_type(Bool);

    return get_type();
}

/* Integer constant */
Symbol int_const_class::eval_expr()
{
    set_type(Int);
    return Int;
}

/* Boolean constant */
Symbol bool_const_class::eval_expr()
{
    set_type(Bool);
    return Bool;
}

/* String constant */
Symbol string_const_class::eval_expr()
{
    set_type(Str);
    return Str;
}

/* New operator */
Symbol new__class::eval_expr()
{
    Symbol t = classtable->find_class_type(type_name);
    if (t == No_class)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "'new' used with undefined class " << type_name << "." << endl;
        t = Object;
    }

    set_type(t);

    return t;
}

/* Isvoid keyword */
Symbol isvoid_class::eval_expr()
{
    e1->eval_expr();
    set_type(Bool);
    return Bool;
}

/* No expression */
Symbol no_expr_class::eval_expr()
{
    set_type(No_type);
    return No_type;
}

/* Object class */
Symbol object_class::eval_expr()
{
    Symbol t = classtable->find_type(name, curr_class_name);
    set_type(t);
    if (t == No_type)
    {
        classtable->semant_error(classtable->get_class(curr_class_name)->get_filename(), this) << "Undeclared identifier " << name << "." << endl;
        set_type(Object);
    }

    return get_type();
}

ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    build_ig(classes);
    check_cycle_ig();

    // Check for Main class
    if (inheritance_graph.find(Main) == inheritance_graph.end())
    {
        semant_error() << "Class Main is not defined." << endl;
    }

    // Debugging only
    print_ig();
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);
    inheritance_graph[Object] = Object_class;

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
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
	       filename);
    inheritance_graph[IO] = IO_class;

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);
    inheritance_graph[Int] = Int_class;

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
    inheritance_graph[Bool] = Bool_class;

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
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
	       filename);
    inheritance_graph[Str] = Str_class;
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    classtable = new ClassTable(classes);

    if (classtable->errors()) {
    cerr << "Compilation halted due to static semantic errors." << endl;
    exit(1);
    }

    /* some semantic analysis code may go here */
    classtable->create_symbol_table();


    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}