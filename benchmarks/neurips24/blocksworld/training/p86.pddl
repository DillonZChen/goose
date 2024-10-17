(define (problem p86-problem)
 (:domain p86-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 - block
   c1 c2 c3 c4 c5 c6 c7 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 24) (= (capacity c2) 3) (= (capacity c3) 1) (= (capacity c4) 1) (= (capacity c5) 0) (= (capacity c6) 4) (= (capacity c7) 4) (base b18) (in b18 c1) (on b18 c1) (clear b18) (base b1) (in b1 c2) (on b1 c2) (on b21 b1) (in b21 c2) (clear b21) (base b24) (in b24 c3) (on b24 c3) (on b15 b24) (in b15 c3) (on b23 b15) (in b23 c3) (clear b23) (base b9) (in b9 c4) (on b9 c4) (on b25 b9) (in b25 c4) (on b16 b25) (in b16 c4) (clear b16) (base b12) (in b12 c5) (on b12 c5) (on b8 b12) (in b8 c5) (on b14 b8) (in b14 c5) (on b22 b14) (in b22 c5) (on b17 b22) (in b17 c5) (on b2 b17) (in b2 c5) (on b5 b2) (in b5 c5) (on b3 b5) (in b3 c5) (on b10 b3) (in b10 c5) (on b13 b10) (in b13 c5) (on b20 b13) (in b20 c5) (on b11 b20) (in b11 c5) (on b19 b11) (in b19 c5) (on b4 b19) (in b4 c5) (on b7 b4) (in b7 c5) (on b6 b7) (in b6 c5) (clear b6) (clear c6) (clear c7))
 (:goal (and (on b24 c1) (on b10 b24) (on b17 b10) (on b20 b17) (on b5 b20) (on b21 b5) (on b1 b21) (on b6 b1) (on b16 b6) (on b3 b16) (on b9 b3) (on b2 b9) (on b15 b2) (on b11 b15) (on b25 b11) (on b14 b25) (on b4 b14) (on b18 b4) (on b22 b18) (on b7 b22) (on b8 b7) (on b23 b8) (on b19 b23) (on b13 b19) (on b12 b13) (clear b12)))
)
