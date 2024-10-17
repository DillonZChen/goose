(define (problem p0_05)
 (:domain spanner)
 (:objects
   shed location1 location2 location3 location4 - location
   bob - man
 )
 (:init (link shed location1) (link location1 location2) (link location2 location3) (link location3 location4) (= (spanners_at location1) 0) (= (spanners_at location2) 0) (= (spanners_at location3) 2) (= (spanners_at location4) 0) (= (spanners_at shed) 0) (= (spanners_at gate) 0) (at bob shed) (link location4 gate)  (= (carrying bob) 0) (= (loose) 1))
 (:goal (and (= (loose) 0)))
)
