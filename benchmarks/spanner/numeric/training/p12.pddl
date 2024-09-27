(define (problem spanner-problem)
 (:domain spanner-domain)
 (:objects
   shed location1 location2 location3 - location
   bob - man
 )
 (:init (link shed location1) (link location1 location2) (link location2 location3) (= (spanners_at location1) 3) (= (spanners_at location2) 0) (= (spanners_at location3) 0) (= (spanners_at shed) 0) (= (spanners_at gate) 0) (at bob shed) (link location3 gate)  (= (carrying bob) 0) (= (loose) 3))
 (:goal (and (= (loose) 0)))
)
