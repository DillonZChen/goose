(define (problem p61-problem)
 (:domain p61-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 2) (= (capacity c3) 11) (= (capacity c4) 10) (base b13) (in b13 c1) (on b13 c1) (on b7 b13) (in b7 c1) (on b11 b7) (in b11 c1) (on b15 b11) (in b15 c1) (on b3 b15) (in b3 c1) (on b16 b3) (in b16 c1) (on b1 b16) (in b1 c1) (clear b1) (base b9) (in b9 c2) (on b9 c2) (on b12 b9) (in b12 c2) (on b6 b12) (in b6 c2) (on b8 b6) (in b8 c2) (on b2 b8) (in b2 c2) (on b17 b2) (in b17 c2) (on b10 b17) (in b10 c2) (on b18 b10) (in b18 c2) (on b14 b18) (in b14 c2) (on b5 b14) (in b5 c2) (on b4 b5) (in b4 c2) (clear b4) (clear c3) (clear c4))
 (:goal (and (on b4 c1) (on b2 b4) (on b10 b2) (on b6 b10) (on b3 b6) (clear b3) (on b1 c2) (on b8 b1) (on b15 b8) (on b13 b15) (on b9 b13) (on b18 b9) (on b17 b18) (on b7 b17) (on b14 b7) (on b11 b14) (on b12 b11) (on b5 b12) (on b16 b5) (clear b16)))
)
