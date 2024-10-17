(define (problem p28-problem)
 (:domain p28-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 4) (= (capacity c3) 4) (= (capacity c4) 4) (base b2) (in b2 c1) (on b2 c1) (on b8 b2) (in b8 c1) (on b7 b8) (in b7 c1) (on b3 b7) (in b3 c1) (on b1 b3) (in b1 c1) (on b5 b1) (in b5 c1) (on b4 b5) (in b4 c1) (on b6 b4) (in b6 c1) (clear b6) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b5 c1) (clear b5) (on b1 c2) (on b8 b1) (on b2 b8) (clear b2) (on b7 c3) (on b3 b7) (on b6 b3) (on b4 b6) (clear b4)))
)
