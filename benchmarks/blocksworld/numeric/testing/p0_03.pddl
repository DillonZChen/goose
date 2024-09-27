(define (problem p0_03-problem)
 (:domain p0_03-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 3) (= (capacity c3) 3) (= (capacity c4) 3) (base b2) (in b2 c1) (on b2 c1) (on b1 b2) (in b1 c1) (on b4 b1) (in b4 c1) (on b5 b4) (in b5 c1) (on b6 b5) (in b6 c1) (on b3 b6) (in b3 c1) (clear b3) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b1 c1) (on b6 b1) (on b2 b6) (on b5 b2) (on b3 b5) (on b4 b3) (clear b4)))
)
