#!/usr/bin/env gosh

(use gauche.parseopt)
(use text.tr)
(use util.match)

(define (p . args) (for-each print args))

(define (usage)
  (p (format #f "Usage: ~a src [dst]" *program-name*)
     )
  (exit 0))

(define (name->c name)
  (string-tr name "\-\." "__"))

(define (gensrc name :optional (out (current-output-port)))
  (let1 p (^ args (for-each (^x (format out "~a~%" x)) args))
    (p (format #f "// decisionforest in ~a" name)
       (format #f "const char *~a[] = {" (name->c name)))
    (call-with-input-file name
      (^p (generator-for-each
           (^[line] (unless (zero? (string-length line))
                      (format out "\"~a\\n\",~%" line)))
           (^[] (read-line p))))
      :if-does-not-exist :error)
    (p "0 // sentinel"
       "};")))

;; Entry point
(define (main args)
  (let-args (cdr args)
      ([#f "h|help" => usage]
       [else (opt . _) (print "Unknown option : " opt) (usage)]
       . args)
    (match args
      [(src)
       (call-with-output-file (string-append src ".cpp")
         (^p (gensrc src p)) :if-exists :supersede)]
      [(src dst)
       (call-with-output-file dst
         (^p (gensrc src p)) :if-exists :supersede)]
      [else (usage)])))
