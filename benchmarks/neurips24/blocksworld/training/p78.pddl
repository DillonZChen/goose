(define (problem p78-problem)
 (:domain p78-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 6) (= (capacity c2) 2) (= (capacity c3) 13) (= (capacity c4) 13) (base b18) (in b18 c1) (on b18 c1) (on b20 b18) (in b20 c1) (on b15 b20) (in b15 c1) (on b8 b15) (in b8 c1) (on b9 b8) (in b9 c1) (on b16 b9) (in b16 c1) (on b21 b16) (in b21 c1) (clear b21) (base b19) (in b19 c2) (on b19 c2) (on b12 b19) (in b12 c2) (on b23 b12) (in b23 c2) (on b4 b23) (in b4 c2) (on b1 b4) (in b1 c2) (on b10 b1) (in b10 c2) (on b17 b10) (in b17 c2) (on b11 b17) (in b11 c2) (on b5 b11) (in b5 c2) (on b3 b5) (in b3 c2) (on b6 b3) (in b6 c2) (on b14 b6) (in b14 c2) (on b22 b14) (in b22 c2) (on b2 b22) (in b2 c2) (on b7 b2) (in b7 c2) (on b13 b7) (in b13 c2) (clear b13) (clear c3) (clear c4))
 (:goal (and (on b16 c1) (on b11 b16) (on b21 b11) (on b3 b21) (on b18 b3) (clear b18) (on b17 c2) (on b4 b17) (on b19 b4) (on b14 b19) (on b12 b14) (on b10 b12) (on b9 b10) (on b5 b9) (on b15 b5) (on b2 b15) (on b23 b2) (on b8 b23) (on b7 b8) (on b6 b7) (on b1 b6) (on b22 b1) (on b20 b22) (on b13 b20) (clear b13)))
)
