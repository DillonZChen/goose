(define (problem p0_13-problem)
 (:domain p0_13-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 12) (= (capacity c2) 2) (= (capacity c3) 1) (= (capacity c4) 7) (base b1) (in b1 c1) (on b1 c1) (on b6 b1) (in b6 c1) (on b4 b6) (in b4 c1) (clear b4) (base b3) (in b3 c2) (on b3 c2) (on b10 b3) (in b10 c2) (on b5 b10) (in b5 c2) (on b12 b5) (in b12 c2) (on b14 b12) (in b14 c2) (on b15 b14) (in b15 c2) (clear b15) (base b2) (in b2 c3) (on b2 c3) (on b13 b2) (in b13 c3) (on b8 b13) (in b8 c3) (on b9 b8) (in b9 c3) (on b11 b9) (in b11 c3) (on b7 b11) (in b7 c3) (clear b7) (clear c4))
 (:goal (and (on b3 c1) (on b10 b3) (on b4 b10) (on b9 b4) (on b11 b9) (on b7 b11) (on b14 b7) (on b8 b14) (on b2 b8) (on b13 b2) (on b1 b13) (on b5 b1) (on b15 b5) (on b12 b15) (on b6 b12) (clear b6)))
)
