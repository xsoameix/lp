#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "scan.h"

int
scan(const char * str, const char ** begin, const char ** end) {
  while (* str == ' ' || * str == '\t' || * str == '\n') str++;
  const char * token = str;
  char c = * str;
  if (c >= '0' && c <= '9') {
    while ((c = * ++str) >= '0' && c <= '9');
    return * begin = token, * end = str, TOK_NUM;
  } else if (c == '#') {
    if ((c = * ++str) == 't' || c == 'f')
      if (!(((c = * ++str) >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z')))
        return * begin = token, * end = str, TOK_BOL;
    for (; ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z')); c = * ++str);
    return * begin = token, * end = str, TOK_NIL;
  } else if (c == '(') {
    return * begin = token, * end = str + 1, TOK_LPAREN;
  } else if (c == ')') {
    return * begin = token, * end = str + 1, TOK_RPAREN;
  } else if (c == '\0') {
    return * begin = token, * end = str, TOK_EOF;
  } else if ((c >= 'a' && c <= 'z') ||
             (c >= 'A' && c <= 'Z')) {
    while (((c = * ++str) >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '-'));
    return * begin = token, * end = str, TOK_ID;
  } else if (c == '<' ||
             c == '>' ||
             c == '=' ||
             c == '+' ||
             c == '-' ||
             c == '*' ||
             c == '/') {
    return * begin = token, * end = str + 1, TOK_ID;
  } else {
    return * begin = token, * end = str, TOK_NIL;
  }
}

int
peek(const char * str) {
  const char * begin, * end;
  return scan(str, &begin, &end);
}

int
match(const char ** str, int tok) {
  const char * begin, * end;
  if (scan(* str, &begin, &end) != tok) return 1;
  return * str = end, 0;
}

node_t *
node_new(node_t * parent, int type) {
  node_t * node = malloc(sizeof(* node));
  if (node == NULL) return NULL;
  node->parent = parent;
  node->front = node->back = node->next = NULL;
  node->type = type;
  return node;
}

void
node_add(node_t * node, node_t * child) {
  if (node->front == NULL)
    node->front = child, node->back = child;
  else
    node->back->next = child, node->back = child;
}

#define COL_RST     "\x1b[0m"
#define COL_MAGENTA "\x1b[35m"
#define COL_GREEN   "\x1b[32m"

void
node_dump(node_t * root) {
  size_t capa = 2;
  node_t ** stack = malloc(sizeof(* stack) * capa);
  if (stack == NULL) return;
  node_t nil;
  size_t indent = 0;
  size_t len = 0;
  stack[len++] = root;
  while (len) {
    node_t * node = stack[--len];
    if (node == &nil) { --len, indent--; continue; }
    for (size_t i = 0; i < indent; i++) putchar(' ');
    if (node->type == NOD_NIL) {
    } else if (node->type == NOD_NUM ||
               node->type == NOD_ID) {
      tok_t * tok = &node->val.t;
      printf("%.*s ", (int) (tok->end - tok->begin), tok->begin);
    } else if (node->type == NOD_INT) {
      printf("%d ", node->val.i);
    } else if (node->type == NOD_BOL) {
      printf("%s ", node->val.i ? "true" : "false");
    } else if (node->type == NOD_VAR) {
      var_t * var = &node->val.v;
      printf("(%zu %zu) ", var->env, var->off);
    } else if (node->type == NOD_DEF) {
      printf("(");
      def_t * def = &node->val.d;
      for (size_t i = 0; i < def->len; i++) {
        printf("%zu", def->args[i]);
        if (i != def->len - 1) printf(" ");
      }
      printf(") ");
    } else if (node->type == NOD_FUN ||
               node->type == NOD_SET ||
               node->type == NOD_IF  ||
               node->type == NOD_LT ||
               node->type == NOD_GT ||
               node->type == NOD_EQ ||
               node->type == NOD_ADD ||
               node->type == NOD_SUB ||
               node->type == NOD_MUL ||
               node->type == NOD_DIV ||
               node->type == NOD_MOD ||
               node->type == NOD_AND ||
               node->type == NOD_OR ||
               node->type == NOD_NOT ||
               node->type == NOD_PRN ||
               node->type == NOD_PRB) {
    } else {
      printf("? ");
    }
    const char * names[] = {
      "NIL",
      "NUM",
      "INT",
      "BOL",
      "ID",
      "VAR",
      "DEF",
      "FUN",
      "SET",
      "IF",
      "LT",
      "GT",
      "EQ",
      "ADD",
      "SUB",
      "MUL",
      "DIV",
      "MOD",
      "AND",
      "OR",
      "NOT",
      "PRN",
      "PRB"
    };
    printf(COL_GREEN "%s" COL_RST " " COL_MAGENTA "%p" COL_RST "\n",
           names[node->type], (void *) node);
    node = node->front;
    size_t size = 0;
    for (node_t * next = node; next != NULL; next = next->next) size++;
    size_t request = len + 2 + size;
    if (request > capa) {
      node_t ** ptr = realloc(stack, sizeof(* ptr) * request);
      if (ptr == NULL) { free(stack); return; }
      capa = request, stack = ptr;
    }
    len++, stack[len++] = &nil, len += size;
    for (size_t i = 0; i < size; i++) {
      stack[len - i - 1] = node;
      node = node->next;
    }
    indent++;
  }
  free(stack);
}

void
node_free(node_t * root) {
  size_t capa = 2;
  node_t ** stack = malloc(sizeof(* stack) * capa);
  if (stack == NULL) return;
  node_t nil;
  size_t len = 0;
  stack[len++] = root;
  while (len) {
    node_t * node = stack[--len];
    if (node == &nil) {
      node_t * parent = stack[--len];
      if (parent->type == NOD_DEF)
        free(parent->val.d.args);
      free(parent);
      continue;
    }
    node = node->front;
    size_t size = 0;
    for (node_t * next = node; next != NULL; next = next->next) size++;
    size_t request = len + 2 + size;
    if (request > capa) {
      node_t ** ptr = realloc(stack, sizeof(* ptr) * request);
      if (ptr == NULL) { free(stack); return; }
      capa = request, stack = ptr;
    }
    len++, stack[len++] = &nil;
    for (; node != NULL; node = node->next)
      stack[len++] = node;
  }
  free(stack);
}

int
fetch(const char ** str, int id, node_t * parent, int type) {
  const char * begin, * end;
  int ret = scan(* str, &begin, &end);
  if (ret != id) return 1;
  node_t * node = node_new(parent, type);
  if (node == NULL) return 1;
  tok_t * tok = &node->val.t;
  tok->begin = begin;
  tok->end = end;
  tok->id = ret;
  return node_add(parent, node), * str = end, 0;
}

void
syntax_error(const char * str) {
  printf("syntax error: %10s\n", str);
}

int
parse(const char ** str, node_t * parent) {
  int tok = peek(* str);
  if (tok == TOK_LPAREN) {
    node_t * node = node_new(parent, NOD_NIL);
    if (node == NULL) return 1;
    if (match(str, TOK_LPAREN)) return node_free(node), 1;
    while ((tok = peek(* str)) != TOK_RPAREN)
      if (tok == TOK_LPAREN) {
        if (parse(str, node)) return node_free(node), 1;
      } else if (tok == TOK_NUM) {
        if (fetch(str, TOK_NUM, node, NOD_NUM)) return node_free(node), 1;
      } else if (tok == TOK_BOL) {
        if (fetch(str, TOK_BOL, node, NOD_BOL)) return node_free(node), 1;
      } else if (tok == TOK_ID) {
        if (fetch(str, TOK_ID, node, NOD_ID)) return node_free(node), 1;
      } else {
        printf("%d\n", tok);
        return syntax_error(* str), node_free(node), 1;
      }
    if (match(str, TOK_RPAREN)) return node_free(node), 1;
    return node_add(parent, node), 0;
  } else if (tok == TOK_EOF) {
    return 0;
  } else {
    return syntax_error(* str), 1;
  }
}

void
map_init(map_t * map, map_t * prev) {
  map->prev = prev;
  map->begin = map->end = NULL;
  map->len = map->capa = 0;
}

void
map_free(map_t * map) {
  free(map->begin), map->begin = NULL;
  free(map->end), map->end = NULL;
  map->len = map->capa = 0;
}

int
map_get(map_t * map, const char * begin, const char * end, var_t * var) {
  for (size_t i = 0; map != NULL; map = map->prev, i++)
    for (size_t j = 0; j < map->len; j++)
      if (map->end[j] - map->begin[j] == end - begin &&
          !memcmp(map->begin[j], begin, (size_t) (end - begin))) {
        if (var != NULL) var->env = i, var->off = j;
        return 1;
      }
  return 0;
}

int
map_set(map_t * map, const char * begin, const char * end, var_t * var) {
  if (map_get(map, begin, end, var)) return 0;
  if (map->len + 1 > map->capa) {
    size_t request = map->capa ? map->capa * 2 : 1;
    const char ** bptr = realloc(map->begin, sizeof(* bptr) * request);
    if (bptr == NULL) return 1;
    const char ** eptr = realloc(map->end, sizeof(* eptr) * request);
    if (eptr == NULL) return free(bptr), 1;
    map->begin = bptr, map->end = eptr, map->capa = request;
  }
  map->begin[map->len] = begin;
  map->end[map->len] = end;
  if (var != NULL) var->env = 0, var->off = map->len;
  return map->len++, 0;
}

void
map_dump(map_t * map) {
  for (size_t i = 0; map != NULL; map = map->prev, i++) {
    printf("--- scope %zu ---\n", i);
    for (size_t j = 0; j < map->len; j++)
      printf("  variable: %.*s\n",
             (int) (map->end[j] - map->begin[j]), map->begin[j]);
  }
  printf("---------------\n");
}

int
tokcmp(tok_t * tok, const char * str) {
  size_t len = strlen(str);
  return (len != (size_t) (tok->end - tok->begin) ||
          memcmp(tok->begin, str, len));
}

int
toktoi(tok_t * tok, int * ret) {
  int i = 0;
  for (const char * ptr = tok->begin; ptr < tok->end; ptr++) {
    if (i > INT_MAX / 10 ||
        i < INT_MIN / 10) return printf("integer buffer overflow\n"), 1;
    i *= 10;
    int c = * ptr - '0';
    if (i > INT_MAX - c) return printf("integer buffer overflow\n"), 1;
    i += c;
  }
  return * ret = i, 0;
}

int semantic(node_t * parent, map_t * prev);

int
variables(node_t * node, map_t * prev) {
  for (; node != NULL; node = node->next)
    if (node->type == NOD_NIL) {
      if (semantic(node, prev)) return 1;
    } else if (node->type == NOD_NUM) {
      if (toktoi(&node->val.t, &node->val.i)) return 1;
      node->type = NOD_INT;
    } else if (node->type == NOD_BOL) {
      node->val.i = !tokcmp(&node->val.t, "#t");
    } else if (node->type == NOD_ID) {
      tok_t * tok = &node->val.t;
      var_t var;
      if (!map_get(prev, tok->begin, tok->end, &var))
        return printf("variable %.*s is undefined\n",
                      (int) (tok->end - tok->begin), tok->begin),
               map_dump(prev), 1;
      node->val.v = var, node->type = NOD_VAR;
    }
  return 0;
}

int
unary(node_t * parent, map_t * prev, int type) {
  node_t * node = parent->front;
  if (node->next == NULL ||
      node->next->next != NULL)
    return printf("the unary operation requires one operand\n"), 1;
  if (variables(node->next, prev)) return 1;
  return parent->type = type, 0;
}

int
binary(node_t * parent, map_t * prev, int type, int multi) {
  node_t * node = parent->front;
  if (node->next == NULL ||
      node->next->next == NULL)
    return printf("the binary operation requires at least two operands\n"), 1;
  if (node->next->next->next != NULL && !multi)
    return printf("the binary operation requires only two operands\n"), 1;
  if (variables(node->next, prev)) return 1;
  return parent->type = type, 0;
}

int
semantic(node_t * parent, map_t * prev) {
  node_t * node = parent->front;
  if (node == NULL) {
    return printf("empty list is not allowed\n"), 1;
  } else if (node->type == NOD_NIL) {
    if (semantic(node, prev) ||
        variables(node->next, prev)) return 1;
    return parent->type = NOD_FUN, 0;
  } else if (node->type == NOD_NUM) {
    return printf("integer is not a function\n"), 1;
  } else if (node->type == NOD_BOL) {
    return printf("boolean is not a function\n"), 1;
  } else if (node->type == NOD_ID) {
    tok_t * tok = &node->val.t;
    if (map_get(prev, tok->begin, tok->end, &node->val.v)) {
      if (variables(node->next, prev)) return 1;
      return node->type = NOD_VAR, parent->type = NOD_FUN, 0;
    } else if (!tokcmp(tok, "fun")) {
      if (node->next == NULL) return 1;
      size_t len = 0;
      for (node_t * arg = node->next->front; arg != NULL; arg = arg->next) {
        if (arg->type != NOD_ID)
          return printf("only named parameters are allowed\n"), 1;
        len++;
      }
      map_t map;
      map_init(&map, NULL);
      size_t * args = malloc(sizeof(* args) * len);
      if (args == NULL) return 1;
      size_t i = 0;
      for (node_t * arg = node->next->front; arg != NULL; arg = arg->next) {
        tok_t * t = &arg->val.t;
        if (map_get(&map, t->begin, t->end, NULL))
          return printf("parameter names are duplicated\n"), free(args), 1;
        var_t v;
        if (map_set(&map, t->begin, t->end, &v)) return free(args), 1;
        args[i++] = v.off;
      }
      map.prev = prev;
      if (variables(node->next->next, &map)) return free(args), 1;
      def_t * def = &parent->val.d;
      def->args = args, def->len = len, def->env = map.len;
      map_free(&map);
      return parent->type = NOD_DEF, 0;
    } else if (!tokcmp(tok, "define")) {
      node_t * name = node->next;
      if (name == NULL)
        return printf("variable name is empty\n"), 1;
      if (name->type != NOD_ID)
        return printf("variable name is not allowed\n"), 1;
      node_t * value = name->next;
      if (value == NULL)
        return printf("variable value is empty\n"), 1;
      if (value->next != NULL)
        return printf("multiple variable values is not allowed\n"), 1;
      if (variables(value, prev)) return 1;
      tok_t * t = &name->val.t;
      if (map_set(prev, t->begin, t->end, &name->val.v)) return 1;
      name->type = NOD_VAR;
      return parent->type = NOD_SET, 0;
    } else if (!tokcmp(tok, "if")) {
      node_t * cond = node->next;
      if (cond == NULL)
        return printf("the condition is empty\n"), 1;
      node_t * if_stmt = cond->next;
      if (if_stmt == NULL)
        return printf("the if-statement is empty\n"), 1;
      node_t * else_stmt = if_stmt->next;
      if (else_stmt == NULL)
        return printf("the else-statement is empty\n"), 1;
      if (variables(cond, prev)) return 1;
      return parent->type = NOD_IF, 0;
    } else if (!tokcmp(tok, "<")) {
      return binary(parent, prev, NOD_LT, 0);
    } else if (!tokcmp(tok, ">")) {
      return binary(parent, prev, NOD_LT, 0);
    } else if (!tokcmp(tok, "=")) {
      return binary(parent, prev, NOD_EQ, 0);
    } else if (!tokcmp(tok, "+")) {
      return binary(parent, prev, NOD_ADD, 1);
    } else if (!tokcmp(tok, "-")) {
      return binary(parent, prev, NOD_SUB, 0);
    } else if (!tokcmp(tok, "*")) {
      return binary(parent, prev, NOD_MUL, 1);
    } else if (!tokcmp(tok, "/")) {
      return binary(parent, prev, NOD_DIV, 0);
    } else if (!tokcmp(tok, "mod")) {
      return binary(parent, prev, NOD_MOD, 0);
    } else if (!tokcmp(tok, "and")) {
      return binary(parent, prev, NOD_AND, 1);
    } else if (!tokcmp(tok, "or")) {
      return binary(parent, prev, NOD_OR, 1);
    } else if (!tokcmp(tok, "not")) {
      return unary(parent, prev, NOD_NOT);
    } else if (!tokcmp(tok, "print-num")) {
      if (node->next == NULL)
        return printf("the parameter of print-num is empty\n"), 1;
      if (node->next->next != NULL)
        return printf("only one parameter of print-num is allowed\n"), 1;
      if (variables(node->next, prev)) return 1;
      return parent->type = NOD_PRN, 0;
    } else if (!tokcmp(tok, "print-bool")) {
      if (node->next == NULL)
        return printf("the parameter of print-bool is empty\n"), 1;
      if (node->next->next != NULL)
        return printf("only one parameter of print-bool is allowed\n"), 1;
      if (variables(node->next, prev)) return 1;
      return parent->type = NOD_PRB, 0;
    } else {
      return printf("variable %.*s is undefined\n",
                    (int) (tok->end - tok->begin), tok->begin),
             map_dump(prev), 1;
    }
  } else {
    return 1;
  }
}

env_t *
env_new(env_t * ret, env_t * prev, size_t len) {
  env_t * env = malloc(sizeof(* env));
  if (env == NULL) return NULL;
  loc_t * locs = malloc(sizeof(* env->locs) * len);
  if (locs == NULL) return free(env), NULL;
  for (size_t i = 0; i < len; i++) {
    locs[i].ref = 0;
    locs[i].obj.type = OBJ_NIL;
  }
  env->ret = ret;
  env->prev = prev;
  env->locs = locs;
  env->len = len;
  return env;
}

void
env_free(env_t * env) {
  free(env->locs);
  free(env);
}

int
env_add(env_t * env, size_t len) {
  if (len <= env->len) return 0;
  loc_t * locs = realloc(env->locs, sizeof(* locs) * len);
  if (locs == NULL) return 1;
  for (size_t i = env->len; i < len; i++) {
    locs[i].ref = 0;
    locs[i].obj.type = OBJ_NIL;
  }
  env->locs = locs;
  env->len = len;
  return 0;
}

void
env_dump(env_t * env, int ret);

void
env_get(env_t * env, var_t * var, obj_t * obj) {
  for (size_t i = 0; i < var->env; i++) env = env->prev;
  * obj = env->locs[var->off].obj;
}

void
env_set(env_t * env, var_t * var, obj_t * obj) {
  for (size_t i = 0; i < var->env; i++) env = env->prev;
  env->locs[var->off].obj = * obj;
}

void
env_dump(env_t * env, int ret) {
  for (size_t i = 0; env != NULL; env = ret ? env->ret : env->prev, i++) {
    printf("--- scope %zu --- %p\n", i, (void *) env);
    printf("  variables size: %zu\n", env->len);
  }
  printf("---------------\n");
}

gc_t *
gc_new(void) {
  gc_t * gc = malloc(sizeof(* gc));
  if (gc == NULL) return NULL;
  gc->addrs = NULL;
  gc->capa = gc->len = 0;
  return gc;
}

int
gc_overflow(gc_t * gc) {
  printf("gc: %zu\n", gc->len);
  return gc->len > 0;
}

int
gc_add(gc_t * gc, env_t * env, size_t * id) {
  if (gc_overflow(gc)) {
    if (gc_cleanup(gc, env->ret)) return 1;
    printf("gc: %zu (cleanup)\n", gc->len);
  }
  if (gc->len + 1 > gc->capa) {
    size_t request = gc->capa ? gc->capa * 2 : 1;
    addr_t * addrs = realloc(gc->addrs, sizeof(* addrs) * request);
    if (addrs == NULL) return 1;
    gc->addrs = addrs, gc->capa = request;
  }
  gc->addrs[gc->len] = (addr_t) {.val = env, .rec = NULL};
  return * id = gc->len++, 0;
}

int
gc_ref(gc_t * gc, size_t id, size_t to) {
  rec_t * rec = malloc(sizeof(* rec));
  if (rec == NULL) return 1;
  addr_t * addr = &gc->addrs[id];
  rec->next = addr->rec, addr->rec = rec;
  return rec->to = to, 0;
}

void
gc_ref_env(gc_t * gc, env_t * env) {
  for (env_t * e = env; e != NULL; e = e->prev) {
    addr_t * addr = &gc->addrs[e->id];
    if (addr->mark == GC_MARK) break;
    printf("env prev %p mark\n", (void *) e);
    addr->mark = GC_MARK;
  }
  for (env_t * e = env; e != NULL; e = e->prev) {
    for (size_t i = 0; i < e->len; i++) {
      //if (!e->locs[i].ref) continue;
      obj_t * obj = &e->locs[i].obj;
      if (obj->type != OBJ_FUN) continue;
      env_t * to = obj->val.f.env;
      if (gc->addrs[to->id].mark == GC_MARK) continue;
      gc_ref_env(gc, to);
    }
  }
}

int
gc_cleanup(gc_t * gc, env_t * env) {
  for (size_t i = 0; i < gc->len; i++) gc->addrs[i].mark = GC_NIL;
  for (env_t * e = env; e != NULL; e = e->ret)
    gc_ref_env(gc, e);
  size_t len = 0;
  for (size_t i = 0; i < gc->len; i++)
    if (gc->addrs[i].mark == GC_MARK) {
      gc->addrs[i].compat = len;
      gc->addrs[i].val->id = len++;
    } else {
      for (rec_t * rec = gc->addrs[i].rec; rec != NULL;) {
        rec_t * next = rec->next;
        free(rec), rec = next;
      }
      env_free(gc->addrs[i].val);
    }
  for (size_t i = 0; i < gc->len; i++) {
    addr_t * addr = &gc->addrs[i];
    if (addr->mark == GC_MARK)
      gc->addrs[addr->compat] = * addr;
  }
  return gc->len = len, 0;
}

void
gc_free(gc_t * gc) {
  gc_cleanup(gc, NULL);
  for (size_t i = 0; i < gc->len; i++)
    for (rec_t * rec = gc->addrs[i].rec; rec != NULL;) {
      rec_t * next = rec->next;
      free(rec), rec = next;
    }
  free(gc->addrs);
  free(gc);
}

void
fun_init(fun_t * fun, node_t * node, env_t * prev) {
  fun->env = prev;
  fun->node = node;
}

typedef int calc_t(int a, int b, int * ret);

int
calc(node_t * parent, env_t * prev, gc_t * gc, calc_t * cb, char type,
     int unary, obj_t * obj) {
  node_t * node = parent->front->next;
  obj_t a;
  if (eval(node, prev, gc, &a)) return 1;
  if (a.type != type)
    return printf("variable is not %s\n",
                  type == OBJ_INT ? "integer" : "boolean"), 1;
  while (node = node->next, node != NULL) {
    obj_t b;
    if (eval(node, prev, gc, &b)) return 1;
    if (b.type != type)
      return printf("variable is not %s\n",
                    type == OBJ_INT ? "integer" : "boolean"), 1;
    if (cb(a.val.i, b.val.i, &a.val.i)) return 1;
  }
  if (unary) if (cb(a.val.i, 0, &a.val.i)) return 1;
  return obj->val.i = a.val.i, obj->type = type, 0;
}

int lt(int a, int b, int * ret) { return * ret = a < b, 0; }
int gt(int a, int b, int * ret) { return * ret = a > b, 0; }
int eq(int a, int b, int * ret) { return * ret = a == b, 0; }

int
add(int a, int b, int * ret) {
  if ((b > 0 && a > INT_MAX - b) ||
      (b < 0 && a < INT_MIN - (b + 1) + 1))
    return printf("integer overflow: %d + %d\n", a, b), 1;
  return * ret = a + b, 0;
}

int
sub(int a, int b, int * ret) {
  if ((b > 0 && a < INT_MIN + b) ||
      (b < 0 && a > INT_MAX + b))
    return printf("integer overflow: %d - %d\n", a, b), 1;
  return * ret = b < 0 ? a - (b + 1) + 1 : a - b, 0;
}

int
mul(int a, int b, int * ret) {
  int c = a * b;
  if (b && c / b != a)
    return printf("integer overflow: %d * %d\n", a, b), 1;
  return * ret = c, 0;
}

int
idiv(int a, int b, int * ret) {
  if (!b)
    return printf("division by zero: %d / %d\n", a, b), 1;
  return * ret = a / b, 0;
}

int
mod(int a, int b, int * ret) {
  if (!b)
    return printf("division by zero: %d %% %d\n", a, b), 1;
  return * ret = a % b, 0;
}

int and(int a, int b, int * ret) { return * ret = a && b, 0; }
int or(int a, int b, int * ret) { return * ret = a || b, 0; }
int not(int a, int b, int * ret) { (void) b; return * ret = !a, 0; }

int
eval(node_t * parent, env_t * prev, gc_t * gc, obj_t * obj) {
  if (parent->type == NOD_INT) {
    return obj->val.i = parent->val.i, obj->type = OBJ_INT, 0;
  } else if (parent->type == NOD_BOL) {
    return obj->val.i = parent->val.i, obj->type = OBJ_BOL, 0;
  } else if (parent->type == NOD_VAR) {
    return env_get(prev, &parent->val.v, obj), 0;
  } else if (parent->type == NOD_DEF) {
    fun_init(&obj->val.f, parent, prev);
    return obj->type = OBJ_FUN, 0;
  } else if (parent->type == NOD_FUN) {
    node_t * caller = parent->front;
    obj_t o;
    if (eval(caller, prev, gc, &o)) return 1;
    if (o.type != OBJ_FUN) return printf("variable is not function\n"), 1;
    fun_t * fun = &o.val.f;
    node_t * callee = fun->node;
    def_t * def = &callee->val.d;
    size_t len = 0;
    for (node_t * arg = caller->next; arg != NULL; arg = arg->next) len++;
    if (len != def->len) return printf("parameters length do not match\n"), 1;
    env_t * env = env_new(prev, fun->env, def->env);
    if (env == NULL) return 1;
    if (gc_add(gc, env, &env->id)) return env_free(env), 1;
    node_t * params = callee->front->next;
    node_t * arg = caller->next;
    for (size_t i = 0; i < def->len; i++, arg = arg->next) {
      obj_t ret;
      if (eval(arg, prev, gc, &ret)) return env_free(env), 1;
      var_t var = {.env = 0, .off = def->args[i]};
      env_set(env, &var, &ret);
    }
    obj->type = OBJ_NIL;
    for (node_t * stmt = params->next; stmt != NULL; stmt = stmt->next)
      if (eval(stmt, env, gc, obj)) return env_free(env), 1;
    return 0;
  } else if (parent->type == NOD_SET) {
    node_t * name = parent->front->next;
    obj_t o;
    if (eval(name->next, prev, gc, &o)) return 1;
    env_set(prev, &name->val.v, &o);
    return obj->type = OBJ_NIL, 0;
  } else if (parent->type == NOD_IF) {
    node_t * cond = parent->front->next;
    obj_t o;
    if (eval(cond, prev, gc, &o)) return 1;
    if (o.type != OBJ_BOL) return printf("variable is not boolean\n"), 1;
    node_t * stmt = o.val.i ? cond->next : cond->next->next;
    if (eval(stmt, prev, gc, obj)) return 1;
    if (obj->type == OBJ_NIL)
      return printf("the return value of if-else statement is nil\n"), 1;
    return 0;
  } else if (parent->type == NOD_LT) {
    return calc(parent, prev, gc, lt, OBJ_BOL, 0, obj);
  } else if (parent->type == NOD_GT) {
    return calc(parent, prev, gc, gt, OBJ_BOL, 0, obj);
  } else if (parent->type == NOD_EQ) {
    return calc(parent, prev, gc, eq, OBJ_BOL, 0, obj);
  } else if (parent->type == NOD_ADD) {
    return calc(parent, prev, gc, add, OBJ_INT, 0, obj);
  } else if (parent->type == NOD_SUB) {
    return calc(parent, prev, gc, sub, OBJ_INT, 0, obj);
  } else if (parent->type == NOD_MUL) {
    return calc(parent, prev, gc, mul, OBJ_INT, 0, obj);
  } else if (parent->type == NOD_DIV) {
    return calc(parent, prev, gc, idiv, OBJ_INT, 0, obj);
  } else if (parent->type == NOD_MOD) {
    return calc(parent, prev, gc, mod, OBJ_INT, 0, obj);
  } else if (parent->type == NOD_AND) {
    return calc(parent, prev, gc, and, OBJ_BOL, 0, obj);
  } else if (parent->type == NOD_OR) {
    return calc(parent, prev, gc, or, OBJ_BOL, 0, obj);
  } else if (parent->type == NOD_NOT) {
    return calc(parent, prev, gc, not, OBJ_BOL, 1, obj);
  } else if (parent->type == NOD_PRN) {
    node_t * num = parent->front->next;
    obj_t o;
    if (eval(num, prev, gc, &o)) return 1;
    if (o.type != OBJ_INT)
      return printf("the argument of print-num is not integer\n"), 1;
    printf("%d\n", o.val.i);
    return obj->type = OBJ_NIL, 0;
  } else if (parent->type == NOD_PRB) {
    node_t * num = parent->front->next;
    obj_t o;
    if (eval(num, prev, gc, &o)) return 1;
    if (o.type != OBJ_BOL)
      return printf("the argument of print-bool is not boolean\n"), 1;
    printf("%s\n", o.val.i ? "#t" : "#f");
    return obj->type = OBJ_NIL, 0;
  } else {
    printf("? %d %p\n", parent->type, (void *) parent);
    return 1;
  }
}

int
run(const char * str, node_t * parent, map_t * map, env_t * env, gc_t * gc) {
  for (node_t * node = NULL; * str;) {
    if (parse(&str, parent)) return 1;
    //node_dump(parent);
    node = node == NULL ? parent->front : node->next;
    if (node == NULL) return 0;
    if (semantic(node, map)) return 1;
    if (env_add(env, map->len)) return 1;
    //node_dump(parent);
    obj_t obj;
    if (eval(node, env, gc, &obj)) return 1;
    if (obj.type == OBJ_NIL)
      printf("obj NIL\n");
    else if (obj.type == OBJ_INT)
      printf("obj %d NUM\n", obj.val.i);
    else if (obj.type == OBJ_BOL)
      printf("obj %s BOL\n", obj.val.i ? "true" : "false");
    else if (obj.type == OBJ_FUN)
      printf("obj %p FUN\n", (void *) obj.val.f.node);
  }
  return 0;
}

int
feed(const char * str) {
  node_t * node = node_new(NULL, NOD_NIL);
  if (node == NULL) return 1;
  map_t map;
  map_init(&map, NULL);
  env_t * env = env_new(NULL, NULL, 0);
  if (env == NULL) return node_free(node), 1;
  gc_t * gc = gc_new();
  if (gc == NULL) return env_free(env), 1;
  if (gc_add(gc, env, &env->id)) return gc_free(gc), env_free(env), 1;
  if (run(str, node, &map, env, gc))
    return gc_free(gc), map_free(&map), node_free(node), 1;
  gc_free(gc);
  map_free(&map);
  node_free(node);
  return 0;
}
