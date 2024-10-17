(define (problem p0_01-problem)
 (:domain p0_01-domain)
 (:objects
   b1 b2 b3 b4 b5 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 1) (= (capacity c2) 0) (= (capacity c3) 3) (= (capacity c4) 3) (base b1) (in b1 c1) (on b1 c1) (on b2 b1) (in b2 c1) (clear b2) (base b4) (in b4 c2) (on b4 c2) (on b5 b4) (in b5 c2) (on b3 b5) (in b3 c2) (clear b3) (clear c3) (clear c4))
 (:goal (and (on b2 c1) (clear b2) (on b3 c2) (on b4 b3) (clear b4) (on b5 c3) (on b1 b5) (clear b1)))
)
