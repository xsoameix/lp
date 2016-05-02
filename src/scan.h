#ifndef SCAN_H
#define SCAN_H

#define TOK_NIL    0
#define TOK_EOF    1
#define TOK_NUM    2
#define TOK_BOL    3
#define TOK_ID     4
#define TOK_LPAREN 5
#define TOK_RPAREN 6

#define NOD_NIL  0
#define NOD_NUM  1
#define NOD_INT  2
#define NOD_BOL  3
#define NOD_ID   4
#define NOD_VAR  5
#define NOD_DEF  6
#define NOD_FUN  7
#define NOD_SET  8
#define NOD_IF   9
#define NOD_LT  10
#define NOD_GT  11
#define NOD_EQ  12
#define NOD_ADD 13
#define NOD_SUB 14
#define NOD_MUL 15
#define NOD_DIV 16
#define NOD_MOD 17
#define NOD_AND 18
#define NOD_OR  19
#define NOD_NOT 20
#define NOD_PRN 21
#define NOD_PRB 22

#define OBJ_NIL 0
#define OBJ_INT 1
#define OBJ_BOL 2
#define OBJ_FUN 3

#define GC_NIL  0
#define GC_MARK 1

typedef struct {
  const char * begin;
  const char * end;
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
  tok_t t;
  int   i;
  char  b;
  var_t v;
  def_t d;
} nval_t;

typedef struct env {
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
  char ref;
  obj_t obj;
} loc_t; // local

typedef struct node {
  struct node * parent;
  struct node * next;
  struct node * front;
  struct node * back;
  int type;
  nval_t val;
} node_t;

node_t * node_new(node_t * parent, int type);
void node_dump(node_t * root);
void node_free(node_t * root);

void map_init(map_t * map, map_t * prev);
void map_free(map_t * map);

env_t * env_new(env_t * prev, size_t len);
int env_add(env_t * env, size_t len);
void env_free(env_t * env);

gc_t * gc_new(void);
int gc_add(gc_t * gc, env_t * env, size_t * id);
int gc_cleanup(gc_t * gc, env_t * env);
void gc_free(gc_t * gc);

int scan(const char * str, const char ** begin, const char ** end);
int parse(const char ** str, node_t * parent);
int semantic(node_t * parent, map_t * prev);
int eval(node_t * parent, env_t * prev, gc_t * gc, obj_t * obj);
int run(const char * str, node_t * parent, map_t * map, env_t * env, gc_t * gc);
int feed(const char * str);

#endif
