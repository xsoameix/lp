#include <stdio.h>
#include "scan.h"

int
main(void) {
  //const char * str = " ";
  //const char * str = "(define a ((fun (a) "
  //    "(define b (fun (b) (+ a b)))"
  //    "b"
  //    ") 1)) (define b ((fun (c) "
  //    "(define b (fun (b) (+ c b (a 3))))"
  //    "b"
  //    ") 2)) (fun () 1)";
  //const char * str = "(define a ((fun (a) "
  //    "(define b (fun (b) (+ a b)))"
  //    "(define c (fun (b) (+ a b)))"
  //    "(define d (fun (b) (+ a b)))"
  //    "(define e (fun (b) (+ a b)))"
  //    "b"
  //    ") 1)) (fun () 1) (a 2) (a 3)";
  //const char * str = "(define a (fun (a) "
  //    "(define b (fun (b) (+ a b)))"
  //    "(define c (fun (b) (+ a b)))"
  //    "b"
  //    ")) (define b (a 1)) (fun () 1) (b 2) (b 3)";
  //const char * str = "(define a (fun () "
  //    "(define b (fun () 1))"
  //    "1"
  //    ")) (a) (define b (fun () 1))";
  //const char * str = "(define a (not #t)) (print-bool a)";
  //const char * str = "(if #t (print-num 1) 2)";
  //const char * str = "(if (and #t #t #f) ((fun () 1)) 2)";
  //const char * str = "((fun () "
  //    "(define a 1)"
  //    "(define b (fun (b) a))"
  //    "(define c (fun () 1))"
  //    "(define b 1)"
  //    "(define c 1)"
  //    "(define d (fun ()))"
  //    "(d)"
  //    "))";
  //const char * str = "(((fun (a) (fun (b) (define c 1) (+ a b c))) 2) 3)";
  //const char * str = "((fun (b) (define c 1) (+ b c)) 1)";
  //const char * str = "(define a (fun (a) (fun (b) (define c 1) (+ a c))))";
  //const char * str =
  //    "(define a (fun (a)"
  //    "  (fun (b) (+ a b))"
  //    "))"
  //    "(define b (a 1))"
  //    "(define c (a 2))"
  //    "(print-num (b 3))"
  //    "(print-num (c 4))";
  //const char * str = "(define a a) ((fun () a))";
  const char * str =
      "(define a (fun (x) "
      "  (define x (- x 1))"
      "  (if (> x 0) (a x) #f)"
      "  (print-num x)"
      "  #t))"
      "(a 10)";
  return feed(str);
}
