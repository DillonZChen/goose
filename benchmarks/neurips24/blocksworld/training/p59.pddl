(define (problem p59-problem)
 (:domain p59-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 0) (= (capacity c3) 12) (= (capacity c4) 9) (base b14) (in b14 c1) (on b14 c1) (on b4 b14) (in b4 c1) (on b8 b4) (in b8 c1) (on b6 b8) (in b6 c1) (on b3 b6) (in b3 c1) (clear b3) (base b1) (in b1 c2) (on b1 c2) (on b11 b1) (in b11 c2) (on b12 b11) (in b12 c2) (on b17 b12) (in b17 c2) (on b13 b17) (in b13 c2) (on b2 b13) (in b2 c2) (on b9 b2) (in b9 c2) (on b5 b9) (in b5 c2) (on b7 b5) (in b7 c2) (on b15 b7) (in b15 c2) (on b16 b15) (in b16 c2) (on b10 b16) (in b10 c2) (clear b10) (clear c3) (clear c4))
 (:goal (and (on b13 c1) (on b15 b13) (clear b15) (on b12 c2) (on b14 b12) (on b7 b14) (clear b7) (on b17 c3) (on b4 b17) (on b11 b4) (on b1 b11) (on b16 b1) (on b3 b16) (on b8 b3) (on b5 b8) (on b6 b5) (on b2 b6) (on b10 b2) (on b9 b10) (clear b9)))
)
