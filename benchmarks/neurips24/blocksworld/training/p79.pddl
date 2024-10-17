(define (problem p79-problem)
 (:domain p79-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 3) (= (capacity c3) 13) (= (capacity c4) 13) (base b23) (in b23 c1) (on b23 c1) (on b16 b23) (in b16 c1) (on b1 b16) (in b1 c1) (on b18 b1) (in b18 c1) (on b17 b18) (in b17 c1) (on b22 b17) (in b22 c1) (on b2 b22) (in b2 c1) (on b9 b2) (in b9 c1) (clear b9) (base b13) (in b13 c2) (on b13 c2) (on b19 b13) (in b19 c2) (on b4 b19) (in b4 c2) (on b8 b4) (in b8 c2) (on b6 b8) (in b6 c2) (on b7 b6) (in b7 c2) (on b12 b7) (in b12 c2) (on b5 b12) (in b5 c2) (on b11 b5) (in b11 c2) (on b14 b11) (in b14 c2) (on b15 b14) (in b15 c2) (on b3 b15) (in b3 c2) (on b20 b3) (in b20 c2) (on b10 b20) (in b10 c2) (on b21 b10) (in b21 c2) (clear b21) (clear c3) (clear c4))
 (:goal (and (on b10 c1) (on b20 b10) (on b9 b20) (on b16 b9) (on b15 b16) (clear b15) (on b14 c2) (on b8 b14) (on b5 b8) (on b19 b5) (on b3 b19) (on b17 b3) (on b4 b17) (on b18 b4) (on b12 b18) (on b23 b12) (on b6 b23) (on b22 b6) (on b7 b22) (on b2 b7) (on b13 b2) (on b21 b13) (on b11 b21) (on b1 b11) (clear b1)))
)
