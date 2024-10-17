(define (problem p77-problem)
 (:domain p77-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 10) (= (capacity c2) 7) (= (capacity c3) 3) (= (capacity c4) 3) (= (capacity c5) 11) (base b15) (in b15 c1) (on b15 c1) (on b3 b15) (in b3 c1) (clear b3) (base b10) (in b10 c2) (on b10 c2) (on b12 b10) (in b12 c2) (on b17 b12) (in b17 c2) (on b4 b17) (in b4 c2) (on b22 b4) (in b22 c2) (clear b22) (base b2) (in b2 c3) (on b2 c3) (on b14 b2) (in b14 c3) (on b20 b14) (in b20 c3) (on b19 b20) (in b19 c3) (on b6 b19) (in b6 c3) (on b9 b6) (in b9 c3) (on b21 b9) (in b21 c3) (on b16 b21) (in b16 c3) (clear b16) (base b1) (in b1 c4) (on b1 c4) (on b7 b1) (in b7 c4) (on b8 b7) (in b8 c4) (on b13 b8) (in b13 c4) (on b11 b13) (in b11 c4) (on b23 b11) (in b23 c4) (on b5 b23) (in b5 c4) (on b18 b5) (in b18 c4) (clear b18) (clear c5))
 (:goal (and (on b14 c1) (clear b14) (on b8 c2) (on b11 b8) (on b23 b11) (clear b23) (on b21 c3) (on b10 b21) (on b17 b10) (on b5 b17) (on b19 b5) (on b1 b19) (on b22 b1) (on b6 b22) (clear b6) (on b3 c4) (on b12 b3) (on b20 b12) (on b18 b20) (on b7 b18) (on b4 b7) (on b2 b4) (on b15 b2) (on b13 b15) (on b16 b13) (on b9 b16) (clear b9)))
)
