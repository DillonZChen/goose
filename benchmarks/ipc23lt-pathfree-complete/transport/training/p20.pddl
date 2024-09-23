;; vehicles=3, packages=4, locations=7, max_capacity=2, out_folder=training/easy, instance_id=20, seed=61

(define (problem transport-20)
 (:domain transport)
 (:objects 
    v1 v2 v3 - vehicle
    p1 p2 p3 p4 - package
    l1 l2 l3 l4 l5 l6 l7 - location
    c0 c1 c2 - size
    )
 (:init (capacity v1 c2)
    (capacity v2 c1)
    (capacity v3 c1)
    (capacity-predecessor c0 c1)
    (capacity-predecessor c1 c2)
    (at p1 l7)
    (at p2 l3)
    (at p3 l3)
    (at p4 l3)
    (at v1 l7)
    (at v2 l6)
    (at v3 l1)
    (road l3 l4)
    (road l2 l7)
    (road l1 l5)
    (road l4 l3)
    (road l5 l1)
    (road l5 l7)
    (road l6 l7)
    (road l7 l6)
    (road l7 l2)
    (road l3 l6)
    (road l7 l5)
    (road l6 l3)
    (road l4 l5)
    (road l5 l4)
    (road l1 l7)
    (road l7 l1)
    )
 (:goal  (and 
    (at p1 l6)
    (at p2 l6)
    (at p3 l2)
    (at p4 l7))))
