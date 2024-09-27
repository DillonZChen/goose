(define (problem p62-problem)
 (:domain p62-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 15) (= (capacity c2) 4) (= (capacity c3) 0) (= (capacity c4) 8) (base b8) (in b8 c1) (on b8 c1) (on b17 b8) (in b17 c1) (on b13 b17) (in b13 c1) (clear b13) (base b16) (in b16 c2) (on b16 c2) (on b10 b16) (in b10 c2) (on b11 b10) (in b11 c2) (on b12 b11) (in b12 c2) (on b1 b12) (in b1 c2) (clear b1) (base b3) (in b3 c3) (on b3 c3) (on b9 b3) (in b9 c3) (on b5 b9) (in b5 c3) (on b6 b5) (in b6 c3) (on b14 b6) (in b14 c3) (on b2 b14) (in b2 c3) (on b18 b2) (in b18 c3) (on b7 b18) (in b7 c3) (on b15 b7) (in b15 c3) (on b4 b15) (in b4 c3) (clear b4) (clear c4))
 (:goal (and (on b11 c1) (on b5 b11) (on b1 b5) (on b16 b1) (on b3 b16) (on b9 b3) (on b4 b9) (on b2 b4) (on b14 b2) (on b6 b14) (on b15 b6) (on b7 b15) (on b13 b7) (on b10 b13) (on b18 b10) (on b17 b18) (on b12 b17) (on b8 b12) (clear b8)))
)
