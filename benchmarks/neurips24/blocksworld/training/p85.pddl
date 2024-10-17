(define (problem p85-problem)
 (:domain p85-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 6) (= (capacity c3) 6) (= (capacity c4) 4) (= (capacity c5) 12) (base b16) (in b16 c1) (on b16 c1) (on b22 b16) (in b22 c1) (on b19 b22) (in b19 c1) (clear b19) (base b20) (in b20 c2) (on b20 c2) (on b10 b20) (in b10 c2) (on b15 b10) (in b15 c2) (on b14 b15) (in b14 c2) (on b6 b14) (in b6 c2) (on b12 b6) (in b12 c2) (clear b12) (base b24) (in b24 c3) (on b24 c3) (on b3 b24) (in b3 c3) (on b17 b3) (in b17 c3) (on b2 b17) (in b2 c3) (on b5 b2) (in b5 c3) (on b18 b5) (in b18 c3) (on b13 b18) (in b13 c3) (on b11 b13) (in b11 c3) (clear b11) (base b8) (in b8 c4) (on b8 c4) (on b9 b8) (in b9 c4) (on b25 b9) (in b25 c4) (on b1 b25) (in b1 c4) (on b7 b1) (in b7 c4) (on b21 b7) (in b21 c4) (on b4 b21) (in b4 c4) (on b23 b4) (in b23 c4) (clear b23) (clear c5))
 (:goal (and (on b9 c1) (on b22 b9) (clear b22) (on b17 c2) (on b6 b17) (on b10 b6) (on b18 b10) (on b14 b18) (on b5 b14) (on b25 b5) (on b4 b25) (on b7 b4) (clear b7) (on b23 c3) (on b12 b23) (on b21 b12) (on b3 b21) (on b19 b3) (on b13 b19) (on b15 b13) (on b20 b15) (on b24 b20) (on b2 b24) (on b1 b2) (on b16 b1) (on b8 b16) (on b11 b8) (clear b11)))
)
