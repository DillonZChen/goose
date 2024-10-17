(define (problem p67-problem)
 (:domain p67-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 6) (= (capacity c2) 0) (= (capacity c3) 12) (= (capacity c4) 12) (base b3) (in b3 c1) (on b3 c1) (on b12 b3) (in b12 c1) (on b13 b12) (in b13 c1) (on b2 b13) (in b2 c1) (on b20 b2) (in b20 c1) (on b16 b20) (in b16 c1) (clear b16) (base b18) (in b18 c2) (on b18 c2) (on b6 b18) (in b6 c2) (on b4 b6) (in b4 c2) (on b9 b4) (in b9 c2) (on b10 b9) (in b10 c2) (on b7 b10) (in b7 c2) (on b14 b7) (in b14 c2) (on b1 b14) (in b1 c2) (on b11 b1) (in b11 c2) (on b8 b11) (in b8 c2) (on b19 b8) (in b19 c2) (on b5 b19) (in b5 c2) (on b15 b5) (in b15 c2) (on b17 b15) (in b17 c2) (clear b17) (clear c3) (clear c4))
 (:goal (and (on b14 c1) (on b16 b14) (on b6 b16) (on b4 b6) (on b5 b4) (on b3 b5) (clear b3) (on b2 c2) (on b18 b2) (on b1 b18) (on b9 b1) (on b8 b9) (on b19 b8) (clear b19) (on b15 c3) (on b7 b15) (on b20 b7) (on b11 b20) (on b12 b11) (on b10 b12) (on b13 b10) (on b17 b13) (clear b17)))
)
