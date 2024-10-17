(define (problem p84-problem)
 (:domain p84-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 14) (= (capacity c2) 8) (= (capacity c3) 0) (= (capacity c4) 15) (base b22) (in b22 c1) (on b22 c1) (clear b22) (base b7) (in b7 c2) (on b7 c2) (on b11 b7) (in b11 c2) (on b16 b11) (in b16 c2) (on b25 b16) (in b25 c2) (on b13 b25) (in b13 c2) (on b14 b13) (in b14 c2) (on b6 b14) (in b6 c2) (clear b6) (base b20) (in b20 c3) (on b20 c3) (on b8 b20) (in b8 c3) (on b3 b8) (in b3 c3) (on b24 b3) (in b24 c3) (on b23 b24) (in b23 c3) (on b18 b23) (in b18 c3) (on b12 b18) (in b12 c3) (on b5 b12) (in b5 c3) (on b17 b5) (in b17 c3) (on b4 b17) (in b4 c3) (on b9 b4) (in b9 c3) (on b19 b9) (in b19 c3) (on b15 b19) (in b15 c3) (on b21 b15) (in b21 c3) (on b1 b21) (in b1 c3) (on b2 b1) (in b2 c3) (on b10 b2) (in b10 c3) (clear b10) (clear c4))
 (:goal (and (on b3 c1) (on b17 b3) (clear b17) (on b21 c2) (on b7 b21) (on b11 b7) (on b12 b11) (on b13 b12) (on b14 b13) (on b2 b14) (clear b2) (on b18 c3) (on b4 b18) (on b23 b4) (on b22 b23) (on b20 b22) (on b5 b20) (on b1 b5) (on b6 b1) (on b16 b6) (on b15 b16) (on b9 b15) (on b10 b9) (on b19 b10) (on b25 b19) (on b24 b25) (on b8 b24) (clear b8)))
)
