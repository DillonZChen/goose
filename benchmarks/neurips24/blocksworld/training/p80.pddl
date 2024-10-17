(define (problem p80-problem)
 (:domain p80-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 12) (= (capacity c3) 13) (= (capacity c4) 11) (base b8) (in b8 c1) (on b8 c1) (on b20 b8) (in b20 c1) (on b19 b20) (in b19 c1) (on b14 b19) (in b14 c1) (on b16 b14) (in b16 c1) (on b10 b16) (in b10 c1) (on b5 b10) (in b5 c1) (on b21 b5) (in b21 c1) (on b4 b21) (in b4 c1) (on b18 b4) (in b18 c1) (on b15 b18) (in b15 c1) (on b22 b15) (in b22 c1) (on b1 b22) (in b1 c1) (on b11 b1) (in b11 c1) (on b17 b11) (in b17 c1) (on b6 b17) (in b6 c1) (on b23 b6) (in b23 c1) (on b2 b23) (in b2 c1) (on b13 b2) (in b13 c1) (on b7 b13) (in b7 c1) (on b9 b7) (in b9 c1) (on b3 b9) (in b3 c1) (on b12 b3) (in b12 c1) (on b24 b12) (in b24 c1) (clear b24) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b12 c1) (on b24 b12) (on b1 b24) (on b3 b1) (clear b3) (on b17 c2) (on b5 b17) (on b19 b5) (on b9 b19) (on b16 b9) (on b22 b16) (on b21 b22) (clear b21) (on b2 c3) (on b13 b2) (on b10 b13) (on b6 b10) (on b8 b6) (on b20 b8) (on b4 b20) (on b15 b4) (on b7 b15) (on b23 b7) (on b11 b23) (on b14 b11) (on b18 b14) (clear b18)))
)
