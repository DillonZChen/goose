(define (problem p0_30-problem)
 (:domain p0_30-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 - block
   c1 c2 c3 c4 c5 c6 c7 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 13) (= (capacity c2) 14) (= (capacity c3) 0) (= (capacity c4) 0) (= (capacity c5) 0) (= (capacity c6) 8) (= (capacity c7) 8) (base b18) (in b18 c1) (on b18 c1) (clear b18) (base b14) (in b14 c2) (on b14 c2) (clear b14) (base b23) (in b23 c3) (on b23 c3) (on b24 b23) (in b24 c3) (on b22 b24) (in b22 c3) (on b3 b22) (in b3 c3) (on b27 b3) (in b27 c3) (on b25 b27) (in b25 c3) (on b2 b25) (in b2 c3) (on b28 b2) (in b28 c3) (clear b28) (base b8) (in b8 c4) (on b8 c4) (on b16 b8) (in b16 c4) (on b13 b16) (in b13 c4) (on b19 b13) (in b19 c4) (on b11 b19) (in b11 c4) (on b7 b11) (in b7 c4) (on b17 b7) (in b17 c4) (on b21 b17) (in b21 c4) (on b4 b21) (in b4 c4) (clear b4) (base b12) (in b12 c5) (on b12 c5) (on b15 b12) (in b15 c5) (on b5 b15) (in b5 c5) (on b1 b5) (in b1 c5) (on b20 b1) (in b20 c5) (on b6 b20) (in b6 c5) (on b10 b6) (in b10 c5) (on b26 b10) (in b26 c5) (on b29 b26) (in b29 c5) (on b9 b29) (in b9 c5) (clear b9) (clear c6) (clear c7))
 (:goal (and (on b8 c1) (on b21 b8) (on b27 b21) (on b9 b27) (on b5 b9) (on b11 b5) (on b1 b11) (on b23 b1) (on b29 b23) (on b25 b29) (on b26 b25) (on b17 b26) (on b16 b17) (on b24 b16) (clear b24) (on b7 c2) (on b4 b7) (on b10 b4) (on b6 b10) (on b22 b6) (on b15 b22) (on b13 b15) (on b18 b13) (on b3 b18) (on b12 b3) (on b2 b12) (on b28 b2) (on b19 b28) (on b14 b19) (on b20 b14) (clear b20)))
)
