#include <stdlib.h>
#include <check.h>
#include <main.h>
#include "scan.h"

START_TEST(test_scan) {
  const char * spaces = " \t 0";
  const char * begin, * end;
  ck_assert(scan(spaces, &begin, &end) == TOK_NUM &&
            begin == spaces + 3 && end == spaces + 4);

  const char * number = "0";
  ck_assert(scan(number, &begin, &end) == TOK_NUM &&
            begin == number && end == number + 1);

  const char * lparen = "(";
  ck_assert(scan(lparen, &begin, &end) == TOK_LPAREN &&
            begin == lparen && end == lparen + 1);

  const char * rparen = ")";
  ck_assert(scan(rparen, &begin, &end) == TOK_RPAREN &&
            begin == rparen && end == rparen + 1);

  const char * nil = "";
  ck_assert(scan(nil, &begin, &end) == TOK_EOF &&
            begin == nil && end == nil);

  const char * op = "+";
  ck_assert(scan(op, &begin, &end) == TOK_ID &&
            begin == op && end == op + 1);

  const char * id = "foo";
  ck_assert(scan(id, &begin, &end) == TOK_ID &&
            begin == id && end == id + 3);

  malloc(1000);
} END_TEST

START_TEST(test_paren) {
  const char * incomplete = "(";
  node_t * node = node_new(NULL, NOD_NIL);
  ck_assert(node != NULL &&
            parse(&incomplete, node));

  const char * complete = "()";
  node = node_new(NULL, NOD_NIL);
  node_dump(node);
  ck_assert(node != NULL &&
            !parse(&complete, node));
  node_dump(node);

  const char * expr = "(+ 1 (add 3 4) 3)";
  node = node_new(NULL, NOD_NIL);
  node_dump(node);
  ck_assert(node != NULL &&
            !parse(&expr, node));
  node_dump(node);

  node_free(node);
} END_TEST

Suite *
make_scan_suite(void) {
  Suite * suite = suite_create("scan");
  TCase * tcase = tcase_create("scan");
  tcase_add_test(tcase, test_scan);
  tcase_add_test(tcase, test_paren);
  suite_add_tcase(suite, tcase);
  return suite;
}

int
main(int argc, char ** argv) {
  int failed;
  SRunner * runner = srunner_create(make_scan_suite());
  srunner_run_all(runner, CK_NORMAL);
  failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return failed;
}
