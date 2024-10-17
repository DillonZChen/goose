(define (problem p06-problem)
 (:domain p06-domain)
 (:objects
   b1 b2 b3 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 2) (= (capacity c2) 1) (= (capacity c3) 0) (= (capacity c4) 1) (base b3) (in b3 c1) (on b3 c1) (clear b3) (base b2) (in b2 c2) (on b2 c2) (clear b2) (base b1) (in b1 c3) (on b1 c3) (clear b1) (clear c4))
 (:goal (and (on b1 c1) (on b2 b1) (on b3 b2) (clear b3)))
)
