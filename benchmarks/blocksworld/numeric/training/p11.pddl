(define (problem p11-problem)
 (:domain p11-domain)
 (:objects
   b1 b2 b3 b4 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 2) (= (capacity c2) 0) (= (capacity c3) 2) (= (capacity c4) 2) (base b2) (in b2 c1) (on b2 c1) (on b1 b2) (in b1 c1) (clear b1) (base b4) (in b4 c2) (on b4 c2) (on b3 b4) (in b3 c2) (clear b3) (clear c3) (clear c4))
 (:goal (and (on b4 c1) (on b3 b4) (on b1 b3) (on b2 b1) (clear b2)))
)
