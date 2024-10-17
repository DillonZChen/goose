(define (problem p74-problem)
 (:domain p74-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 11) (= (capacity c3) 11) (= (capacity c4) 11) (base b19) (in b19 c1) (on b19 c1) (on b7 b19) (in b7 c1) (on b18 b7) (in b18 c1) (on b12 b18) (in b12 c1) (on b15 b12) (in b15 c1) (on b2 b15) (in b2 c1) (on b21 b2) (in b21 c1) (on b10 b21) (in b10 c1) (on b4 b10) (in b4 c1) (on b5 b4) (in b5 c1) (on b8 b5) (in b8 c1) (on b14 b8) (in b14 c1) (on b6 b14) (in b6 c1) (on b22 b6) (in b22 c1) (on b17 b22) (in b17 c1) (on b3 b17) (in b3 c1) (on b9 b3) (in b9 c1) (on b20 b9) (in b20 c1) (on b1 b20) (in b1 c1) (on b13 b1) (in b13 c1) (on b16 b13) (in b16 c1) (on b11 b16) (in b11 c1) (clear b11) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b20 c1) (on b6 b20) (on b16 b6) (clear b16) (on b13 c2) (on b14 b13) (on b1 b14) (on b15 b1) (on b7 b15) (on b4 b7) (on b12 b4) (on b22 b12) (clear b22) (on b19 c3) (on b21 b19) (on b5 b21) (on b3 b5) (on b10 b3) (on b17 b10) (on b2 b17) (on b8 b2) (on b18 b8) (on b11 b18) (on b9 b11) (clear b9)))
)
