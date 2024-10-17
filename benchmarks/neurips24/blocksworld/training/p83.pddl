(define (problem p83-problem)
 (:domain p83-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 9) (= (capacity c3) 8) (= (capacity c4) 12) (= (capacity c5) 8) (base b13) (in b13 c1) (on b13 c1) (on b20 b13) (in b20 c1) (on b8 b20) (in b8 c1) (on b14 b8) (in b14 c1) (on b16 b14) (in b16 c1) (on b9 b16) (in b9 c1) (on b18 b9) (in b18 c1) (on b3 b18) (in b3 c1) (on b22 b3) (in b22 c1) (on b11 b22) (in b11 c1) (on b12 b11) (in b12 c1) (on b1 b12) (in b1 c1) (on b2 b1) (in b2 c1) (on b24 b2) (in b24 c1) (on b25 b24) (in b25 c1) (on b19 b25) (in b19 c1) (on b23 b19) (in b23 c1) (on b17 b23) (in b17 c1) (on b5 b17) (in b5 c1) (on b4 b5) (in b4 c1) (on b7 b4) (in b7 c1) (on b21 b7) (in b21 c1) (on b6 b21) (in b6 c1) (on b10 b6) (in b10 c1) (on b15 b10) (in b15 c1) (clear b15) (clear c2) (clear c3) (clear c4) (clear c5))
 (:goal (and (on b14 c1) (clear b14) (on b5 c2) (on b7 b5) (on b9 b7) (on b6 b9) (clear b6) (on b1 c3) (on b20 b1) (on b23 b20) (on b25 b23) (on b21 b25) (on b15 b21) (on b24 b15) (on b4 b24) (clear b4) (on b8 c4) (on b19 b8) (on b12 b19) (on b22 b12) (on b11 b22) (on b17 b11) (on b3 b17) (on b13 b3) (on b18 b13) (on b10 b18) (on b16 b10) (on b2 b16) (clear b2)))
)
