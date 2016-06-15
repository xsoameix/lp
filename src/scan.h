#ifndef SCAN_H
#define SCAN_H

#define TOK_NIL    0
#define TOK_EOF    1
#define TOK_NUM    2
#define TOK_SYM    3
#define TOK_ID     4
#define TOK_LPAREN 5
#define TOK_RPAREN 6

#define NOD_NIL  0
#define NOD_NUM  1
#define NOD_INT  2
#define NOD_SYM  3
#define NOD_BOL  4
#define NOD_ID   5
#define NOD_VAR  6
#define NOD_DEF  7
#define NOD_FUN  8
#define NOD_SET  9
#define NOD_IF  10
#define NOD_LT  11
#define NOD_GT  12
#define NOD_EQ  13
#define NOD_ADD 14
#define NOD_SUB 15
#define NOD_MUL 16
#define NOD_DIV 17
#define NOD_MOD 18
#define NOD_AND 19
#define NOD_OR  20
#define NOD_NOT 21
#define NOD_PRN 22
#define NOD_PRB 23

#define OBJ_NIL 0
#define OBJ_INT 1
#define OBJ_BOL 2
#define OBJ_FUN 3

#define GC_NIL  0
#define GC_MARK 1

typedef struct {
  const char * begin;
  const char * end;
  const char * line;
  size_t lnum;
  int id;
} tok_t;

typedef struct map {
  struct map * prev;
  const char ** begin;
  const char ** end;
  size_t len;
  size_t capa;
} map_t;

typedef struct {
  size_t env;
  size_t off;
} var_t;

typedef struct {
  size_t * args;
  size_t len;
  size_t env;
} def_t;

typedef union {
  int   i;
  var_t v;
  def_t d;
} nval_t;

typedef struct env {
  struct env * ret;
  struct env * prev;
  struct loc * locs;
  size_t  len;
  size_t  id;
} env_t;

typedef struct rec {
  struct rec * next;
  size_t to;
} rec_t; // record

typedef struct addr {
  env_t * val;
  rec_t * rec;
  char    mark;
  size_t  compat;
} addr_t;

typedef struct {
  addr_t * addrs;
  size_t   capa;
  size_t   len;
} gc_t;

typedef struct {
  env_t * env;
  struct node * node;
} fun_t;

typedef union {
  int   i;
  fun_t f;
} val_t;

typedef struct {
  char  type;
  val_t val;
} obj_t;

typedef struct loc {
  obj_t obj;
} loc_t; // local

typedef struct node {
  struct node * parent;
  struct node * next;
  struct node * front;
  struct node * back;
  tok_t tok;
  int type;
  nval_t val;
} node_t;

node_t * node_new(node_t * parent, int type);
void node_dump(node_t * root);
void node_free(node_t * root);

void map_init(map_t * map, map_t * prev);
void map_free(map_t * map);

env_t * env_new(env_t * ret, env_t * prev, size_t len);
int env_add(env_t * env, size_t len);
void env_free(env_t * env);

gc_t * gc_new(void);
int gc_add(gc_t * gc, env_t * env, size_t * id);
int gc_cleanup(gc_t * gc, env_t * prev, env_t * stack);
void gc_free(gc_t * gc);

int scan(const char * str, const char ** begin, const char ** end,
    const char ** line, size_t * lnum);
int parse(const char ** str, node_t * parent,
    const char * file, const char ** line, size_t * lnum);
int semantic(node_t * parent, map_t * prev, const char * file);
int eval(node_t * parent, env_t * prev, env_t * stack,
    gc_t * gc, const char * file, obj_t * obj);
int run(const char * str, node_t * parent, map_t * map, env_t * env, gc_t * gc,
    const char * file);
int feed(const char * str, const char * file);
int exec(const char * path);

#endif
