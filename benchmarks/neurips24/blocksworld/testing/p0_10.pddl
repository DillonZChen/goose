(define (problem p0_10-problem)
 (:domain p0_10-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 8) (= (capacity c3) 5) (= (capacity c4) 5) (base b6) (in b6 c1) (on b6 c1) (on b7 b6) (in b7 c1) (on b9 b7) (in b9 c1) (on b12 b9) (in b12 c1) (on b3 b12) (in b3 c1) (on b10 b3) (in b10 c1) (on b11 b10) (in b11 c1) (on b1 b11) (in b1 c1) (on b8 b1) (in b8 c1) (on b2 b8) (in b2 c1) (on b5 b2) (in b5 c1) (on b4 b5) (in b4 c1) (clear b4) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b11 c1) (on b12 b11) (on b1 b12) (on b4 b1) (clear b4) (on b2 c2) (on b10 b2) (on b5 b10) (on b6 b5) (on b9 b6) (on b3 b9) (on b7 b3) (on b8 b7) (clear b8)))
)
