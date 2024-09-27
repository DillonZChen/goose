(define (problem p93-problem)
 (:domain p93-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 - block
   c1 c2 c3 c4 c5 c6 c7 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 8) (= (capacity c3) 3) (= (capacity c4) 2) (= (capacity c5) 0) (= (capacity c6) 9) (= (capacity c7) 9) (base b3) (in b3 c1) (on b3 c1) (clear b3) (base b13) (in b13 c2) (on b13 c2) (on b5 b13) (in b5 c2) (clear b5) (base b10) (in b10 c3) (on b10 c3) (on b23 b10) (in b23 c3) (on b16 b23) (in b16 c3) (on b27 b16) (in b27 c3) (on b1 b27) (in b1 c3) (on b4 b1) (in b4 c3) (on b18 b4) (in b18 c3) (clear b18) (base b17) (in b17 c4) (on b17 c4) (on b8 b17) (in b8 c4) (on b7 b8) (in b7 c4) (on b21 b7) (in b21 c4) (on b19 b21) (in b19 c4) (on b22 b19) (in b22 c4) (on b15 b22) (in b15 c4) (clear b15) (base b11) (in b11 c5) (on b11 c5) (on b25 b11) (in b25 c5) (on b26 b25) (in b26 c5) (on b9 b26) (in b9 c5) (on b12 b9) (in b12 c5) (on b6 b12) (in b6 c5) (on b14 b6) (in b14 c5) (on b24 b14) (in b24 c5) (on b20 b24) (in b20 c5) (on b2 b20) (in b2 c5) (clear b2) (clear c6) (clear c7))
 (:goal (and (on b3 c1) (on b7 b3) (on b8 b7) (on b21 b8) (on b10 b21) (clear b10) (on b22 c2) (on b25 b22) (on b27 b25) (on b18 b27) (on b23 b18) (on b20 b23) (clear b20) (on b17 c3) (on b15 b17) (on b6 b15) (on b1 b6) (on b24 b1) (on b4 b24) (on b5 b4) (on b2 b5) (clear b2) (on b13 c4) (on b14 b13) (on b19 b14) (on b11 b19) (on b16 b11) (on b9 b16) (on b12 b9) (on b26 b12) (clear b26)))
)
