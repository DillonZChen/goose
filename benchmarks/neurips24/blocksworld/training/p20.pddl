(define (problem p20-problem)
 (:domain p20-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 4) (= (capacity c3) 3) (= (capacity c4) 2) (base b3) (in b3 c1) (on b3 c1) (on b1 b3) (in b1 c1) (on b4 b1) (in b4 c1) (on b6 b4) (in b6 c1) (on b2 b6) (in b2 c1) (on b5 b2) (in b5 c1) (clear b5) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b6 c1) (on b3 b6) (clear b3) (on b4 c2) (on b5 b4) (on b1 b5) (on b2 b1) (clear b2)))
)
