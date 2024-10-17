(define (problem p0_14-problem)
 (:domain p0_14-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 6) (= (capacity c3) 0) (= (capacity c4) 9) (base b12) (in b12 c1) (on b12 c1) (on b15 b12) (in b15 c1) (on b14 b15) (in b14 c1) (clear b14) (base b13) (in b13 c2) (on b13 c2) (on b6 b13) (in b6 c2) (on b4 b6) (in b4 c2) (clear b4) (base b2) (in b2 c3) (on b2 c3) (on b1 b2) (in b1 c3) (on b8 b1) (in b8 c3) (on b7 b8) (in b7 c3) (on b10 b7) (in b10 c3) (on b3 b10) (in b3 c3) (on b5 b3) (in b5 c3) (on b11 b5) (in b11 c3) (on b9 b11) (in b9 c3) (clear b9) (clear c4))
 (:goal (and (on b11 c1) (on b6 b11) (on b3 b6) (on b12 b3) (on b10 b12) (on b5 b10) (clear b5) (on b7 c2) (on b13 b7) (on b15 b13) (on b9 b15) (on b2 b9) (on b8 b2) (on b1 b8) (on b14 b1) (on b4 b14) (clear b4)))
)
