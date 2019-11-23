(define form1
  (lambda (elem)
     (cons elem (cons "." '()))))

(define form2
  (lambda (elem)
     (cons "." (cons elem '()))))

(define merge
  (lambda (lst1 lst2)
    (cond
     ((null? lst1) lst2)
     (else (merge (cdr lst1) (cons (car lst1) lst2))))))

(define tree
  (lambda (n)
    (cond
     ((= n 1) '((".")))
     (else (let ((a (tree (- n 1)))) (merge (map form1 a) (map form2 a)))))))

(display "Enter the number of nodes:")
(tree (read))

