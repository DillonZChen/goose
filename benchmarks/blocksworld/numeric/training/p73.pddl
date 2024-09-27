(define (problem p73-problem)
 (:domain p73-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 0) (= (capacity c3) 9) (= (capacity c4) 12) (= (capacity c5) 8) (base b5) (in b5 c1) (on b5 c1) (on b15 b5) (in b15 c1) (on b21 b15) (in b21 c1) (on b6 b21) (in b6 c1) (on b13 b6) (in b13 c1) (clear b13) (base b12) (in b12 c2) (on b12 c2) (on b14 b12) (in b14 c2) (on b9 b14) (in b9 c2) (on b18 b9) (in b18 c2) (on b2 b18) (in b2 c2) (on b20 b2) (in b20 c2) (on b16 b20) (in b16 c2) (on b11 b16) (in b11 c2) (on b8 b11) (in b8 c2) (on b22 b8) (in b22 c2) (on b1 b22) (in b1 c2) (on b19 b1) (in b19 c2) (on b10 b19) (in b10 c2) (on b3 b10) (in b3 c2) (on b7 b3) (in b7 c2) (on b17 b7) (in b17 c2) (on b4 b17) (in b4 c2) (clear b4) (clear c3) (clear c4) (clear c5))
 (:goal (and (on b11 c1) (on b2 b11) (on b12 b2) (clear b12) (on b6 c2) (on b7 b6) (on b22 b7) (clear b22) (on b1 c3) (on b8 b1) (on b15 b8) (on b13 b15) (clear b13) (on b3 c4) (on b10 b3) (on b9 b10) (on b19 b9) (on b14 b19) (on b4 b14) (on b16 b4) (on b5 b16) (on b20 b5) (on b18 b20) (on b17 b18) (on b21 b17) (clear b21)))
)
