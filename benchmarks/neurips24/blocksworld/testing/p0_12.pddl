(define (problem p0_12-problem)
 (:domain p0_12-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 2) (= (capacity c2) 2) (= (capacity c3) 9) (= (capacity c4) 8) (base b9) (in b9 c1) (on b9 c1) (on b13 b9) (in b13 c1) (on b5 b13) (in b5 c1) (on b8 b5) (in b8 c1) (on b1 b8) (in b1 c1) (on b3 b1) (in b3 c1) (on b12 b3) (in b12 c1) (clear b12) (base b7) (in b7 c2) (on b7 c2) (on b11 b7) (in b11 c2) (on b14 b11) (in b14 c2) (on b2 b14) (in b2 c2) (on b4 b2) (in b4 c2) (on b6 b4) (in b6 c2) (on b10 b6) (in b10 c2) (clear b10) (clear c3) (clear c4))
 (:goal (and (on b6 c1) (on b2 b6) (on b4 b2) (on b1 b4) (on b14 b1) (on b10 b14) (clear b10) (on b8 c2) (on b12 b8) (on b3 b12) (on b11 b3) (on b9 b11) (on b5 b9) (on b7 b5) (on b13 b7) (clear b13)))
)
