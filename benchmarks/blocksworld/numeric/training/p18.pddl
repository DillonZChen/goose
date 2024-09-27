(define (problem p18-problem)
 (:domain p18-domain)
 (:objects
   b1 b2 b3 b4 b5 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 2) (= (capacity c2) 0) (= (capacity c3) 3) (= (capacity c4) 2) (base b3) (in b3 c1) (on b3 c1) (clear b3) (base b4) (in b4 c2) (on b4 c2) (on b2 b4) (in b2 c2) (on b5 b2) (in b5 c2) (on b1 b5) (in b1 c2) (clear b1) (clear c3) (clear c4))
 (:goal (and (on b1 c1) (clear b1) (on b4 c2) (on b5 b4) (on b3 b5) (on b2 b3) (clear b2)))
)
