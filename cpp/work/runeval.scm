#!/usr/bin/env gosh

(use gauche.parseopt)
(use gauche.process)
(use slib)
(use util.match)

(require 'format)

(define (p . args) (for-each print args))

(define (usage)
  (p (format #f "Usage: ~a csv" *program-name*))
  (exit 0))

(define (dfname ntree step)
  (format #f "dfdump-~3'0d-~a" ntree step))

(define (eval-df dfs csv)
  (let1 p (run-process `(./eval ,dfs ,csv) :wait #t :output :pipe)
    (format #t "~a ~a~%" dfs (read-line (process-output p)))
    (close-input-port (process-output p))))

;; Entry point
(define (main args)
  (let-args (cdr args)
      ([#f "h|help" => usage]
       [else (opt . _) (print "Unknown option : " opt) (usage)]
       . args)
    (match args
      [(csv) (do ((ntree 50 (+ ntree 1))) ((> ntree 100) #f)
               (do ((step 1 (+ step 1))) ((> step 6) #f)
                 (eval-df (dfname ntree step) csv)))]
      [else (usage)]
      )))
