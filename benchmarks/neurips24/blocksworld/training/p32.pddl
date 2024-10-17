(define (problem p32-problem)
 (:domain p32-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 9) (= (capacity c3) 3) (= (capacity c4) 3) (base b4) (in b4 c1) (on b4 c1) (on b2 b4) (in b2 c1) (on b8 b2) (in b8 c1) (on b1 b8) (in b1 c1) (on b9 b1) (in b9 c1) (on b3 b9) (in b3 c1) (on b7 b3) (in b7 c1) (on b5 b7) (in b5 c1) (on b10 b5) (in b10 c1) (on b6 b10) (in b6 c1) (clear b6) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b4 c1) (clear b4) (on b9 c2) (on b2 b9) (on b10 b2) (on b3 b10) (on b6 b3) (on b1 b6) (on b7 b1) (on b8 b7) (on b5 b8) (clear b5)))
)
