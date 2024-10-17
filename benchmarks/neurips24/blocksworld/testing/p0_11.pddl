(define (problem p0_11-problem)
 (:domain p0_11-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 7) (= (capacity c3) 6) (= (capacity c4) 6) (base b8) (in b8 c1) (on b8 c1) (on b11 b8) (in b11 c1) (on b7 b11) (in b7 c1) (on b4 b7) (in b4 c1) (on b2 b4) (in b2 c1) (on b6 b2) (in b6 c1) (on b3 b6) (in b3 c1) (on b1 b3) (in b1 c1) (on b12 b1) (in b12 c1) (on b5 b12) (in b5 c1) (on b13 b5) (in b13 c1) (on b9 b13) (in b9 c1) (on b10 b9) (in b10 c1) (clear b10) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b8 c1) (on b2 b8) (on b11 b2) (on b13 b11) (on b5 b13) (on b10 b5) (on b12 b10) (on b9 b12) (on b1 b9) (on b7 b1) (on b4 b7) (on b6 b4) (on b3 b6) (clear b3)))
)
