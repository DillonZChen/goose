(define (problem p0_22-problem)
 (:domain p0_22-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 18) (= (capacity c2) 0) (= (capacity c3) 8) (= (capacity c4) 7) (base b15) (in b15 c1) (on b15 c1) (on b20 b15) (in b20 c1) (on b13 b20) (in b13 c1) (on b9 b13) (in b9 c1) (clear b9) (base b7) (in b7 c2) (on b7 c2) (on b22 b7) (in b22 c2) (on b12 b22) (in b12 c2) (on b19 b12) (in b19 c2) (on b18 b19) (in b18 c2) (on b4 b18) (in b4 c2) (on b1 b4) (in b1 c2) (on b16 b1) (in b16 c2) (on b2 b16) (in b2 c2) (on b10 b2) (in b10 c2) (on b21 b10) (in b21 c2) (on b5 b21) (in b5 c2) (on b11 b5) (in b11 c2) (on b6 b11) (in b6 c2) (on b14 b6) (in b14 c2) (on b17 b14) (in b17 c2) (on b8 b17) (in b8 c2) (on b3 b8) (in b3 c2) (clear b3) (clear c3) (clear c4))
 (:goal (and (on b14 c1) (on b5 b14) (on b19 b5) (on b9 b19) (on b21 b9) (on b16 b21) (on b6 b16) (on b10 b6) (on b1 b10) (on b8 b1) (on b2 b8) (on b17 b2) (on b22 b17) (on b12 b22) (on b20 b12) (on b11 b20) (on b4 b11) (on b13 b4) (on b15 b13) (on b18 b15) (on b3 b18) (on b7 b3) (clear b7)))
)
