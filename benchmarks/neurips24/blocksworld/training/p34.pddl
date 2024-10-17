(define (problem p34-problem)
 (:domain p34-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 5) (= (capacity c3) 5) (= (capacity c4) 5) (base b8) (in b8 c1) (on b8 c1) (on b3 b8) (in b3 c1) (on b4 b3) (in b4 c1) (on b7 b4) (in b7 c1) (on b9 b7) (in b9 c1) (on b6 b9) (in b6 c1) (on b5 b6) (in b5 c1) (on b10 b5) (in b10 c1) (on b1 b10) (in b1 c1) (on b2 b1) (in b2 c1) (clear b2) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b1 c1) (on b9 b1) (on b2 b9) (on b6 b2) (on b8 b6) (on b3 b8) (on b10 b3) (on b7 b10) (on b4 b7) (on b5 b4) (clear b5)))
)
