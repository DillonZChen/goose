(define (problem p50-problem)
 (:domain p50-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 13) (= (capacity c2) 0) (= (capacity c3) 5) (= (capacity c4) 4) (base b3) (in b3 c1) (on b3 c1) (on b7 b3) (in b7 c1) (clear b7) (base b13) (in b13 c2) (on b13 c2) (on b5 b13) (in b5 c2) (on b6 b5) (in b6 c2) (on b4 b6) (in b4 c2) (on b12 b4) (in b12 c2) (on b11 b12) (in b11 c2) (on b2 b11) (in b2 c2) (on b14 b2) (in b14 c2) (on b10 b14) (in b10 c2) (on b8 b10) (in b8 c2) (on b15 b8) (in b15 c2) (on b9 b15) (in b9 c2) (on b1 b9) (in b1 c2) (clear b1) (clear c3) (clear c4))
 (:goal (and (on b9 c1) (on b8 b9) (on b4 b8) (on b13 b4) (on b12 b13) (on b2 b12) (on b10 b2) (on b11 b10) (on b6 b11) (on b5 b6) (on b14 b5) (on b1 b14) (on b7 b1) (on b3 b7) (on b15 b3) (clear b15)))
)
