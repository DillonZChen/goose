(define (problem p81-problem)
 (:domain p81-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 8) (= (capacity c3) 4) (= (capacity c4) 3) (= (capacity c5) 12) (base b1) (in b1 c1) (on b1 c1) (on b24 b1) (in b24 c1) (on b8 b24) (in b8 c1) (clear b8) (base b18) (in b18 c2) (on b18 c2) (on b14 b18) (in b14 c2) (on b20 b14) (in b20 c2) (on b11 b20) (in b11 c2) (clear b11) (base b4) (in b4 c3) (on b4 c3) (on b17 b4) (in b17 c3) (on b15 b17) (in b15 c3) (on b12 b15) (in b12 c3) (on b6 b12) (in b6 c3) (on b19 b6) (in b19 c3) (on b2 b19) (in b2 c3) (on b16 b2) (in b16 c3) (clear b16) (base b5) (in b5 c4) (on b5 c4) (on b23 b5) (in b23 c4) (on b3 b23) (in b3 c4) (on b13 b3) (in b13 c4) (on b9 b13) (in b9 c4) (on b21 b9) (in b21 c4) (on b7 b21) (in b7 c4) (on b22 b7) (in b22 c4) (on b10 b22) (in b10 c4) (clear b10) (clear c5))
 (:goal (and (on b23 c1) (on b15 b23) (on b21 b15) (clear b21) (on b17 c2) (on b24 b17) (on b3 b24) (on b9 b3) (on b7 b9) (clear b7) (on b11 c3) (on b14 b11) (on b2 b14) (on b5 b2) (on b16 b5) (clear b16) (on b1 c4) (on b12 b1) (on b6 b12) (on b8 b6) (on b20 b8) (on b19 b20) (on b22 b19) (on b4 b22) (on b13 b4) (on b10 b13) (on b18 b10) (clear b18)))
)
