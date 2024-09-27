(define (problem p94-problem)
 (:domain p94-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 11) (= (capacity c2) 10) (= (capacity c3) 6) (= (capacity c4) 1) (= (capacity c5) 14) (base b21) (in b21 c1) (on b21 c1) (on b6 b21) (in b6 c1) (on b20 b6) (in b20 c1) (clear b20) (base b3) (in b3 c2) (on b3 c2) (on b7 b3) (in b7 c2) (on b19 b7) (in b19 c2) (on b13 b19) (in b13 c2) (clear b13) (base b17) (in b17 c3) (on b17 c3) (on b23 b17) (in b23 c3) (on b11 b23) (in b11 c3) (on b10 b11) (in b10 c3) (on b22 b10) (in b22 c3) (on b14 b22) (in b14 c3) (on b1 b14) (in b1 c3) (on b26 b1) (in b26 c3) (clear b26) (base b12) (in b12 c4) (on b12 c4) (on b18 b12) (in b18 c4) (on b27 b18) (in b27 c4) (on b28 b27) (in b28 c4) (on b15 b28) (in b15 c4) (on b8 b15) (in b8 c4) (on b4 b8) (in b4 c4) (on b25 b4) (in b25 c4) (on b16 b25) (in b16 c4) (on b2 b16) (in b2 c4) (on b9 b2) (in b9 c4) (on b5 b9) (in b5 c4) (on b24 b5) (in b24 c4) (clear b24) (clear c5))
 (:goal (and (on b2 c1) (on b18 b2) (on b5 b18) (on b7 b5) (on b23 b7) (on b6 b23) (clear b6) (on b15 c2) (on b27 b15) (on b24 b27) (on b11 b24) (on b8 b11) (on b19 b8) (on b9 b19) (on b28 b9) (on b12 b28) (on b26 b12) (clear b26) (on b1 c3) (on b25 b1) (on b13 b25) (on b21 b13) (on b16 b21) (on b4 b16) (on b22 b4) (on b3 b22) (on b17 b3) (on b14 b17) (on b10 b14) (on b20 b10) (clear b20)))
)
