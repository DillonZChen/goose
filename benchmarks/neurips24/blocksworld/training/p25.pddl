(define (problem p25-problem)
 (:domain p25-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 3) (= (capacity c2) 0) (= (capacity c3) 4) (= (capacity c4) 3) (base b6) (in b6 c1) (on b6 c1) (clear b6) (base b3) (in b3 c2) (on b3 c2) (on b1 b3) (in b1 c2) (on b5 b1) (in b5 c2) (on b4 b5) (in b4 c2) (on b2 b4) (in b2 c2) (on b7 b2) (in b7 c2) (clear b7) (clear c3) (clear c4))
 (:goal (and (on b2 c1) (on b7 b2) (clear b7) (on b4 c2) (on b6 b4) (on b1 b6) (on b5 b1) (on b3 b5) (clear b3)))
)
