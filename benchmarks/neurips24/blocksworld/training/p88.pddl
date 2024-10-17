(define (problem p88-problem)
 (:domain p88-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 0) (= (capacity c3) 15) (= (capacity c4) 15) (base b6) (in b6 c1) (on b6 c1) (on b13 b6) (in b13 c1) (on b2 b13) (in b2 c1) (on b20 b2) (in b20 c1) (on b8 b20) (in b8 c1) (on b25 b8) (in b25 c1) (clear b25) (base b10) (in b10 c2) (on b10 c2) (on b7 b10) (in b7 c2) (on b4 b7) (in b4 c2) (on b18 b4) (in b18 c2) (on b24 b18) (in b24 c2) (on b11 b24) (in b11 c2) (on b23 b11) (in b23 c2) (on b26 b23) (in b26 c2) (on b19 b26) (in b19 c2) (on b21 b19) (in b21 c2) (on b16 b21) (in b16 c2) (on b1 b16) (in b1 c2) (on b9 b1) (in b9 c2) (on b14 b9) (in b14 c2) (on b3 b14) (in b3 c2) (on b12 b3) (in b12 c2) (on b15 b12) (in b15 c2) (on b5 b15) (in b5 c2) (on b17 b5) (in b17 c2) (on b22 b17) (in b22 c2) (clear b22) (clear c3) (clear c4))
 (:goal (and (on b20 c1) (on b9 b20) (on b16 b9) (on b23 b16) (on b18 b23) (on b26 b18) (on b3 b26) (clear b3) (on b24 c2) (on b19 b24) (on b22 b19) (on b15 b22) (on b10 b15) (on b7 b10) (on b17 b7) (on b5 b17) (on b11 b5) (clear b11) (on b1 c3) (on b2 b1) (on b21 b2) (on b25 b21) (on b13 b25) (on b12 b13) (on b8 b12) (on b4 b8) (on b6 b4) (on b14 b6) (clear b14)))
)
