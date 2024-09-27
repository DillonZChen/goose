(define (problem p0_26-problem)
 (:domain p0_26-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 11) (= (capacity c2) 7) (= (capacity c3) 5) (= (capacity c4) 2) (= (capacity c5) 12) (base b17) (in b17 c1) (on b17 c1) (on b3 b17) (in b3 c1) (clear b3) (base b1) (in b1 c2) (on b1 c2) (on b24 b1) (in b24 c2) (on b23 b24) (in b23 c2) (on b4 b23) (in b4 c2) (on b5 b4) (in b5 c2) (on b16 b5) (in b16 c2) (clear b16) (base b20) (in b20 c3) (on b20 c3) (on b8 b20) (in b8 c3) (on b22 b8) (in b22 c3) (on b25 b22) (in b25 c3) (on b9 b25) (in b9 c3) (on b12 b9) (in b12 c3) (on b2 b12) (in b2 c3) (clear b2) (base b13) (in b13 c4) (on b13 c4) (on b21 b13) (in b21 c4) (on b18 b21) (in b18 c4) (on b14 b18) (in b14 c4) (on b6 b14) (in b6 c4) (on b11 b6) (in b11 c4) (on b7 b11) (in b7 c4) (on b15 b7) (in b15 c4) (on b10 b15) (in b10 c4) (on b19 b10) (in b19 c4) (clear b19) (clear c5))
 (:goal (and (on b2 c1) (clear b2) (on b23 c2) (on b9 b23) (on b24 b9) (on b8 b24) (on b22 b8) (on b18 b22) (on b1 b18) (on b4 b1) (on b16 b4) (on b14 b16) (on b11 b14) (on b5 b11) (clear b5) (on b15 c3) (on b21 b15) (on b6 b21) (on b3 b6) (on b12 b3) (on b13 b12) (on b19 b13) (on b20 b19) (on b7 b20) (on b10 b7) (on b17 b10) (on b25 b17) (clear b25)))
)
