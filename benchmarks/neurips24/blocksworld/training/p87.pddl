(define (problem p87-problem)
 (:domain p87-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 - block
   c1 c2 c3 c4 c5 c6 c7 c8 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 5) (= (capacity c3) 5) (= (capacity c4) 5) (= (capacity c5) 5) (= (capacity c6) 11) (= (capacity c7) 4) (= (capacity c8) 4) (base b8) (in b8 c1) (on b8 c1) (on b19 b8) (in b19 c1) (on b4 b19) (in b4 c1) (on b11 b4) (in b11 c1) (on b22 b11) (in b22 c1) (on b26 b22) (in b26 c1) (on b20 b26) (in b20 c1) (on b2 b20) (in b2 c1) (on b5 b2) (in b5 c1) (on b17 b5) (in b17 c1) (on b10 b17) (in b10 c1) (on b16 b10) (in b16 c1) (on b25 b16) (in b25 c1) (on b9 b25) (in b9 c1) (on b6 b9) (in b6 c1) (on b12 b6) (in b12 c1) (on b14 b12) (in b14 c1) (on b15 b14) (in b15 c1) (on b21 b15) (in b21 c1) (on b24 b21) (in b24 c1) (on b1 b24) (in b1 c1) (on b18 b1) (in b18 c1) (on b3 b18) (in b3 c1) (on b13 b3) (in b13 c1) (on b7 b13) (in b7 c1) (on b23 b7) (in b23 c1) (clear b23) (clear c2) (clear c3) (clear c4) (clear c5) (clear c6) (clear c7) (clear c8))
 (:goal (and (on b16 c1) (clear b16) (on b24 c2) (on b4 b24) (on b5 b4) (clear b5) (on b2 c3) (on b7 b2) (on b14 b7) (clear b14) (on b1 c4) (on b22 b1) (on b21 b22) (on b12 b21) (clear b12) (on b18 c5) (on b17 b18) (on b6 b17) (on b13 b6) (clear b13) (on b3 c6) (on b9 b3) (on b11 b9) (on b23 b11) (on b25 b23) (on b8 b25) (on b19 b8) (on b15 b19) (on b26 b15) (on b20 b26) (on b10 b20) (clear b10)))
)
