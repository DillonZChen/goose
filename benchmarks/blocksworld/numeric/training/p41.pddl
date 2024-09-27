(define (problem p41-problem)
 (:domain p41-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 4) (= (capacity c3) 2) (= (capacity c4) 7) (base b12) (in b12 c1) (on b12 c1) (on b8 b12) (in b8 c1) (on b9 b8) (in b9 c1) (clear b9) (base b5) (in b5 c2) (on b5 c2) (on b10 b5) (in b10 c2) (on b6 b10) (in b6 c2) (on b1 b6) (in b1 c2) (clear b1) (base b2) (in b2 c3) (on b2 c3) (on b4 b2) (in b4 c3) (on b11 b4) (in b11 c3) (on b7 b11) (in b7 c3) (on b3 b7) (in b3 c3) (clear b3) (clear c4))
 (:goal (and (on b3 c1) (on b4 b3) (on b11 b4) (on b1 b11) (clear b1) (on b7 c2) (on b10 b7) (on b5 b10) (on b8 b5) (on b2 b8) (on b12 b2) (on b9 b12) (on b6 b9) (clear b6)))
)
