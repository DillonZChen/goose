(define (problem p02-problem)
 (:domain p02-domain)
 (:objects
   b1 b2 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 1) (= (capacity c2) 0) (= (capacity c3) 1) (= (capacity c4) 1) (base b2) (in b2 c1) (on b2 c1) (clear b2) (base b1) (in b1 c2) (on b1 c2) (clear b1) (clear c3) (clear c4))
 (:goal (and (on b1 c1) (on b2 b1) (clear b2)))
)
