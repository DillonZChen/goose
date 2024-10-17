(define (problem p10-problem)
 (:domain p10-domain)
 (:objects
   b1 b2 b3 b4 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 1) (= (capacity c2) 1) (= (capacity c3) 2) (= (capacity c4) 2) (base b2) (in b2 c1) (on b2 c1) (on b3 b2) (in b3 c1) (clear b3) (base b4) (in b4 c2) (on b4 c2) (on b1 b4) (in b1 c2) (clear b1) (clear c3) (clear c4))
 (:goal (and (on b2 c1) (on b1 b2) (clear b1) (on b4 c2) (on b3 b4) (clear b3)))
)
