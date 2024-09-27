(define (problem p33-problem)
 (:domain p33-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 4) (= (capacity c3) 1) (= (capacity c4) 6) (base b8) (in b8 c1) (on b8 c1) (on b2 b8) (in b2 c1) (clear b2) (base b5) (in b5 c2) (on b5 c2) (on b10 b5) (in b10 c2) (on b3 b10) (in b3 c2) (clear b3) (base b4) (in b4 c3) (on b4 c3) (on b9 b4) (in b9 c3) (on b6 b9) (in b6 c3) (on b1 b6) (in b1 c3) (on b7 b1) (in b7 c3) (clear b7) (clear c4))
 (:goal (and (on b8 c1) (on b4 b8) (on b2 b4) (clear b2) (on b3 c2) (on b5 b3) (on b10 b5) (on b9 b10) (on b7 b9) (on b6 b7) (on b1 b6) (clear b1)))
)
