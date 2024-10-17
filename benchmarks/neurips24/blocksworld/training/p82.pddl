(define (problem p82-problem)
 (:domain p82-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 6) (= (capacity c3) 11) (= (capacity c4) 0) (= (capacity c5) 10) (base b2) (in b2 c1) (on b2 c1) (clear b2) (base b3) (in b3 c2) (on b3 c2) (on b7 b3) (in b7 c2) (on b14 b7) (in b14 c2) (on b1 b14) (in b1 c2) (clear b1) (base b9) (in b9 c3) (on b9 c3) (on b8 b9) (in b8 c3) (on b15 b8) (in b15 c3) (on b22 b15) (in b22 c3) (on b18 b22) (in b18 c3) (clear b18) (base b4) (in b4 c4) (on b4 c4) (on b23 b4) (in b23 c4) (on b16 b23) (in b16 c4) (on b5 b16) (in b5 c4) (on b10 b5) (in b10 c4) (on b24 b10) (in b24 c4) (on b19 b24) (in b19 c4) (on b20 b19) (in b20 c4) (on b17 b20) (in b17 c4) (on b13 b17) (in b13 c4) (on b21 b13) (in b21 c4) (on b11 b21) (in b11 c4) (on b6 b11) (in b6 c4) (on b12 b6) (in b12 c4) (clear b12) (clear c5))
 (:goal (and (on b9 c1) (on b23 b9) (on b7 b23) (clear b7) (on b3 c2) (on b20 b3) (on b14 b20) (on b5 b14) (on b12 b5) (clear b12) (on b15 c3) (on b17 b15) (on b19 b17) (on b4 b19) (on b2 b4) (on b24 b2) (on b8 b24) (on b6 b8) (on b10 b6) (on b11 b10) (on b21 b11) (on b1 b21) (on b13 b1) (on b22 b13) (on b16 b22) (on b18 b16) (clear b18)))
)
