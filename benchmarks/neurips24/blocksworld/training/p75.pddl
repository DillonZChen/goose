(define (problem p75-problem)
 (:domain p75-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 10) (= (capacity c2) 0) (= (capacity c3) 12) (= (capacity c4) 11) (base b11) (in b11 c1) (on b11 c1) (on b15 b11) (in b15 c1) (clear b15) (base b5) (in b5 c2) (on b5 c2) (on b20 b5) (in b20 c2) (on b12 b20) (in b12 c2) (on b6 b12) (in b6 c2) (on b18 b6) (in b18 c2) (on b19 b18) (in b19 c2) (on b13 b19) (in b13 c2) (on b22 b13) (in b22 c2) (on b10 b22) (in b10 c2) (on b7 b10) (in b7 c2) (on b14 b7) (in b14 c2) (on b21 b14) (in b21 c2) (on b1 b21) (in b1 c2) (on b9 b1) (in b9 c2) (on b3 b9) (in b3 c2) (on b17 b3) (in b17 c2) (on b4 b17) (in b4 c2) (on b16 b4) (in b16 c2) (on b2 b16) (in b2 c2) (on b8 b2) (in b8 c2) (clear b8) (clear c3) (clear c4))
 (:goal (and (on b7 c1) (on b17 b7) (on b14 b17) (on b20 b14) (on b13 b20) (on b5 b13) (clear b5) (on b6 c2) (on b16 b6) (on b4 b16) (on b2 b4) (on b10 b2) (on b19 b10) (on b21 b19) (on b22 b21) (on b3 b22) (on b12 b3) (on b9 b12) (on b11 b9) (on b8 b11) (on b18 b8) (on b15 b18) (on b1 b15) (clear b1)))
)
