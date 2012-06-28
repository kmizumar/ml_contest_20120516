#!/usr/bin/env gosh

(use gauche.parseopt)
(use math.mt-random)
(use util.match)

(define (p . args) (for-each print args))

(define (usage)
  (p (format #f "Usage: ~a srcfile ratio trainfile testfile" *program-name*)
     "Example"
     (format #f "  % ~a data.csv 0.7 train.csv test.csv" *program-name*)
     "  will split data.csv into train.csv and test.csv"
     "  70% of lines will be kept in train.csv and the rest 30% will be kept in test.csv"
     "  lines are selected randomly")
  (exit 0))

(define (split-file in ratio out0 out1)
  (let1 m (make <mersenne-twister> :seed (sys-time))
    (generator-for-each
     (^[line]
       (format (if (< (mt-random-real m) ratio) out0 out1) "~a~%" line))
     (^[] (read-line in))))
  (close-output-port out0)
  (close-output-port out1))

;; Entry point
(define (main args)
  (let-args (cdr args)
      ([#f "h|help" => usage]
       [else (opt . _) (print "Unknown option : " opt) (usage)]
       . args)
    (match args
      [(src ratio train test)
       (call-with-input-file src
         (^p (split-file p (x->number ratio)
                (open-output-file train :if-exists :supersede)
                (open-output-file test :if-exists :supersede)))
         :if-does-not-exist :error)]
      [else (usage)])))
