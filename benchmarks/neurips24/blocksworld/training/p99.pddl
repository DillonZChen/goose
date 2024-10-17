(define (problem p99-problem)
 (:domain p99-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 - block
   c1 c2 c3 c4 c5 c6 c7 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 12) (= (capacity c3) 4) (= (capacity c4) 2) (= (capacity c5) 0) (= (capacity c6) 8) (= (capacity c7) 8) (base b24) (in b24 c1) (on b24 c1) (on b22 b24) (in b22 c1) (on b5 b22) (in b5 c1) (on b26 b5) (in b26 c1) (clear b26) (base b19) (in b19 c2) (on b19 c2) (on b2 b19) (in b2 c2) (on b29 b2) (in b29 c2) (on b20 b29) (in b20 c2) (clear b20) (base b13) (in b13 c3) (on b13 c3) (on b1 b13) (in b1 c3) (on b25 b1) (in b25 c3) (on b10 b25) (in b10 c3) (on b14 b10) (in b14 c3) (clear b14) (base b15) (in b15 c4) (on b15 c4) (on b8 b15) (in b8 c4) (on b21 b8) (in b21 c4) (on b6 b21) (in b6 c4) (on b16 b6) (in b16 c4) (on b23 b16) (in b23 c4) (clear b23) (base b28) (in b28 c5) (on b28 c5) (on b12 b28) (in b12 c5) (on b11 b12) (in b11 c5) (on b17 b11) (in b17 c5) (on b9 b17) (in b9 c5) (on b3 b9) (in b3 c5) (on b18 b3) (in b18 c5) (on b4 b18) (in b4 c5) (on b7 b4) (in b7 c5) (on b27 b7) (in b27 c5) (clear b27) (clear c6) (clear c7))
 (:goal (and (on b6 c1) (on b16 b6) (on b19 b16) (on b11 b19) (on b15 b11) (on b14 b15) (on b24 b14) (on b17 b24) (on b9 b17) (on b2 b9) (on b12 b2) (on b1 b12) (on b28 b1) (clear b28) (on b5 c2) (on b8 b5) (on b27 b8) (on b3 b27) (on b13 b3) (on b18 b13) (on b21 b18) (on b20 b21) (on b22 b20) (on b4 b22) (on b23 b4) (on b26 b23) (on b25 b26) (on b10 b25) (on b7 b10) (on b29 b7) (clear b29)))
)
