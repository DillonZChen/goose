(define (problem p64-problem)
 (:domain p64-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 12) (= (capacity c3) 8) (= (capacity c4) 8) (base b3) (in b3 c1) (on b3 c1) (on b6 b3) (in b6 c1) (on b18 b6) (in b18 c1) (on b13 b18) (in b13 c1) (on b8 b13) (in b8 c1) (on b14 b8) (in b14 c1) (on b16 b14) (in b16 c1) (on b12 b16) (in b12 c1) (on b15 b12) (in b15 c1) (on b11 b15) (in b11 c1) (on b10 b11) (in b10 c1) (on b5 b10) (in b5 c1) (on b2 b5) (in b2 c1) (on b7 b2) (in b7 c1) (on b9 b7) (in b9 c1) (on b19 b9) (in b19 c1) (on b17 b19) (in b17 c1) (on b1 b17) (in b1 c1) (on b4 b1) (in b4 c1) (clear b4) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b18 c1) (on b7 b18) (on b9 b7) (on b2 b9) (on b17 b2) (on b1 b17) (on b19 b1) (clear b19) (on b13 c2) (on b3 b13) (on b15 b3) (on b4 b15) (on b16 b4) (on b5 b16) (on b10 b5) (on b6 b10) (on b11 b6) (on b12 b11) (on b8 b12) (on b14 b8) (clear b14)))
)
